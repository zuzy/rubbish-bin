#ifndef __HALO_HTTP_H__
#define __HALO_HTTP_H__

#define HC_HTTP_DEBUG 0
#if HC_HTTP_DEBUG
    #define print_http(format, arg...)   do { printf("\033[31m[halo_http]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                        while (0)
#else
    #define print_http(format, arg...)   NULL
#endif

#define MAX_KEY_LEN 128
#define MAX_URL_LEN 1024
#define MAX_BODY_LEN 1024

typedef int (*http_end_cb)(void*);

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

typedef int (*post_result_callback)(void *result, void *loop);

typedef struct _st_post
{
    struct list_head list;
    char *uri;
    struct list_head *heads;
    char *payload;
    char *result;
    post_result_callback result_cb;
}st_post;

/** 
 * @brief  init and add a hash obj to a list
 * @note   
 * @param  *heads: target list
 * @param  *key: hash obj key
 * @param  *val: hash obj value
 * @retval success: 0
 */
int add_hash(struct list_head *heads, char *key, char *val);

/** 
 * @brief  delete and free a hash object
 * @note   
 * @param  *hash: target hash 
 * @retval success: 0
 */
int del_hash(st_hash *hash);

/** 
 * @brief  free a hash list
 * @note   
 * @param  *head: target hash list
 * @retval success 0
 */
int free_hash(struct list_head *head);

/** 
 * @brief  init and add a post obj to a list; 
 *          post will not be added if url & payload are both existed;
 * @note   
 * @param  *posts: target list
 * @param  *url: post url
 * @param  *heads: a hash list of post header; 
 *                  if null content-type->x-www-format will be setted
 * @param  *payload: the body of post
 * @param  callback: the result dispatch callback; 
 *                  dispatch the result code & body;
 *                  posts->list cannot get acted dynamicly.
 * @retval success:0
 */
int add_post(   struct list_head *posts, 
                char *url, 
                struct list_head *heads, 
                char *payload, 
                post_result_callback callback);

/** 
 * @brief  init and add a post obj to a list;
 *          post will not judge anything; just add back at the post obj
 * @note   null
 * @param  *post: target post obj; new obj will add back of it.
 * @param  *url: post url
 * @param  *heads: a hash list of post header; 
 *                  if null content-type->x-www-format will be setted
 * @param  *payload: the body of post
 * @param  callback: the result dispatch callback;
 *                  dispatch the result code & body;
 *                  ** posts->list will act imediately;
 *                  so new post can be added dynamicly in the cb.
 * @retval success:0
 */
int add_post_dynamic(   st_post *post,
                        char *url,
                        struct list_head *heads,
                        char *payload,
                        post_result_callback callback);

/** 
 * @brief  init and add a post obj to a list;
 *          base on add_post
 * @note   
 * @param  *posts: 
 * @param  *url: 
 * @param  *heads: 
 * @param  *bodies: x-www-format hash list
 * @param  callback: 
 * @retval 
 */
int add_post_hash_bodies(struct list_head *posts, 
                        char *url, 
                        struct list_head *heads, 
                        struct list_head *bodies, 
                        post_result_callback callback);

/** 
 * @brief  init and add a post obj to a list;
 *          base on add_post_dynamic
 * @note   
 * @param  *post: 
 * @param  *url: 
 * @param  *heads: 
 * @param  *bodies: x-www-format hash list
 * @param  callback: 
 * @retval 
 */
int add_post_hash_bodies_dy(st_post *post, 
                        char *url, 
                        struct list_head *heads, 
                        struct list_head *bodies, 
                        post_result_callback callback);

/** 
 * @brief  init the http obj;
 * @note   
 * @param  *posts: posts list
 * @param  retry: retry times, if <0 means unlimit retry;
 * @param  detach: is_detach the thread
 * @param  callback: cb will be called when http get success
 * @param  *arg: the arg of callback
 * @retval success:0
 */
int init_http(struct list_head *posts, 
                int retry, 
                int detach, 
                http_end_cb callback,
                void *arg);

/** 
 * @brief  start to post the posts list
 * @note   
 * @retval success:0
 */
int halo_http_start();

/** 
 * @brief  wait for the post finish and get the result
 * @note   
 * @retval success:0
 *          thread is still running: 1
 *          failed  -1;
 */
int halo_http_result();

/** 
 * @brief  delete and free the http obj
 * @note   
 * @retval success:0
 */
int deinit_http();


int is_http_busy();


#endif