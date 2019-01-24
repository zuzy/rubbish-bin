#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> // needed for hostent; gethostbyname

#define PORT 1234

#define MAXDATASIZE 100

int main(int argc, char *argv[]){
    int sockfd, num;
    char buf[MAXDATASIZE];
    struct hostent *he; //structure that will get information about remote host
    struct sockaddr_in server;
    
    if(argc != 2){
        printf("Usage: %s <IP Address>\r\n", argv[0]);
        _exit(1);
    }
    
    if((he = gethostbyname(argv[1])) == NULL){
        printf("gethost by name error\n");
        _exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("socket() error\r\n");
        _exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);

    if(connect(sockfd, (struct in_addr*)&server, sizeof(server)) == -1){
        printf("connect error\r\n");
        _exit(1);
    }
    printf("connect ok\n");
    if((num = recv(sockfd, buf, MAXDATASIZE, 0)) == -1){
        printf("recv error\r\n");
        _exit(1);
    }
    buf[num - 1] = '\0';
    printf("server message: %s\r\n", buf);
    close(sockfd);
}