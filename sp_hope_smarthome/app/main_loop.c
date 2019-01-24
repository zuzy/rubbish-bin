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
#include "net/config.h"
#include "net/wifi_manager.h"
#include "list.h"
#include "common.h"
#include "main_loop.h"
#include "main_pipe.h"
#include "tcp_client.h"
#include "udp_ser.h"
#include "tcp_ser.h"
#include "halo_http_regist.h"
#include "smart_player_api.h"

#include "cloud_music_api.h"

st_halo *halo_handle = NULL;
struct list_head pro_head = LIST_HEAD_INIT(pro_head);
struct list_head msg_head = LIST_HEAD_INIT(msg_head);

static int _manager(void *arg);

int halo_init(callback hal_cb)
{
    if(halo_handle != NULL) {
        print_main("halo need to deinit");
        return -1;
    }
    halo_handle = (st_halo *)malloc(sizeof(st_halo));
    REQUIRE((halo_handle == NULL), Error);
    halo_handle->protos = &pro_head;
    halo_handle->msgs = &msg_head;
    halo_handle->hal_cb = hal_cb;
    halo_handle->manager_cb = _manager;
    halo_handle->env_status = 0;
    halo_handle->mlist = NULL;
    halo_handle->main_disk = -1;
    halo_handle->local_list = NULL;
    print_main("init ok");
    return 0;
Error:
    return -1;
}
#if 1
int wifi_connected = 0;
int alink_connect = 0;
#else 

#endif

static int _manag_unit(uint8_t env_status, 
                int status_change, 
                int64_t now, 
                st_halo *handle,
                st_msg *m_ptr)
{
    uint8_t config = 0;
    st_proto *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
        // print_main("++");
        config = ptr->config & 0x03;
        if(config <= env_status && ptr->active == 0) {
            if(ptr->callbacks->init_cb != NULL && ptr->callbacks->init_cb(ptr) == 0) {
                print_main("init %s success", ptr->name);
            }
        } 
        if(status_change){
            print_main("status change! %d %d", config, ptr->active);
            if(config > env_status && ptr->active == 1) {
                print_main("close !");
                close_proto(ptr);
            }
        }
        if(ptr->callbacks->disproto_cb != NULL && m_ptr != NULL) {
            // print_main("dispatch !!!");
            if(strncmp(m_ptr->target, ptr->name, MAX_PNAME_LEN) == 0) {
                ptr->callbacks->disproto_cb(ptr, m_ptr);
                // del_msg(m_ptr);
            }
        }
        if(ptr->callbacks->timeout_cb != NULL) {
            // print_main("[%s]time %ld", ptr->name, now);
            if(now - ptr->timecount >= ptr->timeout) {
                print_main("count %d, to %d", ptr->timecount, ptr->timeout);
                ptr->timecount = now;
                
                if(ptr->callbacks->timeout_cb(ptr)) {
                    print_main("timeout & error close");
                    close_proto(ptr);
                }
            }
        }
        print_main("%s %d %d", ptr->name, ptr->fd, ptr->active);
    }
    return 0;
}

static int _manager(void *arg)
{
    print_main("manager!------------------------");
    st_halo * handle = (st_halo *)arg;
    uint32_t env_status = 0;
    int status_change = 0;
    int64_t now = get_now_ms();
    int ret = 0;
    print_main("now %lld", now);
    #if 1
    wifi_status_e wifi_status;
    hotplug_get_wifi_status(&wifi_status);

    if(wifi_status == WIFI_STATE_CONNECTED) {
        env_status |= LOOP_CFG_WIFI;
    }

    #if 0
    if(sair_ai_dev_connected() == 1) {
        env_status |= LOOP_CFG_ALINK;
    }
    #else
    if(cloud_music_dev_connected() > 0) {
        env_status |= LOOP_CFG_ALINK;
    }
    #endif
    if(regist_status()) {
        env_status |= LOOP_CFG_HOPE;
    }
    #else
    if(wifi_connected) {
        env_status |= LOOP_CFG_WIFI;
    }
    if(alink_connect) {
        env_status |= LOOP_CFG_ALINK;
    }
    #endif
    if(env_status != handle->env_status) {
        handle->env_status = env_status;
        status_change = 1;
        print_main("status change!");
    }
    st_proto *ptr, *tmp;
    st_msg *m_ptr, *m_tmp;
    uint32_t config;
    print_main("loop env %ld", env_status);

#if 0
    if(list_empty(handle->msgs)) {
        _manag_unit(env_status, status_change, now, handle, NULL);
    } else {
        list_for_each_entry_safe(m_ptr, m_tmp, handle->msgs, list) {
            print_main("msg : %s", m_ptr->target);
            _manag_unit(env_status, status_change, now, handle, m_ptr);
            del_msg(m_ptr);
        }
        // del_all_msg(handle->msgs);
        clean_msg(handle);
    }
    // list_for_each_entry_safe(m_ptr, m_tmp, handle->msgs, list) {
    //     list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
    //         // print_main("++");
    //         config = ptr->config & 0x03;
    //         if(config <= env_status && ptr->active == 0) {
    //             if(ptr->callbacks->init_cb != NULL && ptr->callbacks->init_cb(ptr) == 0) {
    //                 print_main("init %s success", ptr->name);
    //             }
    //         } 
    //         if(status_change){
    //             print_main("status change! %d %d", config, ptr->active);
    //             if(config > env_status && ptr->active == 1) {
    //                 print_main("close !");
    //                 close_proto(ptr);
    //             }
    //         }
    //         if(ptr->callbacks->disproto_cb != NULL) {
    //             // print_main("dispatch !!!");
    //             if(strncmp(m_ptr->target, ptr->name, MAX_PNAME_LEN) == 0) {
    //                 ptr->callbacks->disproto_cb(ptr, m_ptr);
    //                 // del_msg(m_ptr);
    //             }
    //         }
    //         if(ptr->callbacks->timeout_cb != NULL) {
    //             print_main("[%s]time %ld", ptr->name, now);
    //             if(now - ptr->timecount >= ptr->timeout) {
    //                 print_main("count %d, to %d", ptr->timecount, ptr->timeout);
    //                 ptr->timecount = now;
                    
    //                 if(ptr->callbacks->timeout_cb(ptr)) {
    //                     print_main("timeout & error close");
    //                     close_proto(ptr);
    //                 }
    //             }
    //         }
    //         // print_main("%s %d %d", ptr->name, ptr->fd, ptr->active);
    //     }
    //     del_msg(m_ptr);
    // }
    // del_all_msg(handle->msgs);
#else
    // list_for_each_entry_safe(m_ptr, m_tmp, handle->msgs, list) {
    //     print_main("target %s", m_ptr->target);
    //     list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
    //         if(ptr->callbacks->disproto_cb != NULL) {
    //             if(strncmp(ptr->name, m_ptr->target, MAX_PNAME_LEN) == 0) {
    //                 ptr->callbacks->disproto_cb(ptr, m_ptr);
    //             }
    //         }
    //     }
    //     del_msg(m_ptr);
    // }
    // no need to clean!
    // clean_msg(handle);

    list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
        print_main("manage %s:%d %lld",ptr->name, ptr->fd, ptr->timeout);
        // config = ptr->config & (~LOOP_CFG_RETRY);
        config = ptr->config;
        config &= (~LOOP_CFG_RETRY);
        if((config == (config & env_status)) && (!ptr->active)) {
        // if(config <= env_status && ptr->active == 0) {
            #if 0
            if(ptr->callbacks->init_cb != NULL && ptr->callbacks->init_cb(ptr) == 0) {
                print_main("init %s success", ptr->name);
            }
            #else 
            if(ptr->callbacks->init_cb != NULL) {
                ret = ptr->callbacks->init_cb(ptr);
                if(ret == 9) {
                    print_main("init ok & delete the node!");
                    delete_proto(ptr);
                } else if(ret == 0) {
                    ptr->timecount = get_now_ms();
                    print_log("init %s success", ptr->name);
                }
            }
            #endif
        } 
        if(status_change){
            print_main("status change! %d %d", config, ptr->active);
            if((config != (config & env_status)) && ptr->active) {
                print_main("close !");
                close_proto(ptr);
            }
        }
        if(ptr->active && ptr->callbacks->timeout_cb != NULL) {
            print_main("[%s]time %llu %llu %llu", ptr->name, now, ptr->timecount, ptr->timeout);
            if(now - ptr->timecount >= ptr->timeout) {
                print_main("count %d, to %d", ptr->timecount, ptr->timeout);
                ptr->timecount = now;
                ret = ptr->callbacks->timeout_cb(ptr);
                if(ret != 0) {
                    if(ret == 9) {
                        print_main("timeout return 9; delete the proto!!");
                        delete_proto(ptr);
                    } else {
                        print_main("timeout & error close");
                        close_proto(ptr);
                    }
                }
            }
            // send(ptr->fd, "bye\n", 4, 0);
        }
        // print_main("%s %d %d", ptr->name, ptr->fd, ptr->active);
    }

    // print_main("dis msg!!!!");
    list_for_each_entry_safe(m_ptr, m_tmp, handle->msgs, list) {
        print_main("target %s", m_ptr->target);
        list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
            if(ptr->callbacks->disproto_cb != NULL) {
                if(strncmp(ptr->name, m_ptr->target, MAX_PNAME_LEN) == 0) {
                    if(ptr->callbacks->disproto_cb(ptr, m_ptr) < 0) {
                        close_proto(ptr);
                    }
                }
            }
        }
        del_msg(m_ptr);
    }


#endif
    // del_all_msg(handle->msgs);
    return 0;
}

static int _regist_proto(fd_set *fds, st_halo *handle)
{
    st_proto *ptr, *tmp;
    int max_fd = -1;
    if(handle == NULL) {
        return -1;
    }
    FD_ZERO(fds);
    list_for_each_entry_safe(ptr, tmp, handle->protos, list) {
        // print_main("++");
    #if 1
        if(ptr->fd >= 0 && ptr->active == 1) {
            // print_main("regist %s:%d", ptr->name, ptr->fd);
            FD_SET(ptr->fd, fds);
            max_fd = max_fd > ptr->fd ? max_fd : ptr->fd;
        } else {
            print_main("%s %d %d", ptr->name, ptr->fd, ptr->active);
        }
    #else
        if(ptr->fd < 0 && ptr->active == 1) {
            print_main("%s fd is illegal", ptr->name);
            close_proto(ptr);
            // delete_proto(ptr);
        } else {
            // print_main("regist %s:%d", ptr->name, ptr->fd);
            FD_SET(ptr->fd, fds);
            max_fd = max_fd > ptr->fd ? max_fd : ptr->fd;
        }
    #endif
    }
    // print_main("max fd is %d", max_fd);
    return max_fd;
}

static st_proto *_active_proto(fd_set *fds, st_halo *handle)
{
    if(handle == NULL) {
        print_main("handle is null!");
        return NULL;
    }
    st_proto *ptr;
    // print_main("active");
    list_for_each_entry(ptr, handle->protos, list) {
        // print_main("++");
        if(FD_ISSET(ptr->fd, fds)) {
            // print_main("%s is active", ptr->name);
            return ptr;
        }
    }
    print_main("no proto active");
    return NULL;
}

static void *halo_proc(void *arg)
{
    st_halo *handle = halo_handle;
    st_proto *pro, *tmp;
    fd_set fds;
    int max_fd, sel_ret, recv_ret;
    struct timeval t;
    print_common("common thread begin");
    while(1) {
        t.tv_sec = 1;
        t.tv_usec = 0;
        max_fd = _regist_proto(&fds, handle);
        max_fd = max_fd < 0 ? 0 : max_fd;
        sel_ret = select(max_fd + 1, &fds, NULL, NULL, &t);
        // sel_ret = select(0, &fds, NULL, NULL, &t);
        if(sel_ret > 0) {
            #if 1
            list_for_each_entry_safe(pro, tmp, handle->protos, list) {
                if(pro->fd >= 0 && pro->active && FD_ISSET(pro->fd, &fds)) {
                    --sel_ret;
                    print_main("get msg %s", pro->name);
                    #if 1
                    recv_ret = pro->callbacks->recv_cb(pro);
                    if(recv_ret == 0) {
                        if(pro->callbacks->reply_cb != NULL) {
                            if(pro->callbacks->reply_cb(pro)) {
                                close_proto(pro);
                            }
                        }
                    } else if(recv_ret < 0) {
                        close_proto(pro);
                    } 
                    else {
                        NULL;
                    }
                    #else
                    if(pro->callbacks->recv_cb(pro)) {
                        close_proto(pro);
                    } else {
                        if(pro->callbacks->reply_cb != NULL) {
                            if(pro->callbacks->reply_cb(pro)) {
                                close_proto(pro);
                            }
                        }
                    }
                    #endif
                }
                if(!sel_ret)
                    break;
            }
            #else
            pro = _active_proto(&fds, handle);
            if(pro != NULL) {
                print_main("get msg %s", pro->name);
                if(pro->callbacks->recv_cb(pro)) {
                    close_proto(pro);
                } else {
                    if(pro->callbacks->reply_cb != NULL) {
                        if(pro->callbacks->reply_cb(pro)) {
                            close_proto(pro);
                        }
                    }
                }
            }
            #endif
        } else if(sel_ret < 0) {
            if(errno == EINTR) {
                NULL;
            } else {
                print_main("error %d", errno);
                break;
            }
        }
        handle->manager_cb(handle);
    }
    return NULL;
}

int halo_start()
{
    if(halo_handle == NULL) {        print_main("halo need to init");
        return -1;
    }
    return pthread_create(&halo_handle->pid, NULL, halo_proc, NULL);
}

int update_status(splayer_status_t *arg)
{
    if(halo_handle == NULL) {
        return -1;
    }
    st_proto *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, halo_handle->protos, list) {
        if( ptr->fd >= 0 && 
            ptr->active && 
            ptr->callbacks->active_send_cb != NULL) 
        {
            memcpy(&ptr->play_status, arg, sizeof(splayer_status_t));
            if(ptr->callbacks->active_send_cb(ptr)) {
                print_main("active send %s error", ptr->name);
                close_proto(ptr);
            }
        }
    }
    return 0;
}

// int main(int argc, char *argv[])
// {
//     halo_init(NULL);
//     add_pipe();
//     add_tcp_cli();
//     add_tcp_ser();
//     udp_service_init();
//     halo_start();
//     wifi_connected = 1;
//     alink_connect = 1;
//     while(1){
//         // sleep(5);
//         sleep(1);
//         update_pipe(NULL);
//         // wifi_connected = !wifi_connected;
//         // alink_connect = !alink_connect;
//         // print_main("%d", wifi_connected);
//     }
//     return 0;
// }