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

#include "list.h"

#define MAX_KEY_LEN 128
#define MAX_URL_LEN 1024

typedef struct _st_hash
{
    struct list_head list;
    char *key;
    char *value;
}st_hash;

typedef struct _st_result
{
    int code;
    char body[1024];
}st_result;

typedef int (*post_result_callback)(void *);

typedef struct _st_post
{
    struct list_head list;
    char* uri;
    struct list_head *heads;
    struct list_head *bodies;
    char *payload;
    char *result;
    post_result_callback result_cb;
}st_post;

typedef struct _st_http
{
    pthread_t tid;
    struct list_head posts;
    struct _st_result result;
    int retry;
}st_http;

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
    hash->key = key;
    hash->value = val;
    return __add_hash(heads, hash);
}


static int __add_post(struct list_head *posts, st_post *post)
{
    st_post *p, *tmp;
    struct list_head *h = posts;
    if(post->uri == NULL || (post->bodies == NULL && post->payload == NULL)) {
        printf("POST IS NOT PREPARED!! %s %d %s\n", post->uri, post->bodies, post->payload);
        return -1;
    }
    list_for_each_entry_safe(p, tmp, h, list) {
        if(strncmp(p->uri, post->uri, MAX_URL_LEN) == 0){
            if(post->bodies != NULL) {
                if(post->bodies == p->bodies) {
                    printf("uri:%s, body:%d are all same!\n", post->uri, post->bodies);
                    return 0;
                }
            } else {
                if(strcmp(post->payload, p->payload) == 0) {
                    printf("uri:%s, payload:%s are all same!\n", post->uri, post->payload);
                    return 0;
                }
            }
        }
    }
    list_add_tail(&post->list, h);
    return 0;
}

int add_post(struct list_head *posts, char *url, struct list_head *heads, char *payload, post_result_callback callback)
{
    if(payload == NULL || url == NULL) {
        printf("body is illegeal\n");
        return -1;
    }
    st_post *post = (st_post *)malloc(sizeof(st_post));
    if(post == NULL) {
        return -1;
    }
    post->uri = url;
    post->heads = heads;
    post->payload = payload;
    post->result_cb = callback;

    return __add_post(posts, post);
}

size_t sizeof_x_www_format(char *str)
{
    size_t len = 0;
    char *ptr = str;
    while(*ptr != '\0') {
        if(*ptr == ' ' || *ptr == '.' || *ptr == '-' || *ptr == '_' || (*ptr <= 'z' && *ptr >= 'a') || (*ptr <= 'Z' && *ptr >= 'A') || (*ptr <= '9' && *ptr >= '0')) {
            ++len;
        } else {
            len += 3;
        }
        ++ptr;
    }
    ++len;
    return len;
}

static char _num_to_hex(unsigned char num)
{
    char out = 0;
    num &= 0x0f;
    if(num >= 10) {
        out = 'A' + num - 10;
    } else {
        out = '0' + num;
    }
    return out;
}

int add_x_www_couple(char *des, char *key, char *body, int len)
{
    #define JUDGE(d, p, l)    (((p) - (d)) >= (l))
    if(key[0] == 0 || body[0] == 0 || len <= 0){
        printf("body set illegal\n");
        return -1;
    }
    char *ori = des + strlen(des);
    char *ptr = ori;
    char *p_k = key;
    char *p_b = body;
    if(ptr != des) {
        *ptr++ = '&';
    }
    while(*p_k != '\0') {
        if((*p_k <= 'z' && *p_k >= 'a') || (*p_k <= 'Z' && *p_k >= 'A') || (*p_k <= '9' && *p_k >= '0') || *p_k == '.' || *p_k == '-' || *p_k == '_') {
            *ptr++ = *p_k++;
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        } else if(*p_k == ' ') {
            *ptr++ = '+';
            ++p_k;
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        } else {
            *ptr++ = '%';
            if(JUDGE(des, ptr, len)){
                goto Error;
            }
            *ptr++ = _num_to_hex(*p_k >> 4);
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
            *ptr++ = _num_to_hex(*p_k++);
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        }
    }

    *ptr++ = '=';
    if(JUDGE(des, ptr, len)) {
        goto Error;
    }

    while(*p_b != '\0'){
        if((*p_b <= 'z' && *p_b >= 'a') || (*p_b <= 'Z' && *p_b >= 'A') || (*p_b <= '9' && *p_b >= '0') || *p_b == '.' || *p_b == '-' || *p_b == '_'){
            *ptr++ = *p_b++;
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        }else if(*p_b == ' '){
            *ptr++ = '+';
            ++p_b;
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        }else{
            *ptr++ = '%';
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
            *ptr++ = _num_to_hex(*p_b >> 4);
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
            *ptr++ = _num_to_hex(*p_b++);
            if(JUDGE(des, ptr, len)) {
                goto Error;
            }
        }
    }
    return 0;
Error:
    printf("body set error! reset the body\n");
    memset(ori, 0, len - (ori - des));
    return -1;
} 


int bodies_to_payload(char *payload, struct list_head *bodies, size_t len)
{
    st_hash *tmp;
    list_for_each_entry(tmp, bodies, list) {
        if(add_x_www_couple(payload, tmp->key, tmp->value, len)) {
            return -1;
        }
    }
    return 0;
}

int add_post_hash_bodies(struct list_head *posts, char *url, struct list_head *heads, struct list_head *bodies, post_result_callback callback)
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
    printf("malloc %d to payload\n", len);
    memset(payload, 0, len);
    if(bodies_to_payload(payload, bodies, len)) {
        return -1;
    }
    add_post(posts, url, heads, payload,callback);
}

void *proc(void *arg)
{
    int tt = 2;
    do{
        struct list_head heads = LIST_HEAD_INIT(heads);
        add_hash(&heads, "key1", "val1");
        add_hash(&heads, "key2", "val2");

        struct list_head posts = LIST_HEAD_INIT(posts);
        add_post(&posts, "uri1", &heads, "hello", NULL);
        add_post(&posts, "uri2", &heads, "hello", NULL);

        struct list_head bodies = LIST_HEAD_INIT(bodies);
        add_hash(&bodies, "body1", "val1");
        add_hash(&bodies, "键", "值");
        add_hash(&bodies, "body2", "撒的方式的方式而");
        add_post_hash_bodies(&posts, "url3", NULL, &bodies, NULL);
        st_hash *tmp;
        st_post *ptr;
        list_for_each_entry(ptr, &posts, list) {
            printf("-----------\nuri: %s\n", ptr->uri);
            if(ptr->heads) {
                printf("head:\n");
                list_for_each_entry(tmp, ptr->heads, list) {
                    printf("\t%s: %s\n", tmp->key, tmp->value);
                }
            } else {
                printf("head: null\n");
            }
            if(ptr->bodies != NULL) {
                printf("bodies:\n");
                list_for_each_entry(tmp, ptr->bodies, list) {
                    printf("\t%s: %s\n", tmp->key, tmp->value);
                }
            } else {
                printf("payload: %s\n", ptr->payload);
            }
            printf("--------------------\n");
        }
        sleep(2);
        // while(1) {
        //     sleep(1);
        // }
    }while(tt--);
    return 1;
}

int main()
{
    // struct list_head heads = LIST_HEAD_INIT(heads);
    // add_hash(&heads, "key1", "val1");
    // add_hash(&heads, "key2", "val2");

    // struct list_head posts = LIST_HEAD_INIT(posts);
    // add_post(&posts, "uri1", &heads, "hello", NULL);
    // add_post(&posts, "uri2", &heads, "hello", NULL);

    // struct list_head bodies = LIST_HEAD_INIT(bodies);
    // add_hash(&bodies, "body1", "val1");
    // add_hash(&bodies, "键", "值");
    // add_hash(&bodies, "body2", "撒的方式的方式而");
    // add_post_hash_bodies(&posts, "url3", NULL, &bodies, NULL);
    // st_hash *tmp;
    // st_post *ptr;
    // list_for_each_entry(ptr, &posts, list) {
    //     printf("-----------\nuri: %s\n", ptr->uri);
    //     if(ptr->heads) {
    //         printf("head:\n");
    //         list_for_each_entry(tmp, ptr->heads, list) {
    //             printf("\t%s: %s\n", tmp->key, tmp->value);
    //         }
    //     } else {
    //         printf("head: null\n");
    //     }
    //     if(ptr->bodies != NULL) {
    //         printf("bodies:\n");
    //         list_for_each_entry(tmp, ptr->bodies, list) {
    //             printf("\t%s: %s\n", tmp->key, tmp->value);
    //         }
    //     } else {
    //         printf("payload: %s\n", ptr->payload);
    //     }
    //     printf("--------------------\n");
    // }
    pthread_t tid;
    void *tret;
    pthread_create(&tid, NULL, proc, NULL);
    struct timespec t = {1, 0};
    // int err = pthread_join(tid, &tret);
    int err;
    while(1) {
        err = pthread_tryjoin_np(tid, &tret);
        // err = pthread_timedjoin_np(tid, &tret, &t);
        if(err == 0){
            printf("\n try join ok\n");
            break;
        }
        if(errno == EBUSY || errno == ETIMEDOUT) {
            printf("\n wait for thread terminate\n");
        } 
        printf("hello %d %d %d\n", err, errno, tid);

        usleep(200000);
    }
    int a = (int)tret;
    printf("err: %d %d\n", err, a);
    return 0;
}