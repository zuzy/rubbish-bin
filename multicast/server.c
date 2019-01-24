
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXDATASIZE 1024
struct in_addr localInterface;
struct sockaddr_in groupSock, client;
int sd;
char databuf[MAXDATASIZE] = "Multicast test message lol!";
int datalen = sizeof(databuf);


int main (int argc, char *argv[ ])
{
     sd = socket(AF_INET, SOCK_DGRAM, 0);
     if(sd < 0) {
          perror("Opening datagram socket error");
          exit(1);
     } else
          printf("Opening the datagram socket...OK.\n");


     memset((char *) &groupSock, 0, sizeof(groupSock));
     groupSock.sin_family = AF_INET;
     groupSock.sin_addr.s_addr = inet_addr("224.0.0.1");
     groupSock.sin_port = htons(19601);


    //  localInterface.s_addr = inet_addr("203.106.93.94");
     localInterface.s_addr = inet_addr("127.0.0.1");
     if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0) {
        perror("Setting local interface error");
        exit(1);
     } else
        printf("Setting the local interface...OK\n");

#if 0
     if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0) {
        perror("Sending datagram message error");}
     else
        printf("Sending datagram message...OK\n");
#else     
    strcpy(databuf, argv[1]);
     sendto(sd, databuf, strlen(databuf), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
     int num, len;
     len = sizeof(client);
    while(1){
        bzero(databuf, sizeof(databuf));
        num = recvfrom(sd, databuf, MAXDATASIZE, 0, (struct sockaddr*)&client, &len);
        databuf[num] = 0;
        printf("recv %d: %s\r\n", num, databuf);
        break;
    }
#endif        
    close(sd);
     return 0;
}

