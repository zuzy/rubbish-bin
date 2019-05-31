#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234

#define BACKLOG 1

#define exit _exit

#define MAX_RBUF_SIZE 100
struct _st_rbuf{
    char *buf;
    int in;
    int out;
    size_t size;
};

static int init_rbuf(struct _st_rbuf *rbuf, size_t len){
    printf("init ring buf\n");
    rbuf->buf = (char *)malloc(len);
    if(rbuf->buf == NULL){
        rbuf->size = 0;
        return -1;
    }
    rbuf->in = rbuf->out = 0;
    rbuf->size = len;
    return 0;
}

static int delete_rbuf(struct _st_rbuf *rbuf){
    if(rbuf->buf != NULL){
        free(rbuf->buf);
    }
    memset(rbuf, 0, sizeof(*rbuf));
    return 0;
}

static int input_rbuf(struct _st_rbuf *rbuf, char *buf, size_t len){
    char *p = buf;
    if(len > rbuf->size){
        perror("ring buff is not long enough\n");
        return -1;
    }
    printf("start to input\n");
    printf("ring buf in:%d, out:%d, size:%d\n", rbuf->in, rbuf->out, rbuf->size);

    if(len <= rbuf->size - rbuf->in){
        
        memcpy(rbuf->buf + rbuf->in, p, len);
        rbuf->in += len;
    }else{
        if(rbuf->size != rbuf->in){
            int l = rbuf->size - rbuf->in;    
            len -= l;
            memcpy(rbuf->buf + rbuf->in, p, l);
            p += l;
        }
        memcpy(rbuf->buf, p, len);
        rbuf->in = len;
    }
    return 0;
}

static  int sizeof_rbuf(struct _st_rbuf *rbuf){
    return (rbuf->in - rbuf->out) > 0 ? (rbuf->in - rbuf->out) : (rbuf->in - rbuf->out + rbuf->size);
}

static int output_rbuf(struct _st_rbuf *rbuf, char *buf, size_t len){
    if(rbuf->in == rbuf->out){
        return 0;
    }
    int l = (int)rbuf->in - rbuf->out;
    if(l > 0){
        if(l > len){
            l = len;
        }
        memcpy(buf, rbuf->buf + rbuf->out, l);
        rbuf->out += l;
    }else{
        l += rbuf->size;
        if(l > len){
            l = len;            
        }
        int m = rbuf->size - rbuf->out;
        if(l > m){
            memcpy(buf, rbuf->buf + rbuf->out, m);
            memcpy(buf + m,rbuf->buf, l - m);
            rbuf->out = l - m;
        }else{
            memcpy(buf, rbuf->buf + rbuf->out, l);
            rbuf->out += l;
        }
    }
    return l;
}


void main(){
    struct _st_rbuf rbuf;
    int listenfd, connectfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t addrlen;

    char recv_buf[1024];
    char send_buf[1024];
    init_rbuf(&rbuf, 100);

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket error");
        _exit(1);
    }

    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("bind error");
        exit(1);
    }
    
    if(listen(listenfd, BACKLOG) == -1){
        perror("listen error");
        exit(1);
    }
    
    int len = sizeof(client);
    printf("accetp \n");
    if((connectfd = accept(listenfd, (struct sockaddr*)&client, &addrlen)) == -1){
        perror("accept error");
        exit(1);
    }
    printf("you got a connection from client's ip is %s, port is %d\n",
        inet_ntoa(client.sin_addr), htons(client.sin_port));


    while(1){
        memset(recv_buf, 0, sizeof(recv_buf));
        len = recv(connectfd, recv_buf, 1024, NULL);
        recv_buf[len] = 0;
        printf("recv[%d]: %s\n", len, recv_buf);
        input_rbuf(&rbuf, recv_buf, len);
        if(sizeof_rbuf(&rbuf) > 50){
            memset(send_buf, 0, sizeof(send_buf));
            int l = output_rbuf(&rbuf, send_buf, 100);
            if(l)send(connectfd, send_buf, l, NULL);
        }
        if(len > 0){
            // send(connectfd, recv_buf, len, NULL);
        }else{
            break; 
        }
        
    }
    send(connectfd, "Welcome\n", 8, 0);
    
    close(connectfd);
    close(listenfd);
}
