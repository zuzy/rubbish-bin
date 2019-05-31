#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234

#define BACKLOG 1

#define exit _exit

void main(){
    int listenfd, connectfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t addrlen;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket error");
        _exit(1);
    }

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("bind error");
        exit(1);
    }
    
    if(listen(listenfd, BACKLOG) == -1){
        perror("listen error");
        exit(1);
    }
    
    int len = sizeof(client);

    if((connectfd = accept(listenfd, (struct sockaddr*)&client, &addrlen)) == -1){
        perror("accept error");
        exit(1);
    }
    printf("you got a connection from client's ip is %s, port is %d\n",
        inet_ntoa(client.sin_addr), htons(client.sin_port));

    send(connectfd, "Welcome\n", 8, 0);
    
    close(connectfd);
    close(listenfd);
}
