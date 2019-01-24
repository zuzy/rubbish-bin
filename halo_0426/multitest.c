#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"


#define MAXDATASIZE 1024

char* cmd = "{\"cmd\":\"hopediscover\",\"params\":{\"deviceid\":\"e076d092265b\",\"bool\":false,\"arr\":[1,2,3,4]}}";

// char* cmd="{\"name\":\"fengxin\",\"passwd\":\"123\",\"num\":1}";  

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1500];

int multisend(char *str, int len){
    int ret;
    struct sockaddr_in groupSock;
    socklen_t sock_len;    
    if(sd < 0){
        perror("sock fd error");
        exit(1);
    }

    bzero(&groupSock, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("224.0.0.1");
    groupSock.sin_port = htons(19601);
    return sendto(sd, str, len, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
}

int dispatch(char *source, int len, int type, void *ret){

    
}


int main(int argc, char *argv[])
{
#if 0
    int num;

    struct sockaddr_in client;
    socklen_t len;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Opening datagram socket error");
        exit(1);
    } else
        printf("Opening datagram socket....OK.\n");
  
    int opt = SO_REUSEADDR;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(19602);
    localSock.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localSock, sizeof(localSock));

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
  
    datalen = sizeof(databuf);
#if 0
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
    len = sizeof(client);
    multisend(cmd, strlen(cmd));
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
        databuf[num] = 0;
        char *msg = "hello";
        // client.sin_port = htons(19601);
        sendto(sd, msg, strlen(msg), 0, (struct sockaddr*)&client, len);
        if(!strcmp(databuf, "bye")){
            break;
        }
    }
#endif
    close(sd);
#else
    char *str = "{\"cmd\":\"devicecontrol\",\"data\":[{\"state\":\"STATE_ON\",\"name\":\"灯\",\"ids\":[\"0\",\"1\",\"2\"]}]}";
    cJSON *root = cJSON_Parse(str);
    if(root == NULL) {
        printf("cjson parse error\n");
        return 0;
    }
    cJSON *data = cJSON_GetObjectItem(root, "data");
    if(data == NULL) {
        printf("cjosn parse data error\n");
        return 0;
    }
    if(data->type == cJSON_Array) {
        printf("data is array\n");
    }


#endif
    return 0;
}
