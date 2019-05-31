// #include <pthread.h>
// #include <applib/msg_loop.h>
// #include <case_msg.h>
// #include <applib/service_api.h>
// #include <applib/app_timer.h>
// #include <act_debug.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include "halo_utf8.h"

// #include "applib.h"
// #include "act_debug.h"
// #include "net/config.h"
// #include "msg_server_api.h"
// #include "net/wifi_manager.h"

#ifndef off64_t
typedef unsigned long long off64_t;
#endif

// #include "ghttp/ghttp.h" 
// #include "cJSON.h"
// #include "md5.h"
// #include "common.h"
// #include "hc_http.h"

// #include "media_navi_api.h"
// #include "smart_player_api.h"
// #include "smart_player_hal.h"

// #include "music_player_api.h"

// #include "halo_utf8.h"
#include "list.h"

#define HC_HTTP_DEBUG 1
#if HC_HTTP_DEBUG
    #define print_http(format, arg...)   do { printf("\033[31m[-hc_http-]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                        while (0)
#else
    #define print_http(format, arg...)   do {} while (0)
#endif


#define MAX_KEY_LEN 128
#define MAX_URL_LEN 1024
#define MAX_BODY_LEN 1024

typedef struct _st_hash
{
    struct list_head list;
    char *key;
    char *value;
}st_hash;

typedef struct _st_result
{
    int code;
    char body[MAX_BODY_LEN];
}st_result;

typedef int (*post_result_callback)(void *);

typedef struct _st_post
{
    struct list_head list;
    char *uri;
    struct list_head *heads;
    char *payload;
    char *result;
    post_result_callback result_cb;
}st_post;

typedef struct _st_http
{
    pthread_t tid;
    struct list_head *posts;
    st_result result;
    int retry;
    int detach;
    // ghttp_request *req;
    // ghttp_status status;
}st_http;

static st_http *s_handle = NULL;

static int __add_hash(struct list_head *heads, st_hash *hash)
{
    st_hash *tmp;
    struct list_head *h = heads;
    if(h == NULL) {
        return -1;
    }
    list_for_each_entry(tmp, h, list) {
        if(strcmp(hash->key, tmp->key) == 0) {
            printf("key:%s is allready exist!\n", hash->key);
            return 0;
        }
    }
    list_add_tail(&hash->list, h);
    return 0;
}

int add_hash(struct list_head *heads, char *key, char *val)
{
    st_hash *hash = (st_hash *)malloc(sizeof(st_hash));
    if(hash == NULL) {
        printf("malloc failed \n");
        return -1;
    }
    hash->key = (char*)malloc(strlen(key) + 1);
    strcpy(hash->key, key);
    hash->value = (char*)malloc(strlen(val) + 1);
    strcpy(hash->value, val);
    return __add_hash(heads, hash);
}

static int __add_post(struct list_head *posts, st_post *post)
{
    st_post *p, *tmp;
    struct list_head *h = posts;
    if(post->uri == NULL || post->payload == NULL) {
        printf("POST IS NOT PREPARED!! %s %s\n", post->uri,  post->payload);
        return -1;
    }
    list_for_each_entry_safe(p, tmp, h, list) {
        if(strncmp(p->uri, post->uri, MAX_URL_LEN) == 0){
            if(strcmp(post->payload, p->payload) == 0) {
                printf("uri:%s, payload:%s are all same!\n", post->uri, post->payload);
                return 0;
            }
        }
    }
    list_add_tail(&post->list, h);
    return 0;
}

int add_post(   struct list_head *posts, 
                char *url, 
                struct list_head *heads, 
                char *payload, 
                post_result_callback callback)
{
    if(payload == NULL || url == NULL) {
        printf("body is illegeal\n");
        return -1;
    }
    st_post *post = (st_post *)malloc(sizeof(st_post));
    if(post == NULL) {
        return -1;
    }
    post->uri = (char*)malloc(strlen(url) + 1);
    strcpy(post->uri, url);
    post->heads = heads;
    post->payload = (char*)malloc(strlen(payload) + 1);
    strcpy(post->payload, payload);
    post->result_cb = callback;

    return __add_post(posts, post);
}

static int __bodies_to_payload(char *payload, struct list_head *bodies, size_t len)
{
    st_hash *tmp;
    list_for_each_entry(tmp, bodies, list) {
        add_x_www_couple(payload, tmp->key, tmp->value, len);
        if(tmp->key != NULL) {
            free(tmp->key);
            tmp->key = NULL;
        }
        if(tmp->value != NULL) {
            free(tmp->value);
            tmp->value = NULL;
        }
    }
    return 0;
}

int add_post_hash_bodies(struct list_head *posts, 
                        char *url, 
                        struct list_head *heads, 
                        struct list_head *bodies, 
                        post_result_callback callback)
{
    st_hash *tmp;
    size_t len = 0;
    list_for_each_entry(tmp, bodies, list) {
        len += sizeof_x_www_format(tmp->key) + sizeof_x_www_format(tmp->value) + 1;
    }
    // len += 2;
    char *payload = (char *)malloc(len);
    if(payload == NULL) {
        printf("error\n");
        return -1;
    }
    memset(payload, 0, len);
    if(__bodies_to_payload(payload, bodies, len)) {
        print_http("error");
        return -1;
    }
    print_http("payload get %s", payload);
    return add_post(posts, url, heads, payload,callback);
}

int init_http(struct list_head *posts, int retry, int detach)
{
    s_handle = (st_http *)malloc(sizeof(st_http));
    if(s_handle == NULL) {
        print_http("http handle malloc failed");
        return -1;
    }
    s_handle->posts = posts;
    s_handle->retry = retry;
    s_handle->detach = detach;
    // s_handle->req = NULL;
    return 0;
}

static int __http_interrupt(void *param){
    return 0;
}


static void *__http_proc(void *arg)
{
    arg = NULL;
    if(s_handle->detach) {
        pthread_detach(pthread_self());
    }
    st_post *tmp = NULL;
    int times = 0;
    int ret = 0;
    int r = 23;
    

    // wifi_status_e wifi_status;
	// hotplug_get_wifi_status(&wifi_status);
    // if(wifi_status != WIFI_STATE_CONNECTED){
    //     return -1;
    // }

    list_for_each_entry(tmp, s_handle->posts, list) {
        if(tmp->uri == NULL || tmp->payload == NULL) {
            continue;
        }
        times = 0;
    Retry:
        // hotplug_get_wifi_status(&wifi_status);
        // if(wifi_status != WIFI_STATE_CONNECTED){
        //     return -1;
        // }
        if(++times > s_handle->retry) {
            print_http("retry too much");
            sleep(1);
            pthread_exit(-1);
        }
        // goto Retry;
        print_http("uri:%s", tmp->uri);
        print_http("head:");
        if(tmp->heads != NULL) {
            st_hash *ptr;
            list_for_each_entry(ptr, tmp->heads, list) {
                printf("\t%s -> %s\n", ptr->key, ptr->value);
            }
        } else {
            printf("\tContent-Type -> application/x-www-form-urlencoded\n");
        }
        print_http("body is %s", tmp->payload);


        // goto Retry;

        // if(s_handle->req != NULL) {
        //     ghttp_request_destroy(s_handle->req);
        //     s_handle->req = NULL;
        // }
        // s_handle->req = ghttp_request_new(__http_interrupt, NULL);
        // if(ghttp_set_uri(tmp->uri)) {
        //     print_http("set uri %s failed", tmp->uri);
        //     goto Retry;
        // }
        // if(ghttp_set_type(s_handle->req, ghttp_type_post)) {
        //     print_http("set post type failed");
        //     goto Retry;
        // }
        // if(tmp->heads == NULL) {
        //     if(ghttp_set_header(s_handle->req, "Content-Type", "application/x-www-form-urlencoded")){
        //         print_http("set header failed");
        //         goto Retry;
        //     }
        // } else {
        //     st_hash *p_header;
        //     list_for_each_entry(p_header, tmp->heads, list) {
        //         if(ghttp_set_header(s_handle->req, p_header->key, p_header->value)) {
        //             print_http("set header(%s) failed", p_header->key);
        //             goto Retry;
        //         }
        //     }
        // }
        // if(ghttp_set_body(s_handle->req, tmp->payload, strlen(tmp->payload))) {
        //     print_http("set body failed!\n%s", tm->payload);
        //     goto Retry;
        // }
        // if(ghttp_prepare(s_handle->req)) {
        //     print_http("prepare failed");
        //     goto Retry;
        // }
        // s_handle->status = ghttp_connect(req, 0ll, 0ll);
        // if((s_handle->status != ghttp_done) && (s_handle->status != ghttp_error_no_support_range)){
        //     print_http("post_failed!!");
        //     goto Retry;
        // }
        // s_handle->result.code = ghttp_status_code(s_handle->req);
        // if(s_handle->result.code != 200) {
        //     print_http("http post failed with %d", s_handle->result.code);
        //     goto Retry;
        // }
        // ghttp_read(s_handle->req, s_handle->result.body, MAX_BODY_LEN);
        // ret = tmp->result_cb(&s_handle->result);
        // if(ret != 0) {
        //     goto Retry;
        // }
    }
    sleep(2);
    pthread_exit(1);
    // return NULL;
}

int halo_http_start()
{
    pthread_create(&s_handle->tid, NULL, __http_proc, NULL);
}

int halo_http_result()
{
    void *tret;
    pthread_join(s_handle->tid, &tret);
    return tret;
}

int deinit_http()
{
    if(s_handle == NULL) {
        return 0;
    }
    if(s_handle->posts != NULL) {
        st_post *p_post, *tttmp;
        list_for_each_entry_safe(p_post, tttmp, s_handle->posts, list) {
            if(p_post->uri != NULL) {
                free(p_post->uri);
                p_post->uri = NULL;
            }
            // continue;
            // print_http("u:%s", p_post->uri);
            if(p_post->heads != NULL) {
                st_hash *p_head, *ttttt;
                list_for_each_entry_safe(p_head, ttttt, p_post->heads, list) {
                    if(p_head->key != NULL) {
                        free(p_head->key);
                        p_head->key = NULL;
                    }
                    if(p_head->value != NULL) {
                        free(p_head->value);
                        p_head->value = NULL;
                    }
                    list_del(&p_head->list);
                    free(p_head);
                    p_head = NULL;
                }
            }

            // if(strlen(p_post->payload)){
            if(p_post->payload != NULL) {
                free(p_post->payload);
                p_post->payload = NULL;
            }
            list_del(&p_post->list);
            free(p_post);
            p_post = NULL;
        }
        // if(s_handle->req != NULL) {
        //     ghttp_request_destroy(s_handle->req);
        // }
    }
}

int main()
{
    struct list_head heads = LIST_HEAD_INIT(heads);
    char *key = (char*)malloc(1024);
    char *val = (char*)malloc(1024);
    sprintf(key, "kye1");
    sprintf(val, "val1");
    add_hash(&heads, key, val);
    memset(key, 0, 1024);
    sprintf(key, "key2");
    memset(val, 0, 1024);
    sprintf(val, "val2");
    add_hash(&heads, key, val);

    struct list_head posts = LIST_HEAD_INIT(posts);
    char *pay = (char*)malloc(1024);
    sprintf(pay, "hhherwre");
    add_post(&posts, "uri1", &heads, pay, NULL);
    char *pay2 = (char*)malloc(1024);
    sprintf(pay2, "sdfwerwe");
    add_post(&posts, "uri2", &heads, pay, NULL);

    struct list_head bodies = LIST_HEAD_INIT(bodies);
    add_hash(&bodies, "body1", "val1");
    add_hash(&bodies, "ver", "1.0");
    add_hash(&bodies, "body2", "撒的方式的方式而");
    add_post_hash_bodies(&posts, "url3", NULL, &bodies, NULL);
    init_http(&posts, 3, 0);
    halo_http_start();
    // sleep(5);
    int re = halo_http_result();
    print_http("result: %d", re);
    print_http("result: %d", halo_http_result());
    printf("----------------\n\n");
    // return 0;
    deinit_http();
    return 0;
}