#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#include <string>
#include <chrono>

#include <unistd.h>
#include <pthread.h>


#define Z_COMMON_DEBUG 1
#if Z_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[-zizy-]\033[0m:%s:%d -> " format "\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   do {} while (0)
#endif

#define REQUIRE(in,tag)                 do{if(in){print_common("err! %s", #in); goto tag;}}while(0)

#define REQ_EXT(in)                 do{if(in){print_common("err! %s", #in); exit(1);}}while(0)

#define REQ_JSON_OBJ(des,item,tag)      cJSON_GetObjectItem(des,#item); \
                                        REQUIRE((item==NULL),tag)
#define REQ_JSON_PARSE(str,item,tag)    cJSON_Parse(str); \
                                        REQUIRE((item==NULL),tag)


inline long getMicrotime(void)
{
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e3 + currentTime.tv_usec / (int)1e3;
}


using namespace std;

struct mg_str {
    const char *p; /* Memory chunk pointer */
    size_t len;    /* Memory chunk length */
};

struct str_t {
    char *p;
    uint32_t len;

    str_t(void)
    : p(NULL)
    , len(0)
    {}

    str_t(mg_str *tar)
    : len(tar->len)
    {
        p = (char *)malloc(tar->len);
        if(p) memcpy(p, tar->p, tar->len);
    }

    str_t(const str_t & str)
    : len(str.len)
    {
        p = (char *)malloc(len);
        if(p) mempcpy(p, str.p, len);
    }

    str_t(char *des, uint32_t l)
    : len(l)
    {
        p = (char *)malloc(len);
        if(p) mempcpy(p, des, len);
    }

    ~str_t(void)
    {
        if(p) free(p);
        p = NULL;
        len = 0;
    }
};


#define MAX_QUEUE_SIZE 100
#define INIT_QUEUE_SIZE 32

class PayloadList 
{
private:
    struct Payloads {
        uint8_t     *m_buffer;
        uint32_t    m_size;
        uint32_t    m_head;
        uint32_t    m_tail;
        uint32_t    m_valid;
        uint32_t    m_remain;
        mutex       m_mutex;

        Payloads()
        : m_size(INIT_QUEUE_SIZE)
        , m_head(0)
        , m_tail(0)
        , m_valid(0)
        , m_remain(INIT_QUEUE_SIZE)
        {
            m_buffer = (uint8_t*)malloc(INIT_QUEUE_SIZE);
        }
        

        ~Payloads()
        {
            if(m_buffer) free(m_buffer);
            m_buffer = NULL;
        }

        int _append_(uint32_t length)
        {
            uint32_t size = m_size + (length << 2);
            if(size > MAX_QUEUE_SIZE) {
                size = m_size + length;
                if(size > MAX_QUEUE_SIZE) {
                    // m_mutex.unlock();
                    return 1;
                }
            }

            m_buffer = (uint8_t *)realloc(m_buffer, size);
            if(!m_buffer) {
                perror("realloc failed");
                exit(2);
            }

            m_mutex.lock();
            uint32_t extra = size - m_size;
            if(m_head) {
                if(m_head >= m_tail) {
                    uint32_t after_head = m_size - m_head;
                    memmove(m_buffer + m_head + extra, m_buffer + m_head, after_head);
                    m_head += extra;
                }
            }
            m_remain += extra;
            if(m_head == 0 && m_tail == 0) {
                m_tail = m_size;
            }
            m_size = size;
            m_mutex.unlock();
            return 0;
        }

        int push(uint8_t *data, uint32_t len)
        {
            while(m_remain < len) {
                _append_(len);
                usleep(0);
            }
            m_mutex.lock();
            uint32_t after_tail = m_size - m_tail;
            if(after_tail >= len) {
                memcpy(m_buffer + m_tail, data, len);
                if(after_tail != len) {
                    m_tail += len;
                } else {
                    m_tail = 0;
                }
            } else {
                uint32_t remain_of_tail = len - after_tail;
                memcpy(m_buffer + m_tail, data, after_tail);
                memcpy(m_buffer, data + after_tail, remain_of_tail);
                m_tail = remain_of_tail;
            }
            m_remain -= len;
            m_valid += len;
            m_mutex.unlock();
            return 0;
        }

        inline size_t size(void) 
        {
            return m_valid;
        }

        int get(char *data, uint32_t len)
        {
            m_mutex.lock();
            int ret = len < m_valid ? len : m_valid;
            len = ret;
            uint32_t after_head = m_size - m_head;
            if(after_head >= len) {
                memcpy(data, m_buffer + m_head, len);
                if(after_head != len) {
                    m_head += len;
                } else {
                    m_head = 0;
                }
            } else {
                uint32_t remain_of_head = len - after_head;
                memcpy(data, m_buffer + m_head, after_head);
                memcpy(data + after_head, m_buffer, remain_of_head);
                m_head = remain_of_head;
            }

            m_valid -= len;
            m_remain += len;
            m_mutex.unlock();
            return ret;
        }
    };

    map <string, Payloads> m_payload_list;
    
public:
    virtual ~PayloadList(void)
    {
        for(map<string, Payloads>::iterator it = m_payload_list.begin();
            it != m_payload_list.end();) {
            m_payload_list.erase(it++);
        }
    }
    
    void push_back(mg_str *payload);

    inline void push_back(mg_str *topic, mg_str *payload) 
    {
        m_payload_list[topic->p].push((uint8_t *)payload->p, payload->len);
    }

    inline void push_back(string topic, char *p, uint32_t len) 
    {
        m_payload_list[topic].push((uint8_t *)p, len);
    }

    inline int read(string topic, char *data, size_t len) 
    {
        return m_payload_list[topic].get(data, len);
    }
    int pop(string &topic, char *data, size_t len);

    inline int size(string topic) 
    {
        return m_payload_list[topic].m_valid;
    }

    void print(void);
    
    void clear(string topic);
};


void PayloadList::push_back(mg_str *payload)
{
    if(!payload || !payload->len) {
        return;
    }
    char *p = strchr((char *)payload->p, 0x0a);
    if(!p) {
        return;
    }
    string top = string(payload->p, p - payload->p);
    uint32_t len = payload->len - (p + 1 - payload->p);

    m_payload_list[top].push((uint8_t *)p, len);
}

int PayloadList::pop(string &topic, char *data, size_t len)
{
    int ret = 0;
    auto b = m_payload_list.begin(); 
    while(b != m_payload_list.end()) {
        if(b->second.m_size == 0) {
            m_payload_list.erase(b++);
        } else {
            topic = b->first;
            ret = b->second.get(data, len);
            break;
        }
    }
    return ret;
}

void PayloadList::print(void)
{
    print_common();
    for(auto &it: m_payload_list) {
        cout << "topic: " << it.first << endl;
        cout << "\t" << it.second.m_size << " : " << it.second.m_valid << endl;
    }
}

void PayloadList::clear(string topic)
{
    auto it = m_payload_list.find(topic);
    if(it != m_payload_list.end()) {
        m_payload_list.erase(it);
    }
}

#define TOPIC1 "topic1"
#define TOPIC2 "topic2"
#define TOPIC3 "topic3"
#define TOPIC4 "topic4"

#define PAYLOAD1 "payload1"
#define PAYLOAD2 "payload2"
#define PAYLOAD3 "payload3"
#define PAYLOAD4 "payload4"

#define _XX_FUN(body) {body, strlen(body)},
#define _XX_TOPIC \
    _XX_FUN(TOPIC1) \
    _XX_FUN(TOPIC2) \
    _XX_FUN(TOPIC3) \
    _XX_FUN(TOPIC4) 

#define _XX_PAYLOAD \
    _XX_FUN(PAYLOAD1) \
    _XX_FUN(PAYLOAD2) \
    _XX_FUN(PAYLOAD3) \
    _XX_FUN(PAYLOAD4) 

#define SIZE(n)     (sizeof(n) / sizeof(n[0]))

mg_str mg_topic[] = {
    _XX_TOPIC
};
mg_str mg_payloads[] = {
    _XX_PAYLOAD
};

void *read_task(void *arg)
{
    PayloadList *p = (PayloadList *)arg;
    char a[20];
    int total = 0;
    int size = sizeof(a) - 1;
    int lenOfTopic[4] = {0};
    do {
        usleep(1);
        total = 0;
        for(int i = 0; i < SIZE(mg_topic); ++i) {
            int len = 0;
            int total_len = 0;
            if(p->size(mg_topic[i].p) == 0) {
                continue;
            }
            print_common("read: %.*s", (int)mg_topic[i].len, mg_topic[i].p);
            memset(a, 0, sizeof(a));
            while((len = p->read(mg_topic[i].p, a, size)) == size) {
                total_len += len;
                for(int ai = 0; ai < sizeof(a); ++ai) {
                    printf("%.02x", a[ai]);
                }
                printf("\t%s[%d, %d]\n", a, len, total_len);
                memset(a, 0, sizeof(a));
            }
            total_len += len;
            if(len) {
                for(int ai = 0; ai < sizeof(a); ++ai) {
                    printf("%.02x", a[ai]);
                }
                printf("\t%s[%d,%d]\n", a, len, total_len);
            }
            printf("[end] %d\n", total_len);
            total += total_len;
            lenOfTopic[i] += total_len;
        }
    } while (total);
    for(int i = 0; i < 4; ++i) {
        print_common("topic%d: %d", i+1, lenOfTopic[i]);
    }
    pthread_exit(NULL);
    // exit(0);
}

int main(int argc, char *argv[])
{
    PayloadList p;


    pthread_t pid;
    pthread_create(&pid, NULL, read_task, &p);
    // pthread_detach(pid);

    uint32_t sp = SIZE(mg_payloads);
    for(int i = 0; i < SIZE(mg_topic); ++i) {
    // for(int i = 0; i < 1; ++i) {
        for(int j = 0; j < 100; ++j) {            
            p.push_back(&mg_topic[i], &mg_payloads[j % sp]);
        }
    }
    
    pthread_join(pid, NULL);
    return 0;
}
