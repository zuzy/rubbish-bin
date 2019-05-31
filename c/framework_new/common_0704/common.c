#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include<fcntl.h>
#include<termios.h>
// #include "net/config.h"
// #include "net/wifi_manager.h"
#include "list.h"
#include "common.h"

#include "cJSON.h"

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
    print_common("add tail %s", proto->name);
    list_add_tail(&proto->list, h);
    return 0;
}

int delete_proto(st_proto *proto)
{
    if(proto == NULL) {
        print_common("proto is null, need not free");
        return 0;
    }
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
        if(ptr->fd == fd) {
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
    }
    print_common("%d not exist", fd);
    return 1;
}

int add_msg(struct list_head *head, char *target, msg_apps_t msg)
{
    st_msg *m = (st_msg*)malloc(sizeof(st_msg));
    if(m == NULL) {
        print_common("malloc error!");
        return -1;
    }
    memset(m, 0, sizeof(st_msg));
    strncpy(m->target, target, MAX_PNAME_LEN - 1);
    m->type = msg.type;
    memcpy(m->content, msg.content, MAX_CONTENT_LEN);
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
        return delete_proto(proto);
    }
    if(proto->fd >= 0 && proto->callbacks->close_cb != NULL) {
        if(proto->callbacks->close_cb(proto) == 0) {
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

int64_t get_now_us(void)
{
    struct timeval tv;

    //clock_gettime(CLOCK_REALTIME, &tv);
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000ll + tv.tv_usec;
}

int64_t get_now_s(void)
{
    return (get_now_us() >> 20);
}

int reset_timer(st_proto *pro)
{
    if(pro != NULL){
        pro->timecount = get_now_s();
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
    int len = size - step;
    char *ptr = des;
    int i = 0;
    for(; i < len; i++) {
        *ptr = *(ptr + step);
        ++ptr;
    }
}

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

int change_flag(char *des, int size)
{
    char *head = des;
    char *end = des + strlen(des);
    int flag_num = 0;
    char val = 0;
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
    if(flag_num) {
        mvforward(head , size, flag_num);
    }
    return flag_num;
}

