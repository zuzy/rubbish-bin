#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

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

int main(int argc, char *argv[])
{
    int status;
    int i = 0;
    for(; i < 100; i++)
        _network_status();
    return 0;
}