#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

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

int dispatch_server(char *recvbuf, int num);

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

struct ARG{
    int connfd;
    struct sockaddr_in client;
};

void *halo_tcp_proc(void *arg){
    printf("halo tcp server start\n");
    client_num = 0;
    arg = NULL;
    struct ARG *args;
    int listenfd, connfd;
    pthread_t tid[2];
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
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

                    dispatch_server(recvbuf, num);
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
    while(1){
        while(!send_status){
            usleep(50);
        }
        ret = send(info->connfd, (const char*)send_buff, strlen((const char*)send_buff), 0);
        printf("send ret is %d\r\n", ret);
        if(ret <= 0){
            break;
        }
        usleep(100);
    }
    close(info->connfd);
    free(arg);
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


int dispatch_server(char *recvbuf, int num){
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
