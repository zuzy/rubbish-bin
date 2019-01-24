#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>   
#include <errno.h>

#include "tcp_server.h"
#include "cJSON.h"
#include "log.h"
#include "command.h"
#include "halo_common.h"

#include "tcp_server_callback.c"

typedef int (*getparams)(cJSON*);

#define CREATE_CMD(str) {#str,(getparams)cmd_##str}



static struct _st_cmd{
    char *cmd;
    getparams callback;
}_st_cmd;

int dispatch_server(int ,char *recvbuf, int num);

struct _st_cmd cmd_list[] = {
    CREATE_CMD(info),
    CREATE_CMD(control),
    CREATE_CMD(getsonglist),
    CREATE_CMD(getplaylist),
    CREATE_CMD(querysonginfo),
    CREATE_CMD(operlist),
    CREATE_CMD(exit),
    CREATE_CMD(opertunnel),
    CREATE_CMD(deviceinfo),
    CREATE_CMD(hopediscover),
    {NULL, NULL}
};

void process_recv(int connfd, struct sockaddr_in client);
void *recv_thread(void *arg);
void process_send(int connfd, struct sockaddr_in client);
void *send_thread(void *arg);
void savedata(char *recvbuf, int len, char* cli_data);
void *function(void *arg);

volatile int send_status;
volatile char send_buff[MAXDATASIZE];
volatile int client_num;

static int listenfd;

struct ARG{
    int connfd;
    struct sockaddr_in client;
};

struct _st_connfd_list{
    int connfd;
    struct _st_connfd_list *next;
};

struct _st_connfd_list *head;

static void _print_connfd_list(){
    struct _st_connfd_list *p = head;
    int i = 0;
    while(p != NULL){
        printf("connfd %d: %d\n", i, p->connfd);
        p = p->next;
        i++;
    }
    return;
}

static int _delete_all_list(){
    struct _st_connfd_list *p, *next;
    next = head;
    
    while(next != NULL){
        p = next;
        next = next->next;
        free(p);
        p = NULL;
    }
    return 0;
}

static int _init_connfd_list(){
    return _delete_all_list();
}

static int _add_connfd_list(int fd){
    if(fd < 0){
        perror("fd input illegal!\n");
        return -1;
    }
    struct _st_connfd_list *p;

    struct _st_connfd_list *ptr = malloc(sizeof(struct _st_connfd_list));
    bzero(ptr, sizeof(struct _st_connfd_list));
    ptr->connfd = fd;
    if(head == NULL){
        head = ptr;
        printf("add head %d\n", fd);
        return 0;

    }else{
        p = head;
        int i = 0;
        while(p->next != NULL && p->connfd != fd){
            printf("%d: %d\n", i++, p->connfd);
            p = p->next;
        }
        if(p->next == NULL){
            p->next = ptr;
            printf("add tail %d\n", fd);
            return 0;
        }
    }
    printf("fd exist %d\n", fd);
    // connfd_list = connfd_list->next;
    return -1;
}

static int _delete_from_list(int fd){
    struct _st_connfd_list *p, *next, *prev;
    prev = head;
    if(head->connfd == fd){
        head = head->next;
        free(prev);
        prev = NULL;
        return 0;
    }
    p = prev->next;
    while(p != NULL){
        if(p->connfd == fd){
            next = p->next;
            prev->next = next;
            free(p);
            p = NULL;
            printf("fd[%d] is found and deleted\n", fd);
            return 0;
        }
        prev = p;
        p = p->next;
    }
    perror("fd is not found\n");
    return -1;
}
/*
regist whole connfd list to the fd_set*;
return the biggest connfd to select;
*/
static int _regist_connfd_list(fd_set *fds){
    printf("regist_list\n");
    struct _st_connfd_list *p = head;
    FD_ZERO(fds);
    int max_fd = -1;
    while(p != NULL){
        FD_SET(p->connfd, fds);
        max_fd = max_fd > p->connfd ? max_fd : p->connfd;
        p = p->next;
    }
    printf("get max fd is %d", max_fd);
    return max_fd;
}

static int _get_active_fd(fd_set *fds){
    struct _st_connfd_list *p = head;
    while(p != NULL){
        if(FD_ISSET(p->connfd, fds)){
            return p->connfd;
        }
        p = p->next;
    }
    return -1;
}
/* ------------------
halo broad send content;
return the num of successfull sendations;
----------------------*/

int halo_broadcast(char *content, size_t len){
    struct _st_connfd_list *p, *next;
    p = head;
    int num = 0;
    while(p != NULL){
        if(p->connfd == listenfd){
            p = p->next;
            continue;
        }
        if(send(p->connfd, content, len, 0) != len){
            printf("send %d error close and delete!\n", p->connfd);
            next = p->next;
            _delete_from_list(p->connfd);
            p = next;
        }else{
            ++num;
            p = p->next;
        }
    }
    return num;
}


void *halo_tcp_proc(void *arg){
    printf("halo tcp server start\n");
    client_num = 0;
    arg = NULL;
    struct ARG *args;
    int connfd;
    pthread_t tid[2];
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len = 0;
    fd_set fds;
    int max_fd = 0;
    int sel_ret = 0;
    int active_fd = 0;
    _init_connfd_list();
    char recvbuf[MAXDATASIZE];
    int num = 0;
    do{
        if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            perror("socket() error.");
            break;
        }
        int opt = SO_REUSEADDR;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
            perror("bind error.");
            break;;
        }
        if(listen(listenfd, BACKLOG) == -1){
            perror("listen error");
            break;
        }
        printf("head is %d\n", head);
        _add_connfd_list(listenfd);
        printf("head now is %d\n", head);
        len = sizeof(client);
        pthread_create(&tid[0], NULL, send_thread, NULL);
        while(1){
            _print_connfd_list();
            max_fd = _regist_connfd_list(&fds);
            printf("max fd is %d; begin to select\n", max_fd);
            sel_ret = select(max_fd + 1, &fds, NULL, NULL, NULL);
            if(sel_ret > 0){
                active_fd = _get_active_fd(&fds);
                if(active_fd == listenfd){
                    if((connfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1){
                        perror("accept error.");
                        break;
                    }
                    printf("you got a connection from %s.\r\n", inet_ntoa(client.sin_addr));
                    _add_connfd_list(connfd);
                }else{
                    bzero(recvbuf, sizeof(recvbuf));
                    num = recv(active_fd, recvbuf, MAXDATASIZE, 0);
                    if(num <= 0){
                        printf("client disconnect\r\n");
                        close(active_fd);
                        _delete_from_list(active_fd);
                    }else{
                        recvbuf[num] = 0;
                        printf("%s\r\n", recvbuf);
                        halo_broadcast(recvbuf, num);
                        dispatch_server(active_fd, recvbuf, num);
                    }
                }
            }else if (sel_ret == 0 || sel_ret == EINTR){
                continue;
            }else{
                perror("select error\n");
                break;
            }

            // args = (struct ARG *)malloc(sizeof(struct ARG));
            // args->connfd = connfd;
            // memcpy(&args->client, &client, sizeof(client));
            // if(pthread_create(&tid[0], NULL, recv_thread, args)){
            //     perror("thread create error.");
            //     break;
            // }
            // if(pthread_create(&tid[1], NULL, send_thread, args)){
            //     perror("thread create error.");
            //     break;
            // }
            // ++client_num;
        }
    }while(0);
    close(listenfd);
}

void process_recv(int connfd, struct sockaddr_in client){
    fd_set readfds;
    int sel_ret;
    struct timeval t;
    int num;
    char cli_data[1000];
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cliname[MAXDATASIZE];
    printf("you got a connection from %s.\r\n", inet_ntoa(client.sin_addr));
    sel_ret = 0;

    while(1){
        t.tv_sec = HEARTBEAT;
        t.tv_usec = 0;        
        FD_ZERO(&readfds);
        FD_SET(connfd, &readfds);
        usleep(100);
        sel_ret = select(connfd+1, &readfds, NULL, NULL, &t);
        if(sel_ret < 0){
            printf("select error\r\n");
            break;
        }else if(sel_ret == 0){
            printf("time out \r\n");
            break;
        }else{
            if(FD_ISSET(connfd, &readfds)){
                bzero(recvbuf, sizeof(recvbuf));
                num = recv(connfd, recvbuf, MAXDATASIZE, 0);
                if(num <= 0){
                    printf("client disconnect\r\n");
                    break;
                }else{
                    recvbuf[num] = 0;
                    printf("%s\r\n", recvbuf);

                    // dispatch_server(recvbuf, num);
                }
            }
        }
        usleep(100);
    }
    close(connfd);
}

void *recv_thread(void *arg){
    struct ARG *info;
    info = (struct ARG *)arg;
    process_recv(info->connfd, info->client);
    free(arg);
    pthread_exit(NULL);
}


void *send_thread(void *arg){
    int ret;
    struct ARG *info;
    info = (struct ARG *)arg;
    pthread_detach(pthread_self());
    char *str = "hallo world";
    while(1){
        sleep(2);
        if(halo_broadcast(str, strlen(str))){
            printf("send %s\n", str);
        }
    }
    pthread_exit(NULL);
}

void savedata(char *recvbuf, int len, char *clidata){
    static int index = 0;
    int i = 0;
    while(i < len-1){
        clidata[index++] = recvbuf[i];
        i++;
    }
    clidata[index] = 0;

}



static int dispatch(char* cmd, int len){
    cJSON *root = cJSON_Parse(cmd);
    if(!root){
        log("get json error!");
        return -1;
    }
    cJSON *cmd_item = cJSON_GetObjectItem(root, "cmd");

}


int dispatch_server(int fd, char *recvbuf, int num){
    int ret;
    cJSON *root = cJSON_Parse(recvbuf);
    if(!root){
        return -1;
    }
    char *cmd_str = GET_STRING_FROM_KEY(root, "cmd");
    if(!cmd_str){
        ret = -2;
        goto Exit;
    }
    struct _st_cmd *cmd_ptr = cmd_list;
    while(cmd_ptr->cmd){
        if(!strcmp(cmd_str, cmd_ptr->cmd)){
            cmd_ptr->callback(cJSON_GetObjectItem(root, "params"));
            break;
        }
        ++cmd_ptr;
    }
    printf("dispatch end\r\n");
Exit:
    if(root)cJSON_Delete(root);
    return ret;
}
