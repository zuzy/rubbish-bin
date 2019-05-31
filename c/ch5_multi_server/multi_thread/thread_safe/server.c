#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1234
#define MAXDATASIZE 100
#define BACKLOG 5

void process_cli(int connfd, struct sockaddr_in client);
void *function(void *arg);
void savedata(char *recvbuf, int len, char* cli_data);
void *function(void *arg);
struct ARG{
    int connfd;
    struct sockaddr_in client;
};

int main(){
    int listenfd, connfd;
    pthread_t tid;
    struct ARG *arg;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;
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
        perror("bind error.");
        exit(1);
    }
    if(listen(listenfd, BACKLOG) == -1){
        perror("listen error");
        exit(1);
    }
    len = sizeof(client);
    while(1){
        if((connfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1){
            perror("accept error.");
            exit(1);
        }
        arg = (struct ARG *)malloc(sizeof(struct ARG));
        arg->connfd = connfd;
        memcpy(&arg->client, &client, sizeof(client));
        if(pthread_create(&tid, NULL, function, arg)){
            perror("thread create error.");
            exit(1);
        }
    }
    close(listenfd);
}

void process_cli(int connfd, struct sockaddr_in client){
    int num;
    char cli_data[1000];
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cliname[MAXDATASIZE];
    printf("you got a connection from %s.\r\n", inet_ntoa(client.sin_addr));
    num = recv(connfd, cliname, MAXDATASIZE, 0);
    if(num == 0){
        close(connfd);
        printf("client disconnected\r\n");
        return;
    }
    cliname[num - 1] = '\0';
    printf("client name is %s\r\n", cliname);
    while(num = recv(connfd, (void *)recvbuf, MAXDATASIZE, 0)){
        printf("received client(%s) message: %s", cliname, recvbuf);
        savedata(recvbuf, num, cliname);
        bzero(sendbuf, sizeof(sendbuf));
        memcpy(sendbuf, recvbuf, num);
        sendbuf[num - 1] = 0;
        send(connfd, sendbuf, strlen(sendbuf), 0);
    }
    close(connfd);
    printf("client(%s) closed connection.\r\nuser's data: %s\r\n", cliname, cli_data);
}

void *function(void *arg){
    struct ARG *info;
    info = (struct ARG *)arg;
    process_cli(info->connfd, info->client);
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
