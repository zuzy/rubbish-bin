#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/time.h>
#include "mqttclient.h"
#include "buffer.h"
#include "parse_pipe.h"
static int state = 0;

#define PROGRAM_VERSION "0.0.1"

struct st_mqtt_handle{
    int connection;
    int sockfd;
    union{
        int pipefd[2];
        struct {
            int refd;
            int wrfd;
        };
    };
    int timeout;
    char willmsg[64];
    char port[6];
    char *host;
    struct st_buffer buf;
};

struct st_mqtt_handle mqtt_handle = {
    0,
    -1,
    {-1, -1},
    30,
    "",
    "1883",
};

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        print_mqtt("fcntl(F_GETFL)");
    }
    if ((flags & O_NONBLOCK) == O_NONBLOCK) {
        return;
    }
    flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (flags == -1) {
        print_mqtt("fcntl(F_SETFL)");
    }
}


int tcp_connect(int cli_fd, struct sockaddr_in server_addr, int isblock)
{
    if(isblock == 0) {
        int flags = fcntl(cli_fd,F_GETFL,0);
        fcntl(cli_fd,F_SETFL,flags | O_NONBLOCK);

        int n = connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(n < 0) {

        Wait:
            REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);

            struct timeval tv;
            int err;
            socklen_t len;
            tv.tv_sec = 0;
            tv.tv_usec = 100000;
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(cli_fd,&wset);
            n = select(cli_fd+1,NULL,&wset,NULL,&tv);
            if(n < 0) {
                print_mqtt("n : %d \terror is %d", n, errno);
            }
            REQUIRE((n <= 0), Exit);
            REQUIRE(!FD_ISSET(cli_fd, &wset), Wait); 
            REQUIRE((getsockopt(cli_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0), Exit);
            REQUIRE(err != 0, Exit);
            print_mqtt("Connectd.");
        }
    } else {
        //fcntl(cli_fd,F_SETFL,flags & ~O_NONBLOCK);

        //def connect timeout 75s
        REQUIRE((connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0), Exit);
        print_mqtt("Connect OK!\n");
    }
    return cli_fd;
Exit:
    if(cli_fd >= 0) {
        close(cli_fd);
    }
    cli_fd = -1;
    return -1;
}


int tcp_cli_init(char *host, char *port)
{
    int fd = -1;
    uint16_t _port = 0;
    if(port && *port) {
        _port = atoi((const char *)port);
    } else 
        _port = 1883;

    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *rp;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    REQUIRE(getaddrinfo(host, NULL, &hints, &res) != 0, Error);
    print_mqtt("get info ok!!");
    for(rp = res; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in *s = (struct sockaddr_in *)rp->ai_addr;
        s->sin_port = htons(_port);
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(fd < 0) continue;
        // set_nonblocking(fd);
        if(tcp_connect(fd, *(struct sockaddr_in *)rp->ai_addr, 0) >= 0) {
            print_mqtt("conn ok !");
            break;
        }
        close(fd);
        fd = -1;
    }
    REQUIRE(rp == NULL, Error);
    freeaddrinfo(res);
    set_nonblocking(fd);
    return fd;
    Error:{
        if(res != NULL) {
            freeaddrinfo(res);
            res = NULL;
        }
        if(fd >= 0) {
            close(fd);
        }
        fd = -1;
        return -1;
    }
}

uint8_t get_u8(uint8_t *body, uint8_t *ptr)
{
    ptr = body;
    REQUIRE(ptr == NULL, Error);
    uint8_t ret = *ptr++;
    return ret;
    Error:
        return 0;
}

uint16_t get_u16(uint8_t *body, uint8_t *ptr)
{
    ptr = body;
    REQUIRE(body == NULL, Error);
    uint16_t ret;
    ret += *ptr++;
    ret <<= 8;
    ret += *ptr++;
    return ret;
    Error:
        return 0;
}

uint8_t *get_uflow(uint8_t *body, uint8_t *ptr, size_t len)
{
    ptr = body;
    uint8_t *flow = NULL;
    REQUIRE(body == NULL, Error);
    len = ntohs(*(uint16_t *)ptr);
    REQUIRE(len == 0, Error);
    ptr += 2;
    flow = calloc(1, len);
    REQUIRE(flow == NULL, Error);
    bcopy(ptr, flow, len);
    ptr += len;
    return flow;
    Error:
        if(flow != NULL) {
            free(flow);
            flow = NULL;
        }
        return NULL;
}

int get_fix_header(uint8_t *body, fix_head *head, uint8_t **ptr)
{
    *ptr = body;
    if(head == NULL) {
        return -1;
    }
    memset(head, 0, sizeof(*head));
    head->ctrl_type = (**ptr >> 4) & 0x0f;
    head->dup = (**ptr >> 3) & 0x01;
    head->qos = (**ptr >> 1) & 0x03;
    head->retain = **ptr & 0x01;
    ++*ptr;
    int i = 0;
    for(; i < 4; ++i, ++*ptr) {
        // head->remain_length <<= 7;
        head->remain_length += (**ptr & 0x7f) << (7 * i);
        print_mqtt("%d -> %u", i, head->remain_length);
        if(**ptr < 0x80) {
            ++*ptr;
            break;
        }
    }
    print_mqtt("type:%.X; dup:%d; qos:%d; retain:%d; length:%u", head->ctrl_type, head->dup, head->qos, head->retain, head->remain_length);
    return *ptr - body;
}

void dump_fix_head(fix_head head)
{
    print_mqtt("type: %d; dup: %d, qos: %d; retain: %d; length: %d", head.ctrl_type, head.dup, head.qos, head.retain, head.remain_length);
}

void dump_payload(payload_t p)
{
    int i;
    print_mqtt("payload:");
    printf("\t");
    for(i = 0; i < p.len; ++i) {
        printf("%.02X ", p.payload[i]);
        if( i % 5 == 4) {
            printf("\n\t");
        }
    }
    printf("\t[end]\n");
}

uint8_t *get_var_header(uint8_t *body, var_head *v_head)
{
    if(v_head == NULL) {
        v_head = (var_head *)malloc(sizeof(var_head));
    }
    uint8_t *ptr = body;
    uint16_t len = ((uint16_t)*ptr << 8) + *(ptr+1);
    ptr += 2;
    if(len > 0) {
        
    }
}

uint8_t body[1024] = "";

static const struct option long_options[]=
{
    {"willmsg",required_argument,NULL,'w'},
    {"proxy",required_argument,NULL,'p'},
    {"timeout",required_argument,NULL,'t'},
    {"help",no_argument,NULL,'?'},
    {"version",no_argument,NULL,'V'},
    {NULL,0,NULL,0}
};

static void usage(void)
{
    fprintf(stderr,
            "mqttclient -p|--proxy host[:port] [option]\n"
            "  -t|--timeout <sec>       Connection heartbeat timeout for <sec> seconds. Default 30.\n"
            "  -w|--willmsg             Will message of Connection.\n"
            "  -?|-h|--help             This information.\n"
            "  -V|--version             Display program version.\n"
           );
}

int connection()
{
    if(mqtt_handle.sockfd < 0) {
        return -1;
    }
}

uint64_t get_now_ms()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    uint64_t ret = t.tv_sec * 1000 + t.tv_usec;
    return ret;
}

void *client_proc(void *arg) 
{
    print_mqtt("client proc start");
    fd_set fds;
    struct timeval t;
    int ret, maxfd;
    static uint64_t time = 0;
    print_mqtt("%d %d", mqtt_handle.refd, mqtt_handle.sockfd);
    while(1) {
        FD_ZERO(&fds);
        FD_SET(mqtt_handle.refd, &fds);
        FD_SET(mqtt_handle.sockfd, &fds);
        // print_mqtt("get now %u %d", get_now_ms(), mqtt_handle.refd);
        t.tv_sec = 1;
        t.tv_usec = 0;
        maxfd = 0;
        maxfd = maxfd > mqtt_handle.refd? maxfd: mqtt_handle.refd;
        maxfd = maxfd > mqtt_handle.sockfd? maxfd: mqtt_handle.sockfd;
        ret = select(maxfd + 1, &fds, NULL, NULL, &t);
        if(ret < 0) {
            if(errno == EINTR) {
                continue;
            }
            break;
        } else if(ret == 0) {
            clearbuf(&mqtt_handle.buf);
        } else {
            if(FD_ISSET(mqtt_handle.sockfd, &fds)) {
                if(transform(&mqtt_handle.buf) == 0) {
                    if(checkbuf(&mqtt_handle.buf)) {
                        // parse!
                    }
                    resetbody(&mqtt_handle.buf);
                }
            } else if(FD_ISSET(mqtt_handle.refd, &fds)) {
                //publish / sub / unsub
                memset(body, 0, sizeof(body));
                int len = read(mqtt_handle.refd, body, sizeof(body));
                parse_pipe_buf(body, len);
                

            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        usage();
        return -1;
    }
    int opt=0;
    int options_index=0;
    char *tmp=NULL;
    while((opt=getopt_long(argc,argv,"Vt:p:w:?h",long_options,&options_index))!=EOF )
    {
        print_mqtt("opt %c; optarg %s, optind %d, argv[optind - 1] %s", opt, optarg, optind, argv[optind - 1]);
        switch(opt)
        {
            case  0 : break;
            case 'V': printf(PROGRAM_VERSION"\n");exit(0);
            case 't': mqtt_handle.timeout = atoi(optarg);break;	     
            case 'p': {
                /* proxy server parsing server:port */
                tmp=strrchr(optarg,':');
                mqtt_handle.host=optarg;
                if(tmp==NULL) {
                    break;
                }
                if(tmp==optarg) {
                    fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
                    return 2;
                }
                if(tmp==optarg+strlen(optarg)-1) {
                    fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
                    return 2;
                }
                *tmp='\0';
                // mqtt_handle.port=atoi(tmp+1);
                memset(mqtt_handle.port, 0, sizeof(mqtt_handle.port));
                strcpy(mqtt_handle.port, tmp+1);

                break;
            }
            case 'w': {
                memset(mqtt_handle.willmsg, 0, sizeof(mqtt_handle.willmsg));
                strncpy(mqtt_handle.willmsg, optarg, sizeof(mqtt_handle.willmsg) - 1);
                break;
            }
            case ':': 
            case 'h':
            case '?': usage();return 2;break;
        }
    }
    if(mqtt_handle.host == NULL) {
        usage();
        return 1;
    }
    pipe(mqtt_handle.pipefd);

    print_mqtt("%d %d %d %d", mqtt_handle.pipefd[0], mqtt_handle.pipefd[1], mqtt_handle.refd, mqtt_handle.wrfd);

    mqtt_handle.sockfd = tcp_cli_init(mqtt_handle.host, mqtt_handle.port);
    print_mqtt("get mqtt %d", mqtt_handle.sockfd);
    if(mqtt_handle.sockfd < 0) {
        print_mqtt("cannot connect to %s:%s", mqtt_handle.host, mqtt_handle.port);
        exit(1);
    }

    pthread_t pid;
    pthread_create(&pid, NULL, client_proc, NULL);
    pthread_detach(pid);

    char *input = malloc(1024);
    while(scanf("%s", input) != EOF) {
        printf("get input: %s\n", input);
        write(mqtt_handle.wrfd, input, strlen(input));
        usleep(200000);
    }

    return 0;
}