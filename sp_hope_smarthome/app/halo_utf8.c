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


static char *pattern[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十", "百", ""};

static int _get_num_(char *input, size_t *len)
{
    char **p = pattern;
    int i = 0;
    for(; **p; ++i) {
        if(memcmp(input, *p, strlen(*p)) == 0) {
            goto Find;
        }
        p += 1;
    }
    return -1;
    Find:
    *len = strlen(*p);
    return i;
}

static char *_cut_(char *input, int forcebegin)
{
    #if 1
    char **p = pattern;
    char *out = NULL;
    char *ptr = NULL;

    #endif
    if(forcebegin) {
        ptr = out = input;
    } else {
        while(**p != '\0' && *p != NULL) {
            char *tmp = strstr(input, *p);
            p += 1;
            if(tmp != NULL) {
                if(out == NULL) {
                    out = tmp;
                } else {
                    out = out < tmp ? out : tmp;
                }
            }
        }

        ptr = out;

    }
    if(ptr == NULL) {
        return NULL;
    }
    while(*ptr) {
        size_t len;
        if(_get_num_(ptr, &len) < 0) {
            break;
        }
        ptr += len;
    }
    if(ptr == out) {
        return NULL;
    }
    char *r_str = calloc(1, ptr - out + 1);
    if(r_str != NULL)
        memcpy(r_str, out, ptr-out);
    return r_str;
}

static char *_revert_(char *input)
{
    char *ptr = calloc(strlen(input) + 1, 1);
    char *r_ptr = ptr + strlen(input);

    
    char *tmp = input;
    while(*tmp != '\0') {
        char **p = pattern;
        while(**p != '\0' && *p != NULL) {
            if(memcmp(tmp, *p, strlen(*p)) == 0) {
                break;
            }
            p += 1;
        }
        r_ptr -= strlen(*p);
        memcpy(r_ptr, *p, strlen(*p));
        tmp += strlen(*p);
    }
    return ptr;
}

char *str_befor_ch_num(char *input)
{
    if(input == NULL || *input == '\0') return NULL;
    char *num_str = _cut_(input, 0);
    print_utf8("num get %s", num_str);
    char *ret_str = NULL;
    if(num_str != NULL) {
        char *ptr = strstr(input, num_str);
        size_t len = ptr - input;
        ret_str = calloc(1, len + 1);
        strncpy(ret_str, input, len);
    } else {
        ret_str = calloc(1, strlen(input) + 1);
        strcpy(ret_str, input);
    }
    free(num_str);
    return ret_str;
}

int chinese_num(char *input, int forcebegin)
{
    if(input == NULL || *input == '\0') {
        // printf("input illegal\n");
        return -1;
    }
    int ret = 0;
    char *ptr = _cut_(input, forcebegin);

    if(ptr == NULL) {
        return -1;
    }
    if(strstr(ptr, "十") || strstr(ptr, "百")) {
        char *rev = _revert_(ptr);
        char *_ptr = rev;
        int b = 0;
        while(*_ptr) {
            size_t len;
            int i = _get_num_(_ptr, &len);
            _ptr += len;
            switch(b) {
                case 0: {
                    if(i < 10) {
                        ret += i;
                    } else {
                        b = i - 9;
                    } 
                    break;
                }
                case 1: {
                    if(i < 10) {
                        ret += i * 10;
                        b = 0;
                    } else {
                        ret += 10;
                        b = i - 9;
                    }
                    break;
                }
                case 2: {
                    if(i < 10) {
                        ret += i * 100;
                        b = 0;
                    } else {
                        ret += 100;
                        b = i - 9;
                    }
                    break;
                }
                default:break;
            }
        }
        if(b == 1) {
            ret += 10;
        } else if( b == 2) {
            ret += 100;
        }
        free(rev);
    } else {
        char *_ptr = ptr;
        while (*_ptr) {
            size_t len;
            int i = _get_num_(_ptr, &len);
            ret *= 10;
            ret += i;
            _ptr += len;
        }
    }
    free(ptr);
    return ret;
}

