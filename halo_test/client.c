
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


#if 0
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
     localInterface.s_addr = htonl(INADDR_ANY);
    //  localInterface.s_addr = inet_addr("127.0.0.1");
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

#else



#define MAXDATASIZE 256

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1500];

int dispatch(char *source, int len, int type, void *ret){

}


int main(int argc, char *argv[])
{
    int num;
    if(argc != 2){
        printf("usage %s <input>\r\n", argv[0]);
        return 1;
    }
    struct sockaddr_in client;
    struct sockaddr_in groupSock;
    socklen_t len;
    bzero(&groupSock, sizeof(groupSock));
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
    localSock.sin_port = htons(19602);
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

    // group.imr_interface.S_un.s_addr = inet_addr("127.0.0.1");
    // group.imr_multiaddr.S_un.s_addr = inet_addr("224.0.0.1");

    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    } else
        printf("Adding multicast group...OK.\n");
    setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localSock, sizeof(localSock));
    datalen = sizeof(databuf);
  
    len = sizeof(client);
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("224.0.0.1");
    groupSock.sin_port = htons(19602);
    
    bzero(databuf, sizeof(databuf));
    strcpy(databuf, argv[1]);
    sendto(sd, databuf, datalen, 0, (struct sockaddr*)&group, sizeof(group));
    while(1){
        printf("wait for multicast\r\n");
        bzero(databuf, sizeof(databuf));
        num = recvfrom(sd, databuf, MAXDATASIZE, 0, (struct sockaddr *)&client,&len);
        if(num < 0){
            close(sd);
            perror("recvfrom error");
            exit(1);
        }
        printf("recv msg %d, %s\r\n", num, databuf);
        break;
        // databuf[num] = 0;
        // char *msg = "hello";
        // // client.sin_port = htons(19601);
        // sendto(sd, msg, strlen(msg), 0, (struct sockaddr*)&client, len);
        // if(!strcmp(databuf, "bye")){
        //     break;
        // }
    }
    close(sd);
    
    return 0;
}
#endif