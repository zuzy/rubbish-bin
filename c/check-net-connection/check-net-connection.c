#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <assert.h>

static int _network_status()
{
    char *hostname;
	struct hostent *hostinfo;

	hostname = "baidu.com";
	hostinfo = gethostbyname (hostname);

	if (hostinfo == NULL) {
        return 0;
    }else {
        return 1;
    }
}

#define STATE_PATH_FMT "/sys/class/net/%s/operstate"
#define PATH_SIZE   128
static int _check_phy_connection(char *interface)
{
    FILE *st_file;
    int nread;
    char state_str[10] = "\0";
    char path[PATH_SIZE] = "\0";
    sprintf(path, STATE_PATH_FMT, interface);
    
    if(access(path, F_OK) == -1) {
        fprintf(stderr, "%s is not exist!\n", path);
        return 0;
    }
    if((st_file = fopen(path, "r")) == NULL) {
        fprintf(stderr, "open error\n");
        return 0;
    }
    if((nread = fread(state_str, 1, sizeof(state_str), st_file)) > 0) {
        if(memcmp(state_str, "up", 2) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int status;
    int i = 0;
    for(; i < 100; i++)
        _network_status();

    int st = _check_phy_connection("eth0");
    // int st = _check_phy_connection("enp5s0");
    printf("st is %d\n", st);
    return 0;
}