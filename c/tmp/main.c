#include<stdio.h>      /*标准输入输出定义*/    
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    
#include<errno.h>      /*错误号定义*/    
#include<string.h>    
#include <stdarg.h>
#include <pthread.h>

char *sss = "{ \
    \"code\": 100000, \
    \"message\": \"成功！\", \
    \"object\": { \
        \"authCode\": \"2B6DFE887320407D9F64B634415E4D01\", \
        \"refrenceId\": -7540578269179781 \
    } \
}";

static char _ref_ori[21];

unsigned char my_htoi(char *input){
    unsigned char ret = 0;
    char *str = input;
    while(*str){
        ret <<= 4;
        if(*str <= '9' && *str >= '0'){
            ret += *str-'0';
        }else if(*str <= 'f' && *str >= 'a'){
            ret += 10 + *str - 'a';
        }else if(*str <= 'F' && *str >= 'A'){
            ret += 10 + *str - 'A';
        }else{
            break;
        }
        ++str;
    }
    return ret;
}

int get_ref_from_json(char *body, unsigned char *ref, char *ref_str){
    int i, size;
    char *ptr = NULL;
    printf("start\n");
    ptr = strstr(body, "refrenceId\":");
    printf("%s\n", ptr);
    printf("ene\n");
    if(ptr == NULL){
        printf("null\n");
        return -1;
    }
    printf("begin:\n");
    while(*ptr != '-' && (*ptr > '9' || *ptr < '0')){
        // printf("++");
        ++ptr;
    }
    char *p_ref = _ref_ori;
    while((*ptr <= '9' && *ptr >= '0') || *ptr == '-'){
        *p_ref++ = *ptr++;
    }
    printf("%s\n", _ref_ori);
    p_ref = _ref_ori;
    char *p_ref_str = ref_str;
    int j;
    int len = strlen(p_ref);
    printf("len: %d\n", len);
    if(p_ref[0] != '-'){
        if(len > 20){

        }else {
            if(len < 20){
                int i , size;
                size = 20 - len;
                for(i = 0; i < size; i++){
                    *p_ref_str++ = 'A';
                }
            }
            strcpy(p_ref_str, p_ref);
        }
    }else{
        if(len > 20){

        }else{
            if(len < 20){
                int size = 21 - len; // 20 - len + 1 the '-'
                int i;
                for(i = 0; i < size; i++){
                    *p_ref_str++ = 'F';
                }
                strcpy(p_ref_str, p_ref+1);
            }else{
                strcpy(p_ref_str, p_ref);
                p_ref_str[0] = 'F';
            }
        }
    }

    char hex_num[3] = {0};
    for(i = 0; i < 10; i++){
        hex_num[0] = ref_str[(i<<1)];
        hex_num[1] = ref_str[(i<<1)+1];
        ref[i] = my_htoi(hex_num);
    }
}

typedef int (*callback)(void *);
typedef struct _st_dis
{
    char        name[128];
    callback    dis_cb;
}st_dis;

#define CREATE(cmd) {#cmd, (callback)tcp_ser_##cmd##_cb}

int tcp_ser_info_cb(void *arg)
{
    printf("info cb\n");
    return 0;
}

int tcp_ser_cmd_cb(void *arg)
{
    printf("cmd cb\n");
    return 0;
}

st_dis ddd[] = {
    CREATE(info),
    CREATE(cmd),
};

#if 0
void main(){

    u_int8_t a = 0xff;
    u_int8_t b = 0x01;
    u_int8_t c = 0x02;
    printf("a = %.X\n", a);
    a &= ~b;
    printf("a = %.X\n", a);
    a &= ~c;

    // a &= ~b & ~c;
    printf("a = %.X\n", a);
    // unsigned char* ref = (unsigned char *)malloc(20);
    // char *ref_str = (char*)malloc(100);
    // get_ref_from_json(sss, ref, ref_str);
    // printf("%s\n\n", ref_str);
    // int i = 0;
    // for(i; i < 10; i++){
    //     printf("%.02X\t", ref[i]);
    // }
    // printf("\n");

#if 0
    int i, j;
    for(i = 0; i < 10; i++) {
        printf("%d:", i);
        for(j = 0; j < 5; j++) {
            printf(" %d", j);
            if(i + j > 5){
                printf("\t%d + %d > 5", i, j);
                break;
            }
        }
        printf("\n");
    }
    return ;
#else
    ddd[0].dis_cb(NULL);
    ddd[1].dis_cb(NULL);
#endif
}
#endif

// #define LUA_CBS_BUILD(fun, cb, format)      int fun(int a){return cb(a);}

#define LUA_CBS_BUILD(fun, cb, format)      \
int fun(int a){return cb(a);}

static char *_get_member(char *obj)
{
    char *out = NULL;
    out = strrchr(obj, '.');
    return ++out;
}

#define GETBODY(N) _get_member(#N)

struct _st_demo
{
    int a;
    int b;
    int c;
};

int plus(int a)
{
    return ++a;
}

LUA_CBS_BUILD(pplus, plus, "bb")



int get_music_name(char *uri, char *name, int maxlen)
{
    char *ptr = uri;
    char *tail;
    ptr = strrchr(uri, '/');
    ++ptr;
    tail = strrchr(uri, '.');
    if(ptr && tail && tail-ptr) {
        strncpy(name, ptr, (tail-ptr > maxlen) ? maxlen : tail-ptr);
        // print_zizy("get name %s[end]",name);
    }
    return 0;
}

void mvforward(char *des, int size, int step)
{
    if(step > size)
        return;
    int len = size - step;
    char *ptr = des;
    int i = 0;
    for(; i < len; i++) {
        *ptr = *(ptr + step);
        ++ptr;
    }
    memset(des + size - step, 0, step);
}


/** 
 * @brief  get a line of buf when the key is '\n'
 * @note   make the key char to '\0' and return the length of matched buf
 * @param  *input: the input buffer
 * @param  key: key char
 * @retval length of char matched
 */
int getlines(char *input, char key)
{
    char *p = NULL;
    printf("getliens\n*******\n");
    printf("%s %.02X\n", input, key);
    printf("********\n");
    p = strchr(input, key);
    if(p == NULL) {
        return 0;
    }
    int len = p - input + 1;
    *p = 0;
    return len;
}

// #define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = getlines(buf, '\n')) != 0; \
                                                    mvforward(buf, length, tmp), length -= tmp)


int str_to_zero(char *input, char *key)
{
    char *p = strstr(input, key);
    if(p == NULL) {
        return 0;
    }
    memset(p, 0, strlen(key));
    return p - input + strlen(key);
}

#define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = str_to_zero(buf, "\r\n")) != 0; \
                                                    mvforward(buf, length, tmp), length -= tmp)

#if 0

int main()
{
    // char *ptr = "tmp/song.mp3";
    // printf("ptr now is %s\n", ptr);
    // get_music_name(ptr, ptr, strlen(ptr));
    // printf("ptr now is %s\n", ptr);
    // struct _st_demo demo;
    // printf("body is %s %d\n", GETBODY(demo.a), pplus(1));

    char *str = "asdfsdf\r\ndslsdkdkdk\nieiwiweri";
    // char *str = "\r\n\0";
    // char *str = "sdflsdjflsdkjflksdjfklsd\n\0";
    char *s = malloc(strlen(str) + 1);
    strcpy(s, str);
    int len = strlen(s);
    int l;
    int i = 0;
    FOREACH_LINE_OF_BUF(s, len, l){
    // for(l = getlines(s, '\n'); l != 0; mvforward(s, len, l),l = getlines(s, '\n')){
    // for(; (l = getlines(s, '\n')) != 0; mvforward(s, len, l), i++){
        printf("\n------------\n %d lines: %d->%s\n-------------\n", i++, len, s);
        
    }
    printf("len is %d\n", len);
    return 0;
}

#else

struct _st_arg
{
    char *file;
    char *func;
    char *format;
    void (*callback)(char *, va_list);
    va_list arg;
};

void _cb_(char *format, va_list arg) 
{
    printf("callback !! %s\n", format);
    char *ptr = format;
    while(*ptr && *ptr != '>') ++ptr;
    ++ptr;
    printf("form %s\n", ptr);
    // va_list arg, arg1;
    // va_start(arg, format);
    #if 1
    while(*ptr) {
        switch(*ptr++) {
            case 'd' : {
                printf("get double %f\n", *va_arg(arg, double*));
                break;
            }
            case 'i' : {
                printf("get int %d\n", *va_arg(arg, int*));
                break;
            }
            case 's' : {
                printf("get str %s\n", va_arg(arg, char*));
                break;
            }
            default : {
                printf("invalid option!\n");
                break;
            }
        }
    }
    #endif
}

void fun(void (*callback)(char*, va_list), char *fmt, ...)
{
    va_list arg, arg1;
    va_start(arg, fmt);
    char *format = fmt;
    while(*format) {
        switch(*format++) {
            case 'd' : {
                printf("get double %f\n", va_arg(arg, double));
                break;
            }
            case 'i' : {
                printf("get int %d\n", va_arg(arg, int));
                break;
            }
            case 's' : {
                printf("get str %s\n", va_arg(arg, char*));
                break;
            }
            case '>' : {
                goto Endwhile;
            }
            default : {
                printf("invalid option!\n");
                break;
            }
        }
    } Endwhile:
    NULL;
    va_copy(arg1, arg);
    char *s = format;
    while(*format) {
        switch(*format++) {
            case 'd' : {
                *va_arg(arg, double *) = 1;
                break;
            }
            case 'i' : {
                *va_arg(arg, int *) = 10;
                break;
            }
            case 's' : {
                strcpy(va_arg(arg, char*), "hello");
                break;
            }

            default : {
                printf("invalid option!\n");
                break;
            }
        }
    }
    if(callback)
        callback(fmt, arg1);

    va_end(arg);
    va_end(arg1);
}

static void *proc(void *a)
{
    struct _st_arg *arg = (struct _st_arg*)a;
    char *format = arg->format;
    // while(1)
    printf("proc !!!\nfmt %s\n", format);
    while(*format) {
        switch(*format++) {
            case 'd' : {
                printf("get double %f\n", va_arg(arg->arg, double));
                break;
            }
            case 'i' : {
                printf("get int %d\n", va_arg(arg->arg, int));
                break;
            }
            case 's' : {
                printf("get str %s\n", va_arg(arg->arg, char*));
                break;
            }
            case '>' : {
                goto Endwhile;
            }
            default : {
                printf("invalid option!\n");
                break;
            }
        }
    } Endwhile:
    NULL;
    va_list arg1;
    va_copy(arg1, arg->arg);
    char *s = format;
    while(*format) {
        switch(*format++) {
            case 'd' : {
                *va_arg(arg->arg, double *) = 1;
                break;
            }
            case 'i' : {
                *va_arg(arg->arg, int *) = 10;
                break;
            }
            case 's' : {
                strcpy(va_arg(arg->arg, char*), "hellsdfsaddfdsaaaaaaaaaaaaaaaaaassssssssssssssssssssssssssssssssssssssssssso");
                break;
            }

            default : {
                printf("invalid option!\n");
                break;
            }
        }
    }
    if(arg->callback)
        arg->callback(arg->format, arg1);

    va_end(arg->arg);
    va_end(arg1);
    if(arg) {
        if(arg->format) {
            free(arg->format);
            arg->format = NULL;
        }
        free(arg);
        arg = NULL;
    }

    return NULL;
}

int func_async(void (*cb)(char *,va_list), char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    struct _st_arg *a = calloc(1, sizeof(struct _st_arg));
    a->format = format;
    a->format = calloc(1, strlen(format) + 1);
    strcpy(a->format, format);
    va_copy(a->arg, arg);
    a->callback = cb;

    pthread_t pid;
    printf("phtread create\n");
    pthread_create(&pid, NULL, proc, (void *)a);
    pthread_detach(pid);
    return 0;
}

static int ret;
char *sss ;
static int __fff___()
{
    sss = calloc(1, 100);
    // fun(_cb_, "ii>si", 123, 234, sss, &ret);
    func_async(_cb_, "ii>si", 456,678, sss, &ret);
    // free(sss);
    // sss = NULL;
}

int main(int argc, char *argv[])
{
    // int ret;
    // char *sss = calloc(1, 100);
    // // fun(_cb_, "ii>si", 123, 234, sss, &ret);
    // func_async(_cb_, "ii>si", 456,678, sss, &ret);
    // free(sss);
    // sss = NULL;
    __fff___();
    while(1) {
        // printf("%s\n",sss);
        sleep(1);
    }
    // _cb_("ii>si", sss, &ret);
    return 0;
}


#endif
