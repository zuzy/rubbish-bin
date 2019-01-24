#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 123

#define MAXDATASIZE 100

#define int8 char
#define uint8 unsigned char
#define uint32 unsigned int
#define ulong32 unsigned long
#define long32 long
#define int32 int
#define long64 long long


typedef struct NTPPACKET
{
  uint8 li_vn_mode;
  uint8 stratum;
  uint8 poll;
  uint8 precision;
  ulong32 root_delay;
  ulong32 root_dispersion;
  int8 ref_id[4];
  ulong32 reftimestamphigh;
  ulong32 reftimestamplow;
  ulong32 oritimestamphigh;
  ulong32 oritimestamplow;
  ulong32 recvtimestamphigh;
  ulong32 recvtimestamplow;
  ulong32 trantimestamphigh; 
  ulong32 trantimestamplow;
}NTPPacket;

long64 firsttimestamp,finaltimestamp;
long64 diftime,delaytime;

int main(int argc, char *argv[]){

    NTPPacket ntppack, newpack;
    unsigned char msg[48] = {0};
    int sockfd, num;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in server, peer;
    struct timeval tv,tv1;
    // if(argc != 3){
    //     printf("usage: %s <ip address> <message>\n", argv[0]);
    //     exit(1);
    // }
    bzero(&ntppack, sizeof(ntppack));
    if((he = gethostbyname("ntp3.aliyun.com")) == NULL){
        printf("gethostbyname error");
        exit(1);
    }
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("socket() error");
        exit(1);
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    msg[0] = 0x1b;
    sendto(sockfd, msg, 48, 0, (struct sockaddr *)&server, sizeof(server));

    socklen_t len = sizeof(server);
    while(1){
        if((num = recvfrom(sockfd, &ntppack, sizeof(ntppack), 0, (struct sockaddr *)&peer, &len)) == -1){
            printf("recvfrom error\r\n");
            exit(1);
        }

        newpack.root_delay= ntohl(newpack.root_delay);
        newpack.root_dispersion= ntohl(newpack.root_dispersion);
        newpack.reftimestamphigh=ntohl(newpack.reftimestamphigh);
        newpack.reftimestamplow= ntohl(newpack.reftimestamplow);
        newpack.oritimestamphigh= ntohl(newpack.oritimestamphigh);
        newpack.oritimestamplow= ntohl(newpack.oritimestamplow);
        newpack.recvtimestamphigh= ntohl(newpack.recvtimestamphigh);
        newpack.recvtimestamplow= ntohl(newpack.recvtimestamplow);
        newpack.trantimestamphigh= ntohl(newpack.trantimestamphigh);
        newpack.trantimestamplow= ntohl(newpack.trantimestamplow);

         diftime=((newpack.recvtimestamphigh-firsttimestamp)+(newpack.trantimestamphigh-finaltimestamp))>>1;
        //求出延时
        delaytime=((newpack.recvtimestamphigh-firsttimestamp)-(newpack.trantimestamphigh-finaltimestamp))>>1; 


        //求出真正时间的时间戳
        tv1.tv_sec=time(NULL)+diftime+delaytime;
        tv1.tv_usec=0;
        //tz.

        printf("\n\ndebug information ...\n\n");
        printf("time(NULL) is %ld\n",time(NULL));
        printf("different time is %ld\n",diftime); 
        printf("delaytime is %ld\n",delaytime);
        printf("time(NULL)+diftime+delaytime=%ld\n",time(NULL)+diftime+delaytime);
        printf("tv1.tv_sec is %ld\n\n", tv1.tv_sec);

        // if(len != sizeof(server) || 
        //     memcmp((const void *)&server, (const void *)&peer, len) != 0){
        //     printf("receive message from other server.\n");
        //     continue;
        // }
        // printf("get message:\r\n");
        // int i;
        // for(i = 0; i < num; i++){
        //     printf("%d:\t%02x\r\n", i, buf[i]);
        // }
        // printf("\r\n");
        break;
    }
    close(sockfd);
}