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
        mutex           m_mutex;

        Payloads()
        : m_size(0)
        , m_cursor(NULL)
        {}

        ~Payloads()
        {
            m_mutex.lock();
            while(m_payloads.size()) {
                str_t *tmp = m_payloads[0];
                m_payloads.erase(m_payloads.begin());
                delete tmp;
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
                print_common("msize: %u < %u", m_size, len);
                len = 0;
                ret = 0;
            }
            while(len) {
                str_t *tmp = m_payloads[0];
                if(!m_cursor) {
                    print_common("m_cursor is NULL (m: %u, l: %u)", m_size, len);
                    if((m_size == len) && (tmp != NULL)) {
                        print_common("reset to tmp!");
                        m_cursor = tmp->p;
                    } else {
                        print_common("tmp %p len: %u", tmp, tmp->len);
                        print_common("????? %u", m_size - len);
                        exit(1);
                    }
                    print_common("reset ok");
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

        int pop()
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
    mutex m_mutex;
    
public:
    virtual ~PayloadList(void)
    {
        for(map<string, Payloads>::iterator it = m_payload_list.begin();
            it != m_payload_list.end();) {
            // print_common("erase: %s", it->first.c_str());
            m_payload_list.erase(it++);
        }
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

    m_mutex.lock();
    m_payload_list[top] += realp;
    m_mutex.unlock();
}

void PayloadList::push_back(mg_str *topic, mg_str *payload)
{
    if(!topic || !payload || !topic->len || !payload->len) {
        return;
    }
    string s = string(topic->p, topic->len);
    m_mutex.lock();
    m_payload_list[string(topic->p, topic->len)] += payload;
    m_mutex.unlock();
}

void PayloadList::push_back(string topic, char *p, uint32_t len)
{
    str_t *tmp = new str_t(p, len);
    m_mutex.lock();
    m_payload_list[topic] += tmp;
    m_mutex.unlock();
}


int PayloadList::read(string topic, char *data, size_t len)
{
    return m_payload_list[topic].get(data, len);
}

int PayloadList::pop(string &topic, char *data, size_t len)
{
    int ret = 0;
    m_mutex.lock();
    auto b = m_payload_list.begin(); 
    while(b != m_payload_list.end()) {
        if(b->second.m_size == 0) {
            m_payload_list.erase(b++);
        } else {
            topic = b->first;
            ret = b->second.get(data, len, true);
            break;
        }
    }
    m_mutex.unlock();
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
    auto it = m_payload_list.find(topic);
    if(it != m_payload_list.end()) {
        m_mutex.lock();
        m_payload_list.erase(it);
        m_mutex.unlock();
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

int main(int argc, char *argv[])
{
    PayloadList p;
    mg_str mg_topic[] = {
        _XX_TOPIC
    };
    mg_str mg_payloads[] = {
        _XX_PAYLOAD
    };

    for(int i = 0; i < SIZE(mg_topic); ++i) {
        for(int j = 0; j < SIZE(mg_payloads); ++j) {
            p.push_back(&mg_topic[i], &mg_payloads[j]);
        }
    }

    p.print();

    char a[20] = "\0";
    p.read(TOPIC3, a, 19);
    print_common("read a %s", a);
    memset(a, 0, sizeof(a));
    p.read(TOPIC3, a, 10);
    print_common("read a %s", a);
    p.clear(TOPIC3);
    p.print();

    return 0;
}