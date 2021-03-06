#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <strings.h>

ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;
    ptr = vptr;
    nleft = n;
    while(nleft > 0) {
        if((nread == read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR)
                nread = 0;
            else 
                return -1;
        } else if (nread == 0) 
            break;
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while(nleft > 0) {
        if( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR) 
                nwritten = 0;
            else 
                return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}
#if 0
// PAINFULLY SLOW!!
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for(n = 1; n < maxlen; n++) {
    Again:
        if( (rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if(c == '\n')
                break;
        } else if(rc == 0) {
            *ptr = 0;
            return n-1;
        } else {
            if (errno == EINTR)
                goto Again;
            return -1;
        }
    }
    *ptr = 0;
    return n;
}
#else
#define MAXLINE 1024
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr)
{
    if(read_cnt <= 0) {
        Again:
        if(read_cnt = read(fd, read_buf, sizeof(read_buf)) < 0) {
            if(errno == EINTR) 
                goto Again;
            return -1;
        } else if(read_cnt == 0)
            return 0;
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for(n = 1; n < maxlen; n++) {
        if((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if(c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return n - 1;
        } else 
            return -1;
    }
    *ptr = 0;
    return n;
}
#endif
int main(int argc, char *argv[])
{
    // uint32_t ip = 0x7f000001;
    struct in_addr ip;
    ip.s_addr = 0x7f000001;
    char *str = inet_ntoa((struct in_addr)ip);
    printf("ip is %s\n", str);
    ip.s_addr = 0x12345678;
    char *str2 = inet_ntoa((struct in_addr)ip);
    printf("ip is %s  %s\n", str, str2);
    // return 0;

    int ret = inet_pton(AF_INET, "127.0.0.1", &ip);
    printf("get %.X\n", htonl(ip.s_addr));

    char des[INET_ADDRSTRLEN];
    char *ss = inet_ntop(AF_INET,(const void *) &ip.s_addr, des, sizeof(des));
    printf("get %s\n", des);
    return 0;

}