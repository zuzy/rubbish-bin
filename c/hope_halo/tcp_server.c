#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "applib.h"
#include "act_debug.h"

#include "tcp_server.h"
#include "cJSON.h"
// #include "log.h"
#include "command.h"
#include "common.h"
#include "dispatchcmd.h"

#include "tcp_server_callback.h"

#define MAX_CMD_NUM     10

// cJSON_AddItemToObject(root, "params", params);
#define ADD_ITEM(json,params) cJSON_AddItemToObject(json,#params,params)

void process_recv(int connfd, struct sockaddr_in client);
void *recv_thread(void *arg);

volatile int send_status;
volatile char g_send_buff[MAXDATASIZE];
static int p_fd[2];
struct ARG{
    int connfd;
    struct sockaddr_in client;
};

void *halo_tcp_proc(void *arg){
    printf("halo tcp server start\n");
    arg = NULL;
    struct ARG *args;
    int listenfd, connfd;
    pthread_t tid[2];
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
    do{
        if(pipe(p_fd) < 0){
            perror("pipe create error");
            break;
        }
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
        len = sizeof(client);
        while(1){
            if((connfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1){
                perror("accept error.");
                break;
            }
            args = (struct ARG *)malloc(sizeof(struct ARG));
            args->connfd = connfd;
            memcpy(&args->client, &client, sizeof(client));
            if(pthread_create(&tid[0], NULL, recv_thread, args)){
                perror("thread create error.");
                break;
            }
        }
    }while(0);
    close(listenfd);
}
int dispatch_cmd(char *recvbuf, int num){
    int ret = 0;
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
            ret = cmd_ptr->callback(cJSON_GetObjectItem(root, "params"));
            break;
        }
        ++cmd_ptr;
    }
    printf("dispatch end\r\n");
Exit:
    if(root)cJSON_Delete(root);
    return ret;
}

static void exit_connection(int fd){
    cJSON *root = cJSON_CreateObject();
    cJSON *params = cJSON_CreateObject();
    ADD_ITEM(root,params);
    cJSON_AddStringToObject(params, "deviceid", "");
    char *out = cJSON_Print(root);
    send(fd, out, strlen(out), 0);
    cJSON_Delete(root);
}

void process_recv(int connfd, struct sockaddr_in client){
    fd_set readfds;
    int sel_ret;
    struct timeval t;
    int num;
    char cli_data[1000];
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE];
    printf("you got a connection from %s.\r\n", inet_ntoa(client.sin_addr));
    sel_ret = 0;
    int max_fd = (connfd > p_fd[0])? connfd : p_fd[0];

    while(1){
        t.tv_sec = HEARTBEAT;
        t.tv_usec = 0;        
        FD_ZERO(&readfds);
        FD_SET(connfd, &readfds);
        FD_SET(p_fd[0], &readfds);
        usleep(100);
        sel_ret = select(max_fd+1, &readfds, NULL, NULL, &t);
        if(sel_ret < 0){
            if(sel_ret == EINTR){
                continue;
            }
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
                    if(dispatch_cmd(recvbuf, num) == -10){
                        exit_connection(connfd);
                    }
                }
            }
            if(FD_ISSET(p_fd[0], &readfds)){
                bzero(sendbuf, sizeof(sendbuf));
                num = read(p_fd[0], sendbuf, MAXDATASIZE);
                send(connfd, sendbuf, num, 0);
            }
        }
        usleep(100);
    }
    close(connfd);
}

// void test_send(void *param){
//     char *str = "hello world";
//     write(p_fd[1], str, strlen(str));
// }

void *recv_thread(void *arg){
    struct ARG *info;
    info = (struct ARG *)arg;
    process_recv(info->connfd, info->client);
    // set_timer(1000, test_send, NULL);
    free(arg);
    pthread_exit(NULL);
}

