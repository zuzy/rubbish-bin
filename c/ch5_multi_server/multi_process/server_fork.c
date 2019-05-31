#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define exit(n) _exit(n)
#define PORT 1234
#define BACKLOG 5
#define MAXDATASIZE 1000

void process_cli(int connfd, struct sockaddr_in client);

int main(){
    int listenfd, connfd;
    pid_t pid;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int len;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Creating socket error");
        exit(1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("listen error");
        exit(1);
    }
    if(listen(listenfd, BACKLOG) == -1){
        perror("listen error");
        exit(1);
    }
    len = sizeof(client);
    while(1){
        if((connfd = accept(listenfd, (struct sockaddr *)&client, &len)) == -1){
            perror("accept error");
            exit(1);
        }
        if((pid = fork()) > 0){
            close(connfd);
            continue;
        }else if(pid == 0){
            close(listenfd);
            process_cli(connfd, client);
            exit(0);
        }else{
            printf("fork() error\r\n");
            exit(0);
        }
    }
    close(listenfd);
}

void process_cli(int connfd, struct sockaddr_in client){
    int num;
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cliname[MAXDATASIZE];
    printf("you got a connection from %s.", inet_ntoa(client.sin_addr));
    num = recv(connfd, cliname, MAXDATASIZE, 0);
    if(num == 0){
        close(connfd);
        printf("client disconnected\r\n");
        return;
    }
    cliname[num - 1] = '\0';
    printf("client name is %s.\r\n", cliname);
    while(num = recv(connfd, recvbuf, MAXDATASIZE, 0)){
        recvbuf[num] = '\0';
        printf("received client(%s) message: %s", cliname, recvbuf);
        int i = 0;
        for(i = 0; i < num - 1; i++){
            if((recvbuf[i] >= 'a' && recvbuf[i] <= 'z') || (recvbuf[i] >= 'A' && recvbuf[i] <= 'Z')){
                recvbuf[i] += 3;
                if((recvbuf[i] > 'Z' && recvbuf[i] < 'Z' + 3) || (recvbuf[i] > 'z')){
                    recvbuf[i] -= 26;
                }
            }
            sendbuf[i] = recvbuf[i];
        }
        sendbuf[num - 1] = '\0';
        send(connfd, sendbuf, strlen(sendbuf), 0);
    }
    close(connfd);
}