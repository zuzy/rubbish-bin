#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdarg.h>
#include "net/config.h"
#include "net/wifi_manager.h"
#include <apconfig.h>
#include <ctype.h>
#include "list.h"
#include "common.h"

#include "cJSON.h"

#include <applib.h>
#include "case_msg.h"

#include "lua_common.h"

static st_halo *s_halo_handle = NULL;

int add_proto(st_halo *halo, st_proto *proto)
{
    st_proto *ptr;
    struct list_head *h = halo->protos;
    if(halo == NULL){
        print_common("handle need to init");
        return -1;
    }
    if(proto->fd > 0) {
        proto->active = 1;
    } else {
        proto->active = 0;
    }
    proto->msgs = halo->msgs;
    // list_for_each_entry(ptr, halo->protos, list) {
    //     if(strncmp(ptr->name, proto->name, MAX_PNAME_LEN) == 0) {
    //         print_common("proto[%s] is exist!", proto->name);
    //         return -1;
    //     }
    // }
    print_common("add tail %s %d %llu %llu", proto->name, proto->config, proto->timecount, proto->timeout);
    list_add_tail(&proto->list, h);
    return 0;
}

int delete_proto(st_proto *proto)
{
    if(proto == NULL) {
        print_common("proto is null, need not free");
        return 0;
    }
    print_log("delete %s protocol", proto->name);
    list_del(&proto->list);
    if(proto->callbacks->close_cb != NULL)
        proto->callbacks->close_cb(proto);
    free(proto);
    return 0;
}

int close_proto_fd(int fd, struct list_head *head)
{
    st_proto *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, head, list) {
        if(ptr->fd != fd) {
            continue;
        }
        print_common("[%s:%d] found & close!", ptr->name, ptr->fd);
        if(ptr->config & LOOP_CFG_RETRY) {
            if(ptr->fd >= 0 && ptr->active){
                if(ptr->callbacks->close_cb(ptr) == 0) {
                    ptr->active = 0;
                    print_common("close %s success", ptr->name);
                    return 0;
                }
                print_common("close %s failed", ptr->name);
                return -1;
            } else {
                print_common("%s %d %d", ptr->name, ptr->fd, ptr->active);
            }
        } else {
            return delete_proto(ptr);
        }
    }
    print_common("%d not exist", fd);
    return 1;
}

int add_msg(struct list_head *head, char *target, st_pro_msg msg)
{
    st_msg *m = (st_msg*)malloc(sizeof(st_msg));
    if(m == NULL) {
        print_common("malloc error!");
        return -1;
    }
    memset(m, 0, sizeof(st_msg));
    strncpy(m->target, target, MAX_PNAME_LEN - 1);
    m->type = msg.type;
    memcpy(m->content, msg.content, msg.length);
    m->length = msg.length;
    list_add_tail(&m->list, head);
    return 0;
}

int del_msg(st_msg *msg)
{
    if(msg == NULL) {
        return -1;
    }
    list_del(&msg->list);
    free(msg);
    msg = NULL;
    return 0;
}

int del_all_msg(struct list_head *msgs)
{
    st_msg *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, msgs, list) {
        del_msg(ptr);
    }
    return 0;
}

int clean_msg(st_halo *halo)
{
    st_msg *p_msg, *t_msg;
    st_proto *p_pro, *t_pro;
    int del = 0;
    list_for_each_entry_safe(p_msg, t_msg, halo->msgs, list) {
        print_common("target %s", p_msg->target);
        del = 1;
        list_for_each_entry_safe(p_pro, t_pro, halo->protos, list) {
            print_common("pro scan");
            if(strncmp(p_msg->target, p_pro->name, MAX_PNAME_LEN) == 0) {
                del = 0;
                break;
            }
        }
        if(del) {
            print_common("target to %s is illegal!", p_msg->target);
            del_msg(p_msg);
        }
    }
    return 0;
}

int close_proto(st_proto *proto)
{
    if((proto->config & LOOP_CFG_RETRY) == 0) {
        print_common("delete!");
        return delete_proto(proto);
    }
    if(proto->fd >= 0 && proto->callbacks->close_cb != NULL) {
        if(proto->callbacks->close_cb(proto) == 0) {
            print_log("close %s and wait for retry", proto->name);
            proto->active = 0;
        } else {
            print_common("close %s failed!", proto->name);
            return -1;
        }
    }
    return 0;
}

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        print_common("fcntl(F_GETFL)");
    }
    if ((flags & O_NONBLOCK) == O_NONBLOCK) {
        return;
    }
    flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (flags == -1) {
        print_common("fcntl(F_SETFL)");
    }
}

uint64_t get_now_us(void)
{
    struct timeval tv;

    //clock_gettime(CLOCK_REALTIME, &tv);
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ll + tv.tv_usec;
}

uint64_t get_now_s(void)
{
    return (get_now_us() >> 20);
}

uint64_t get_now_ms(void)
{
    return (get_now_us() >> 10);
}

int reset_timer(st_proto *pro)
{
    if(pro != NULL){
        pro->timecount = get_now_ms();
    }
    return 0;
}

char *getip(void)
{
    char *ip = malloc(strlen("255.255.255.255") + 1);
    strcpy(ip, "192.168.31.183");
    return ip;
}

void clear_payload(st_proto *pro)
{
    if(pro != NULL) {
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
        pro->recv_buf.begin = 0;
    }
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

int key_to_zero(char *input, char key)
{
    char *p = NULL;

    if(strlen(input) <= 0) {
        // print_common("input is null");
        return 0;
    }
    // print_common("in %s", input);
    // print_common("intput: %s %.02X", input, key);
    p = strchr(input, key);
    if(p == NULL) {
        return 0;
    }
    int len = p - input + 1;
    *p = 0;
    // print_common("[end]");
    return len;
}

int str_to_zero(char *input, char *key)
{
    if(input == NULL || *input == '\0') {
        return 0;
    }
    char *p = strstr(input, key);
    if(p == NULL) {
        return 0;
    }
    memset(p, 0, strlen(key));
    return p - input + strlen(key);
}

#if 0
#define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = key_to_zero(buf, '\n')) != 0; \
                                                    mvforward(buf, length, tmp), length -= tmp)
int main()
{
    // char *ptr = "tmp/song.mp3";
    // printf("ptr now is %s\n", ptr);
    // get_music_name(ptr, ptr, strlen(ptr));
    // printf("ptr now is %s\n", ptr);
    // struct _st_demo demo;
    // printf("body is %s %d\n", GETBODY(demo.a), pplus(1));

    char *str = "asdfsdf\r\ndslsdkdkdk\r\nieiwiweri\n";
    char *s = malloc(strlen(str) + 1);
    strcpy(s, str);
    int len = strlen(s);
    int l;
    FOREACH_LINE_OF_BUF(s, len, l){
    // for(l = key_to_zero(s, '\n'); l != 0; mvforward(s, len, l),l = getlines(s, '\n')){
        printf("\n------------\nlines: %s\n-------------\n", s);
    }
    return 0;
}
#endif


void del_null(char *des, int len)
{
    int num = 0;
    char *ptr = des;
    int size = len;
    while(*ptr++ < 32 && len-- > 0) {
        ++num;
    }
    if(len && num) {
        mvforward(des, size, num);
    }
}

#if 1
int change_flag(char *des, int size)
{
    if(des == NULL || size <= 0) {
        return -1;
    }
    char *ptr = des;
    char *p_mv = des;
    int escape = 0;
    int i = 0;
    for(i = 0; i < size; ++i) {
        if(escape) {
            if(*p_mv == 0x01 || *p_mv == 0x02) {
                *ptr++ = *p_mv++ + 0x7c;
                escape = 0;
                continue;
            }
            printf("escape error %d %.02X !\n", i, *p_mv);
            return -1;
        }
        if(*p_mv == 0x7d) {
            escape = 1;
            ++p_mv;
            continue;
        } else {
            *ptr++ = *p_mv++;
        }
    }
    i = ptr - des;
    memset(ptr, 0, size - i);
    return size - i;
}
#else
int change_flag(char *des, int size)
{
    char *head = des;
    char *end = des + strlen(des);
    int flag_num = 0;
    char val = 0;
    // print_common();
    while((end - flag_num) >= head) {
        val = *(end - flag_num);
        if((val != 0x01 && val != 0x02) || *(end - flag_num - 1) != 0x7d) {
            *end = *(end - flag_num);
            --end;
        } else {
            *end-- = val + 0x7c;
            ++flag_num;
        }
    }
    // print_common();
    if(flag_num) {
        mvforward(head , size, flag_num);
    }
    return flag_num;
}
#endif
void lua_stachDump(lua_State *L)
{
    int i;
    int top = lua_gettop(L);
    printf("top : %d\n", top);
    for(i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch(t) {
            case LUA_TSTRING: {
                printf("%s", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                printf("%g", lua_tonumber(L, i));
                break;
            }
            default: {
                printf("%s", lua_typename(L, t));
                break;
            }
        }
        printf("\n");
    }
    printf("[end]\n");
}

int get_tab(lua_State *L)
{
    printf("set tab !!!\n");

    // double d = luaL_checknumber(L, 1);
    // lua_pushnumber(L, d*2); 

    lua_newtable(L);
    int i;
    char value[100] = "";
    for(i = 0; i < 5; ++i) {
        sprintf(value, "value %d", i+1);
        lua_pushnumber(L, i+1);
        lua_pushstring(L, value);
        lua_settable(L, -3);
    }
    // lua_setglobal(L, "new_tab");
    return 1;
}

int lua_broad_cast(lua_State *L)
{
    printf("lua broad cast\n");
    int type = luaL_checkint(L, 1);
    char *str = luaL_checkstring(L, 2);
    msg_apps_t msg;
    msg.type = type + MSG_LUA_START;
    strncpy(msg.content, str, sizeof(msg.content));
    int ret = broadcast_msg(&msg);
    lua_pushinteger(L, ret);
    return 1;
}

void lua_call_fun(const char *filename,
                    const char *func,
                    const char *format,
                    ...)
{
    uint64_t _ms = get_now_ms();
    uint64_t _us = get_now_us();
    lua_State *L = lua_open();
    print_common("call lua fun %s", func);
    char tmp[128] = "";
    snprintf(tmp, 127, "echo 'call lua fun %s' >> /tmp/halo_mem.log", func);
    // system("date >> /tmp/halo_mem.log");
    system(tmp);
    system("procrank | grep 'cmdline\\|halo' >> /tmp/halo_mem.log");
    luaL_openlibs(L);
    // lua_pushcfunction(L, get_tab);
    // lua_setglobal(L, "gettab");
    // lua_register(L, "gettab", get_tab);
    // lua_register(L, "broadcast", lua_broad_cast);
    // lua_stachDump(L);
    #if 0
    lua_init_local_api(L);
    #else
    // print_common();
    luaopen_hpio(L);
    // print_common("\n\n\n open ok \n\n\n");
    // lua_stachDump(L);
    #endif
    // get_tab(L); // error!!!!
    if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
        printf("lua load %s file error\n", lua_tostring(L, -1));
        goto Exit;
    }
        va_list v1;
    int narg, nres;
    lua_getglobal(L, func);
    if(format == NULL || strlen(format) <= 0) {
        lua_pcall(L, 0, 0, 0);
        print_common("lua call without args");
        return;
    }
    va_start(v1, format);
    narg = 0;
    
    while(*format) {
        switch(*format++) {
            case 'd' : {
                lua_pushnumber(L, va_arg(v1, double));
                break;
            }
            case 'i' : {
                lua_pushnumber(L, va_arg(v1, int));
                break;
            }
            case 's' : {
                char *str = va_arg(v1, char *);
                lua_pushstring(L, str);
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
        ++narg;
        luaL_checkstack(L, 1, "toomany arguments");
    } Endwhile:
    nres = strlen(format);
    if(lua_pcall(L, narg, nres, 0)) {
        printf("call function error!\n");
        goto Exit;
    }
    nres = 0;
    // lua_stachDump(L);
    while(*format) {
        ++nres;
        switch(*format++) {
            case 'd': {
                if(!lua_isnumber(L, nres)){
                    printf("wrong type d!\n");
                    goto Exit;
                }
                int n = lua_tonumber(L, nres);
                // print_common("%d-> %d", nres, n);
                *va_arg(v1, double *) = n;
                break;
            }
            case 'i': {
                if(!lua_isnumber(L, nres)) {
                    printf("wrong type i!\n");
                    goto Exit;
                }
                int n = lua_tonumber(L, nres);
                // print_common("%d-> %d", nres, n);
                *va_arg(v1, int *) = n;
                // *va_arg(v1, int *) = lua_tonumber(L, nres);
                break;
            }
            case 's': {
                if(!lua_isstring(L, nres)) {
                    printf("wrong type s!\n");
                    goto Exit;
                }
                char *str = lua_tostring(L, nres);
                // print_common("%d -> %s", nres, str);
                strncpy(va_arg(v1, char **), str, MAX_BODY_LEN);
                break;
            }
            default: {
                printf("invalid option (%c)\n", *(format-1));
                goto Exit;
            }
        }
    }

    va_end(v1);
    lua_pop(L,nres);
Exit:
    lua_close(L);
    print_err("\nlua call last %lld ms %lld ms\n\n", get_now_ms() - _ms, get_now_us() - _us);
    system("procrank | grep 'cmdline\\|halo' >> /tmp/halo_mem.log");

    // print_err("\nlua call last %lld us\n\n", get_now_us() - _us);
}

#if 0
// lua call function
void lua_call_func(const char *filename, const char *func,
				   const char *param,int len,char *ret)
{
	lua_State *L = lua_open();
	luaL_openlibs(L);
	if(luaL_loadfile(L,filename)|| lua_pcall(L,0,0,0)) {
		printf("luaL_loadfile err:%s\n",lua_tostring(L,-1));
		goto EXIT;
		//lua_error(L);
	}
	
	lua_getglobal(L,func);
	lua_pushlstring(L,param,len);
	
	if(lua_pcall(L,1,1,0)){
		printf("lua_pcall func '%s' err:%s\n",func,lua_tostring(L,-1));
		goto EXIT;
	}
	
	if(!lua_isstring(L,-1)) {
		printf("lua return type error:%s\n",lua_tostring(L,-1));
		goto EXIT;
	}
	
	strncpy(ret,lua_tostring(L,-1),127);
	lua_pop(L,1);
// 	printf("lua_call_func-ret[%ld]:%s\n",strlen(ret),ret);
EXIT:
 	lua_close(L);
}
#endif

void lua_call_device()
{
    int ret = 0;
    print_common("lua call for dev list");
    lua_call_fun("/tmp/lua/http_regist.lua", "hope_http_get_device", ">i", &ret);
    print_common("ret %d", ret);
}

void lua_regitst(const char *uri, const char *body, char* ret, int *code)
{
    #if  1
    lua_call_fun("/tmp/lua/http_regist.lua", "hope_http_regist", "ss>si", uri, body, ret, code);
    print_common("regist code is %d", *code);
    print_common("regist ret is %s", ret);
    #else
    lua_State *L = lua_open();
    luaL_openlibs(L);
    if(luaL_loadfile(L, "/tmp/lua/http_regist.lua") || lua_pcall(L, 0, 0, 0)) {
		printf("luaL_loadfile err:%s\n",lua_tostring(L,-1));
		goto EXIT;
		//lua_error(L);
	}
	
	lua_getglobal(L,"hope_http_regist");
	lua_pushstring(L,uri);
	lua_pushstring(L, body);
	if(lua_pcall(L,2,2,0)){
		printf("lua_pcall func '%s' err:%s\n","hope_http_regist",lua_tostring(L,-1));
		goto EXIT;
	}
	
    if(!lua_isnumber(L,-1)) {
		printf("lua return type error:%s\n",lua_tostring(L,-1));
		goto EXIT;
	}
    *code = lua_tonumber(L, -1);
	print_common("regist code is %d", *code);
	// strncpy(ret,lua_tostring(L,-1),127);
	// lua_pop(L,1);

	if(!lua_isstring(L,-2)) {
		printf("lua return type error:%s\n",lua_tostring(L,-2));
		goto EXIT;
	}
	
	strncpy(ret,lua_tostring(L,-2),127);
	lua_pop(L,2);
// 	printf("lua_call_func-ret[%ld]:%s\n",strlen(ret),ret);
EXIT:
 	lua_close(L);
     #endif
}

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

int play_list_msg()
{
    msg_apps_t msg;
    msg.type = MSG_HOPE_PLAY_LIST;
    memset(msg.content, 0, sizeof(msg.content));
    return broadcast_msg(&msg);
}

int sep_file_suffix(char *uri, char *name, int max_len, char *suffix)
{
    if(uri == NULL || strlen(uri) <= 0 || max_len <= 3) {
        return -1;
    }
    char *ptr = strrchr(uri, '.');
    if(ptr == NULL) {
        return -1;
    }
    strncpy(suffix, ptr, MAX_SUFFIX_LEN);
    if(strchr(uri, '/') != NULL) {
        get_music_name(uri, name, max_len);
    } else {
        *ptr = '\0';
        strncpy(name, uri, max_len);
    }
    return 0;
}

int sep_to_index_suffix(char *uri, char *suffix)
{
    char name[20] = "";
    
    REQUIRE(sep_file_suffix(uri, name, 20, suffix) < 0, Error);
    char *p_s = suffix;
    int tmp = 0;
    while(*p_s != '\0') {
        tmp = tolower(*p_s);
        *p_s++ = tmp;
        // *p_s++ = tolower(*p_s);
    }
    int num = 0;
    char *p = name;
    if(*p <= '9' && *p >= '0') {
        while(*p <= '9' && *p >= '0') {
            num *= 10;
            num += *p++ - '0';
        }
        return num;
    }
    Error:{
        return -1;
    }
}

int format_ign_list()
{
    char play_path[50] = "";
    char cmd[200] = "";
    int m_disk = HOTPLUGIN_DEV_TYPE_NONE;
    get_config(CFG_MAIN_DISK, &m_disk, sizeof(int));
    switch(m_disk) {
        case HOTPLUGIN_DEV_TYPE_NONE :
        case HOTPLUGIN_DEV_TYPE_LOCAL :{
            strcpy(play_path, "/mnt/udisk");
            break;
        }
        case HOTPLUGIN_DEV_TYPE_CARD: {
            strcpy(play_path, "/mnt/sdcard");
            break;
        }
        case HOTPLUGIN_DEV_TYPE_UHOST: {
            strcpy(play_path, "/mnt/external");
            break;
        }
        default: break;
    }
    if(*play_path != '\0') {
        strcat(play_path, "\\ign");
        print_common("ign path is %s", play_path);
    }
    snprintf(cmd, 128, "ls %s | awk '{print %s/$0}' > /tmp/ign_list", play_path, play_path);
    system(cmd);
    return 0;
    // ls -R |awk '{print i$0}' i=`pwd`'/'
}
