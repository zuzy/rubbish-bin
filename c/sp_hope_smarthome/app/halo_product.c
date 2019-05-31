#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>

#include "applib.h"
#include "net/wifi_manager.h"
#include "net/config.h"
#include "act_debug.h"

#include "common.h"
#include "halo_product.h"

static char *ver = "1.0";
static char *cid = "750832243333435392";
static char *sid = "750837261197414400";
static char *key = "306BE98143724C6481E5F2BB7F6951D5";


int get_mac_string(char *mac)
{
    struct ifreq ifr_mac;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        print_product ("socket");
        return -1;
    }
    strcpy (ifr_mac.ifr_name, "wlan0");    //Currently, only get eth0
    if (ioctl (sock, SIOCGIFHWADDR, &ifr_mac) < 0) {
        print_product ("ioctl");
        return -1;
    }    
	close(sock);
    return sprintf (mac, "%.02X:%.02X:%.02X:%.02X:%.02X:%.02X",
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[0], 
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr_mac.ifr_hwaddr.sa_data[5]);
}


int get_server_ip(char *ip)
{
    wifi_info_t wifi_info;
    wifi_get_wifi_info(&wifi_info);
    strcpy(ip, wifi_info.ip_address);
    return 0;
}

int get_dev_sn(char *sn)
{
    return get_config("ALI_AI_SN", sn, 64);
}

int get_dev_uuid(char *uuid)
{
    return get_config("ALI_AI_UUID", uuid, 64);    
}

void get_ver(char *str)
{
    strncpy(str, ver, 20);
}

void get_cid(char *str)
{
    strncpy(str, cid, 22);
}

void get_sid(char *str)
{
    strncpy(str, sid, 22);
}

void get_key(char *str)
{
    strncpy(str, key, 34);
}