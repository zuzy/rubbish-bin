#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define TEST_HOST "http://192.168.2.9:8080"
#define OFFICIAL_HOST "http://open.nbhope.cn"

#define MAC_PATH        "/hopeApi/upgrade/mac"
#define DEV_REGIST_PATH	"/hopeApi/device/register"
#define STATUS_PATH     "/hopeApi/device/status"
#define JOIN_PATH       "/hopeApi/device/editJoin"

int main(int argc, char *argv[])
{
    char mac[1024] = "";
    char dev_uri[1024] = "";
    strncpy(dev_uri, OFFICIAL_HOST DEV_REGIST_PATH, 1023);
    int i, s;
    s = sizeof(dev_uri);
    printf("uri is %s\n", dev_uri);
    printf("hex:");
    for(i = 0; i < s; i++) {
        printf(" %.02x", dev_uri[i]);
    }
    printf("\n");
    char *str = strdup(dev_uri);
    printf("get %lu %s\n", strlen(str), str);
    int len = strlen("");
    printf("strlen null is %d \n", len);
    return 0;
    
}