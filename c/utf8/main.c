//串口相关的头文件    
#include<stdio.h>      /*标准输入输出定义*/    
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    
#include<errno.h>      /*错误号定义*/    
#include<string.h>    

#include <stdint.h>


char str[] = "{\"albumName\":\"狂想曲(夏日狂欢庆功限量影音版)\",\"authorName\":\"萧敬腾\",\"displayName\":\"06.怎么说我不爱你\",\"musicName\":\"06.怎么说我不爱你\",\"musicId\":20,\"musicTime\":268}]} er";


static size_t _str_u8_len(char *str){
    char *ptr = str;
    unsigned char tmp = 0;
    size_t len = 0;
    int u8_len = 0;
    int num = 0;
    while(*ptr != '\0'){
        if(*ptr & 0x80){
            ++u8_len;
            #if 0
            num = 0;
            while(*ptr & 0x80){
                ++ptr;
                ++num;
            }
            printf("num is %d\n", num);
            #else
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80){
                ++num;
                tmp <<= 1;
            }
            // printf("num is %d\n", num);
            ptr += num;
            #endif
        }else{
            ++ptr;
        }
        ++len;
    }
    printf("strlen is %ld, u8 is %d, final is %ld\n", strlen(str), u8_len, len);
    return len;
}


static int _cut_string(char *str, int len){
    if(strlen(str) < len){
        return 0;
    }
    // printf("%d:%s\n", strlen(str), str);
    char *ptr = str;
    unsigned char tmp = 0;

    int num = 0;
    while(*ptr != '\0'){
        if(*ptr & 0x80){
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80){
                ++num;
                tmp <<= 1;
            }
            ptr += num;
            if(ptr-str >= len){
                *(ptr-num) = 0;
                break;
            }
        }else{
            ++ptr;
            if(ptr - str >= len){
                *ptr = 0;
            }
        }
    }
    printf("%ld: %s\n", strlen(str), str);
    return 0;
}

static char *str_get_string(char *str, int utflen){
    if(strlen(str) < utflen){
        return strdup(str);
    }
    // printf("%d:%s\n", strlen(str), str);
    char *ptr = str;
    unsigned char tmp = 0;

    int num = 0;
    int ulength = 0;
    char t = 0;
    while(*ptr != '\0'){
        if(*ptr & 0x80){
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80){
                ++num;
                tmp <<= 1;
            }
            ptr += num;
        }else{
            ++ptr;
        }
        ++ulength;
        if(ulength >= utflen) {
            t = *ptr;
            *ptr = 0;
        }
    }
    char *ret = strdup(str);
    *ptr = t;
    printf("%ld: %s\n", strlen(ret), ret);
    return ret;
}

static char _num_to_hex(uint8_t num){
    char out = 0;
    num &= 0x0f;
    if(num >= 10){
        out = 'A' + num - 10;
    }else{
        out = '0' + num;
    }
    return out;
}

int add_x_www_couple(char *des, char *key, char *body, int len)
{
    #define JUDGE(d, p, l)    (((p) - (d)) >= (l))
    if(key[0] == 0 || body[0] == 0 || len <= 0){
        // print_utf8("body set illegal");
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
    #if 1
        switch (*p_k) {
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
            case '.':
            case '-':
            case '_': {
                *ptr++ = *p_k++;
                if(JUDGE(des, ptr, len)) {
                    goto Error;
                }
                break;
            }
            case ' ': {
                *ptr++ = '+';
                ++p_k;
                if(JUDGE(des, ptr, len)) {
                    goto Error;
                }
                break;
            }
            
            
            default: {
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
                break;
            }
        }
    #else
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
    #endif
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
    memset(ori, 0, len - (ori - des));
    printf("error\n");
    return -1;
}

static int _append_(void **tar, size_t *len, size_t unit)
{
    *len += unit;
    *tar = realloc(*tar, *len);
    return *tar == NULL;
}

char *encode(char *target)
{
    #ifndef JUDGE
    #define JUDGE(d, p, l)    (((p) - (d)) >= (l))
    #endif
    size_t len = 0;
    size_t unit = strlen(target);
    char *ret = NULL;
    size_t offset = 0;
    size_t i = 0;
    char *ptr = ret;
    for(; i < unit; ++i) {
        if(JUDGE(0, offset, len)) {
            if(_append_((void **)&ret, &len, unit)) {
                return ret;
            }
        }
        switch (target[i]) {
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
            case '.':
            case '-':
            case '_': {
                ret[offset++] = target[i];
                break;
            }
            case ' ': {
                ret[offset++] = '+';
                break;
            }
            default: {
                ret[offset++] = '%';
                if(JUDGE(0, offset, len)){
                    if(_append_((void **)&ret, &len, unit)) {
                        return ret;
                    }
                }
                ret[offset++] = _num_to_hex(target[i] >> 4);
                if(JUDGE(0, offset, len)) {
                    if(_append_((void **)&ret, &len, unit)) {
                        return ret;
                    }
                }
                ret[offset++] = _num_to_hex(target[i]);
                break;
            }
        }
    }
    return ret;
}

int main(){
    int i;
    printf("%ld: %s\n", strlen(str), str);
    printf("_str_u8_len(str) = %ld\n", _str_u8_len(str));
    for(i = 40; i < 50; i++){
        // printf("%d:\n",i);
        // char *s1 = malloc(strlen(str) + 1);
        // strcpy(s1, str);
        // _cut_string(s1, i);
        // printf("[end]\n");

        char *tmp  = str_get_string(str, i);
        printf("get str %d %s\n[end]\n", i, tmp);
        free(tmp);
    }

    char *bbb = (char *)malloc(1000);
    char *lll = malloc(0);
    printf("%p\n", lll);
    free(lll);
    printf("%p\n", lll);

    printf("%p\n", bbb);
    printf("%p\n", bbb);
    char *s = NULL;
    memset(bbb, 0, 1000);
    add_x_www_couple(bbb, "bb是的方式的方式地方", "中国", 1000);
    printf("bbb is %s\n", bbb);
    free(bbb);


    char *sss = encode("bb是的方式的方式地方");
    printf("bbb is %s\n", sss);
    free(sss);
    return 0;
}
