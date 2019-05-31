#include "halo_utf8.h"

static char _num_to_hex(uint8_t num)
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

int utf8_cut_string(char *str, int len)
{
    if(strlen(str) < len) {
        return 0;
    }
    char *ptr = str;
    unsigned char tmp = 0;
    int num = 0;
    while(*ptr != '\0') {
        if(*ptr & 0x80) {
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80) {
                ++num;
                tmp <<= 1;
            }
            ptr += num;
            if(ptr-str >= len) {
                *(ptr-num) = 0;
                break;
            }
        } else {
            ++ptr;
            if(ptr - str >= len) {
                *ptr = 0;
            }
        }
    }
    return 0;
}

size_t utf8_strlen(char *str)
{
    char *ptr = str;
    unsigned char tmp = 0;
    size_t len = 0;
    int u8_len = 0;
    int num = 0;
    while(*ptr != '\0') {
        if(*ptr & 0x80) {
            ++u8_len;
            
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80) {
                ++num;
                tmp <<= 1;
            }
            print_utf8("num is %d", num);
            ptr += num;
        } else {
            ++ptr;
        }
        ++len;
    }
    // print_utf8("strlen is %d, u8 is %d, final is %d", strlen(str), u8_len, len);
    return len;
}

int add_x_www_couple(char *des, char *key, char *body, int len)
{
    #define JUDGE(d, p, l)    (((p) - (d)) >= (l))
    if(key[0] == 0 || body[0] == 0 || len <= 0){
        print_utf8("body set illegal");
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
    print_utf8("body set error! reset the body");
    memset(ori, 0, len - (ori - des));
    return -1;
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