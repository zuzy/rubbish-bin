#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1234
#define BACKLOG 5
#define MAXDATASIZE 100
#define exit(n) _exit(n)

void process_cli(int connfd, struct sockaddr_in client);
void *function(void *arg);
struct ARG{
    int confd;
    struct sockaddr_in client;
};

int main(){
    int listenfd, connfd;
    pthread_t tid;
    struct ARG *arg;
    struct sockaddr_in server, client;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket() error.");
        exit(1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("bind() error.");
        exit(1);
    }
    if(listen(listenfd, BACKLOG) == -1){
        perror("listen() error.");
        exit(1);
    }
    int len = sizeof(client);
    while(1){
        if((connfd = accept(listenfd, (struct sockaddr*)&client, &len)) == -1){
            perror("accept error.");
            exit(1);
        }
        arg = (struct ARG*)malloc(sizeof(struct ARG));
        arg->confd = connfd;
        memcpy((void *)&arg->client, &client, sizeof(client));
        #if 1
        pthread_create(&tid, NULL, function, arg);
        #else
        if(pthread_create(&tid, NULL, function, (void*)arg)){
            perror("pthread create error.");
            exit(1);
        }
        #endif
    }
    close(listenfd);
}

void process_cli(int connfd, struct sockaddr_in client){
    int num;
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cliname[MAXDATASIZE];
    printf("you got a connection from %s.\r\n", inet_ntoa(client.sin_addr));
    num = recv(connfd, cliname, MAXDATASIZE, 0);
    if(num == 0){
        close(connfd);
        printf("client disconnected\r\n");
        return;
    }
    cliname[num - 1] = '\0';
    printf("client's name is %s.\r\n", cliname);
    while(num = recv(connfd, recvbuf, MAXDATASIZE, 0)){
        recvbuf[num] = '\0';
        printf("received client(%s) message: %s", cliname, recvbuf);
        int i;
        bzero(sendbuf, sizeof(sendbuf));
        memcpy(sendbuf, recvbuf, num);
        send(connfd, sendbuf, strlen(sendbuf), 0);
    }
    close(connfd);
}

void * function(void *arg){
    struct ARG *info;
    info = (struct ARG *)arg;
    process_cli(info->confd, info->client);
    free (arg);
    pthread_exit(NULL);
}