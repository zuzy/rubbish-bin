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

class PayloadList 
{
private:
    struct Payloads {

        vector<str_t*>  m_payloads;
        uint32_t        m_size;
        char            *m_cursor;
        std::mutex      m_mutex;

        Payloads(void)
        : m_size(0)
        , m_cursor(NULL)
        {}

        ~Payloads(void)
        {
            m_mutex.lock();
            m_size = 0;
            m_cursor = NULL;
            while(m_payloads.size()) {
                delete m_payloads[0];
                m_payloads.erase(m_payloads.begin());
            }
            m_mutex.unlock();
        }

        Payloads &operator+=(mg_str *msg) 
        {
            str_t *tmp = new str_t(msg);
            *this += tmp;
            return *this;
        }

        Payloads &operator+=(str_t *msg) 
        {
            m_mutex.lock();
            if(!m_cursor) m_cursor = (char *)msg->p;
            m_payloads.push_back(msg);
            m_size += msg->len;
            m_mutex.unlock();
            return *this;
        }

        int get(char *data, uint32_t len, bool force = false)
        {
            if(force) {
                if(m_size < len) len = m_size;
            }
            m_mutex.lock();
            int ret = len;
            if(m_size < len || m_size == 0) {
                len = 0;
                ret = 0;
            }
            while(len) {
                str_t *tmp = m_payloads[0];
                if(!m_cursor) {
                    if((m_size == len) && (tmp != NULL)) {
                        m_cursor = tmp->p;
                    } else {
                        exit(1);
                    }
                }
                if(tmp->len <= len) {
                    memcpy(data, m_cursor, tmp->len);
                    len -= tmp->len;
                    data += tmp->len;
                    pop();
                } else {
                    memcpy(data, m_cursor, len);
                    tmp->len -= len;
                    m_size -= len;
                    m_cursor += len;
                    break;
                }
            }
            m_mutex.unlock();
            return ret;
        }

        int pop(void)
        {
            if(m_payloads.size() <= 0) {
                return 1;
            }
            str_t *tmp = m_payloads[0];
            m_payloads.erase(m_payloads.begin());
            if(tmp) {
                m_size -= tmp->len;
                delete tmp;
            }
            if(m_payloads.size() > 0) {
                tmp = m_payloads[0];
                m_cursor = (char *)tmp->p;
            } else {
                m_cursor = NULL;
            }
    	    return 0;
        }
    };

    map <string, Payloads> m_payload_list;
    pthread_mutex_t     m_mutex;
    pthread_cond_t      m_cond;
    int                 m_to;
    
public:
    PayloadList(void)
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
    , m_cond(PTHREAD_COND_INITIALIZER)
    , m_to(-1)
    {}

    PayloadList(int timeout)
    : m_mutex(PTHREAD_MUTEX_INITIALIZER)
    , m_cond(PTHREAD_COND_INITIALIZER)
    , m_to(timeout)
    {}

    virtual ~PayloadList(void)
    {
        m_payload_list.clear();
    }
    
    void push_back(mg_str *payload);
    void push_back(mg_str *topic, mg_str *payload);
    void push_back(string topic, char *p, uint32_t len);

    int read(string topic, char *data, size_t len);
    int pop(string &topic, char *data, size_t len);

    int size(string topic);
    int size(void);

    void print(void);
    
    void clear(string topic);
    void clear(void);
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
Retry:
    str_t *realp = new str_t();
    if(!realp) {
        usleep(10);
        goto Retry;
    }
    realp->len = payload->len - (p + 1 - payload->p);
    realp->p = (char *)malloc(realp->len);
    memcpy(realp->p, p + 1, realp->len);

    pthread_mutex_lock(&m_mutex);
    m_payload_list[top] += realp;
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_broadcast(&m_cond);
}

void PayloadList::push_back(mg_str *topic, mg_str *payload)
{
    if(!topic || !payload || !topic->len || !payload->len) {
        return;
    }
    string s = string(topic->p, topic->len);
    pthread_mutex_lock(&m_mutex);
    m_payload_list[string(topic->p, topic->len)] += payload;
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_broadcast(&m_cond);
}

void PayloadList::push_back(string topic, char *p, uint32_t len)
{
    str_t *tmp = new str_t(p, len);
    pthread_mutex_lock(&m_mutex);
    m_payload_list[topic] += tmp;
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_broadcast(&m_cond);
}


int PayloadList::read(string topic, char *data, size_t len)
{
    return m_payload_list[topic].get(data, len);
}

int PayloadList::pop(string &topic, char *data, size_t len)
{
    int ret = 0;
    pthread_mutex_lock(&m_mutex);
    if(m_payload_list.size() == 0) {
        if(m_to >= 0) {
            timespec ts;
            ts.tv_sec = m_to / 1000;
            ts.tv_nsec = (m_to % 1000) * 1000000;
            ret = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
            if(ret) {
                goto End;
            }
        } else {
            ret = pthread_cond_wait(&m_cond, &m_mutex);
            if(ret) {
                goto End;
            }
        }
    }
    {
        auto b = m_payload_list.begin(); 
        while(b != m_payload_list.end()) {
            if(b->second.m_size == 0) {
                b = m_payload_list.erase(b);
            } else {
                topic = b->first;
                ret = b->second.get(data, len, true);
                break;
            }
        }
    }
End:
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

int PayloadList::size(string topic)
{
    return m_payload_list[topic].m_size;
}

int PayloadList::size(void)
{
    return m_payload_list.size();
}

void PayloadList::print(void)
{
    for(auto &it: m_payload_list) {
        cout << "topic: " << it.first << endl;
        cout << "\t" << it.second.m_size << " : " << it.second.m_payloads.size() << endl;
    }
}

void PayloadList::clear(string topic)
{
    pthread_mutex_lock(&m_mutex);
    auto it = m_payload_list.find(topic);
    if(it != m_payload_list.end()) {
        it = m_payload_list.erase(it);
    }
    pthread_mutex_unlock(&m_mutex);    
}

void PayloadList::clear(void)
{
    pthread_mutex_lock(&m_mutex);
    m_payload_list.clear();
    pthread_mutex_unlock(&m_mutex);
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
