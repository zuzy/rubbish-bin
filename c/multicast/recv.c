#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDATASIZE 256

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1500];


int main(int argc, char *argv[])
{
    int num;

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Opening datagram socket error");
        exit(1);
    } else
        printf("Opening datagram socket....OK.\n");
  
    {
        int reuse = 1;
        if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
            perror("Setting SO_REUSEADDR error");
            close(sd);
            exit(1);
        } else
            printf("Setting SO_REUSEADDR...OK.\n");
    }

    int opt = SO_REUSEADDR;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(19601);
    localSock.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))){
        perror("Binding datagram socket error");
        close(sd);
        exit(1);
    } else
        printf("Binding datagram socket...OK.\n");
  
    // group.imr_multiaddr.s_addr = inet_addr("227.0.0.25");
    // group.imr_interface.s_addr = inet_addr("150.158.231.2");
    group.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    group.imr_interface.s_addr = inet_addr("127.0.0.1");

    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    } else
        printf("Adding multicast group...OK.\n");
  
    datalen = sizeof(databuf);
#if 1
    if(read(sd, databuf, datalen) < 0){
        perror("Reading datagram message error");
        close(sd);
        exit(1);
    } else {
        printf("Reading datagram message...OK.\n");
        printf("The message from multicast server is: %d\n", datalen);
        printf("message is %s\r\n", databuf);
    }

#else    
    while(1){
        bzero(databuf, sizeof(databuf));
        num = recvfrom(sd, databuf, MAXDATASIZE, 0, (struct sockaddr *)&client,&len);
        if(num < 0){
            close(sd);
            perror("recvfrom error");
            exit(1);
        }
        printf("recv msg %d, %s", num, databuf);
        databuf[num] = 0;
        char *msg = "hello";
        sendto(sd, msg, strlen(msg), 0, (struct sockaddr*)&client, len);
        if(!strcmp(databuf, "bye")){
            break;
        }
    }
#endif
    close(sd);
    
    return 0;
}
