#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int luaopen_mt(lua_State *L);

static char *pattern[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十", "百", ""};

// void dump() 
// {
//     char **p = pattern;
//     while(**p) {
//         printf("**%s\n", *p);
//         p += 1;
//     }
// }

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

static char *_cut_(char *input)
{
    char **p = pattern;
    char *out = NULL;
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

    char *ptr = out;
    while(*ptr) {
        size_t len;
        if(_get_num_(ptr, &len) < 0) {
            break;
        }
        ptr += len;
    }
    char *r_str = calloc(ptr - out + 1, 1);
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

int chinese_num(char *input)
{
    if(input == NULL || *input == '\0') {
        printf("input illegal\n");
        return -1;
    }
    int ret = 0;
    char *ptr = _cut_(input);

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

static int ch_to_num(lua_State *L)
{
    char *str = luaL_checkstring(L, 1);
    lua_pushnumber(L, chinese_num(str));
    return 1;
}

static const struct luaL_Reg mt_lib [] = {
        {"ch_to_num", ch_to_num}, 
        {NULL, NULL}
};

int luaopen_ch2num(lua_State *L)
{
    // lua_register(L, "ch_to_num", ch_to_num);
    luaL_register(L, "ch2num", mt_lib);
    printf("regtist!\n");
    return 1;
}


#if 0
char *test[] = {
    "灯一百sdfdd",
    "百十八",
    "百零八",
    "百二十八",
    "百",
    "八百二十",
    "七百八十九",
    "六十八",
    "六十",
    "十",
    "十八",
    "零",
    "一百零八",
    "一二三",
    "一二十",
    "六百一二十九",
    "一二三四",
    ""
};

int main(int argc, char *argv[])
{
    // dump();
    // return 0;
    char **p = test;
    while(**p) {
        printf("test %s\t", *p);
        int num = chinese_num(*p);
        printf("get num %d\n", num);
        p += 1;
    }
}
#endif