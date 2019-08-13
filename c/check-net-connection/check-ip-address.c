#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>

int _get_ip_from_interface(char *ip, size_t ip_size, char *interface)
{
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(fd < 0) {
        return -1;
    }
    // ifr.ifr_addr.sa_family = AF_INET;
    // strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    strncpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), ip_size);
    
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    int i;
    for (i = 0; i < 6; ++i)
        printf("%.02x", (unsigned char) ifr.ifr_addr.sa_data[i]);
    printf("\n");
    close(fd);

    return 0;
}

int _get_info_from_interface(char *ip, char * mac, char *interface)
{
    int fd, ret;
    struct ifreq ifr;
    char ip_tmp[64] = "\0";
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }
    ifr.ifr_addr.sa_family = AF_INET;
    // strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if((ret = ioctl(fd, SIOCGIFADDR, &ifr)) < 0) {
        goto End;
    }
    strcpy(ip_tmp, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    if(strlen(ip_tmp) == 0) {
        ret = -2;
        goto End;
    }
    strcpy(ip, ip_tmp);

    if((ret = ioctl(fd, SIOCGIFHWADDR, &ifr)) < 0) {
        goto End;
    }
    sprintf(mac, "%.02x:%.02x:%.02x:%.02x:%.02x:%.02x", 
        (unsigned char) ifr.ifr_addr.sa_data[0],
        (unsigned char) ifr.ifr_addr.sa_data[1],
        (unsigned char) ifr.ifr_addr.sa_data[2],
        (unsigned char) ifr.ifr_addr.sa_data[3],
        (unsigned char) ifr.ifr_addr.sa_data[4],
        (unsigned char) ifr.ifr_addr.sa_data[5]);
End:
    close(fd);
    return ret;
}


int main()
{
    char ip[64] = "\0";
    char *interface = "enp5s0";
    // char *interface = "eth0";
    int ret = _get_ip_from_interface(ip, sizeof(ip), interface);
    printf("ret is %d\n");
    printf("get ip %s\n", ip);

    char mac[64] = "\0";
    ret = _get_info_from_interface(ip, mac, interface);
    printf("get ip: %s mac: %s[end]\n", ip, mac);

    return 0;
}

