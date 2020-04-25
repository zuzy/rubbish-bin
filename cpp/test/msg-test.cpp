#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <cstring>
using namespace std;

struct mg_str {
  const char *p; /* Memory chunk pointer */
  size_t len;    /* Memory chunk length */
};

struct str_t {
    char *p;
    size_t len;
    static int index;
    str_t(void)
    : p(NULL), len(0)
    {
        printf("construct %d\n", index++);
    }

    str_t(mg_str *tar)
    : len(tar->len)
    {
        printf("construct %d\n", index++);
        p = (char *)malloc(tar->len);
        memcpy(p, tar->p, tar->len);
    }

    str_t(const str_t & str)
    {
        len = str.len;
        p = (char *)malloc(len);
        mempcpy(p, str.p, len);
        printf("construct %d\n", index++);
    }

    ~str_t(void)
    {
        printf("distruct %d\n", --index);
        if(p) free(p);
    }
};
struct msg_t {
    str_t topic;
    str_t payload;
    // msg_t(void)
    // :topic(), payload()
    // {}
    msg_t(mg_str *t, mg_str *p)
    :topic(t), payload(p)
    {
        printf("msg construct!\n");
    }

    // msg_t(const msg_t &msg)
    // : topic(msg.topic)
    // , payload(msg.payload)
    // {
    //     printf("msg copy construct!\n");
    // }
};

#if 1
class Test 
{
private:
    vector<msg_t> m_sub_payloads;
    // vector<msg_t> m_pub_payloads;
public:
    void dispose(mg_str *t, mg_str *p);
    void print(void);
    ~Test(void);
};


Test::~Test(void)
{
    // while(m_sub_payloads.size() > 0) {
    //     msg_t *msg = m_sub_payloads.back();
    //     printf("del %s %s\n", msg->topic.p, msg->payload.p);
    //     m_sub_payloads.pop_back();
    //     delete msg;
    // }
    
    // for (int i = 0; i < m_sub_payloads.size(); ++i) {
    //     delete m_sub_payloads[i];
    // }
}

void Test::dispose(mg_str *t, mg_str *p)
{
    msg_t msg(t, p);
    m_sub_payloads.push_back(msg);
    // msg_t msg(t, p);
    // m_sub_payloads.push_back(msg);
}

void Test::print()
{
    for(int i = 0; i < m_sub_payloads.size(); ++i) {
        printf("%d\t%d:%s -> %d:%s\n", i, (int)m_sub_payloads[i].topic.len, m_sub_payloads[i].topic.p, (int)m_sub_payloads[i].payload.len, m_sub_payloads[i].payload.p);
    }
}
#else 
class Test 
{
private:
    vector<msg_t*> m_sub_payloads;
    // vector<msg_t> m_pub_payloads;
public:
    void dispose(mg_str *t, mg_str *p);
    void print(void);
    ~Test(void);
};


Test::~Test(void)
{
    while(m_sub_payloads.size() > 0) {
        msg_t *msg = m_sub_payloads.back();
        printf("del %s %s\n", msg->topic.p, msg->payload.p);
        m_sub_payloads.pop_back();
        delete msg;
    }
    // for (int i = 0; i < m_sub_payloads.size(); ++i) {
    //     delete m_sub_payloads[i];
    // }
}

void Test::dispose(mg_str *t, mg_str *p)
{
    msg_t *msg = new msg_t(t, p);
    m_sub_payloads.push_back(msg);
    // msg_t msg(t, p);
    // m_sub_payloads.push_back(msg);
}

void Test::print()
{
    for(int i = 0; i < m_sub_payloads.size(); ++i) {
        printf("%d\t%d:%s -> %d:%s\n", i, (int)m_sub_payloads[i]->topic.len, m_sub_payloads[i]->topic.p, (int)m_sub_payloads[i]->payload.len, m_sub_payloads[i]->payload.p);
    }
}
#endif
int str_t::index = 0;

int main(int argc, char *argv[])
{
    mg_str s = {"123456", 7};
    mg_str d = {"1234567", 8};
    Test t;
    #if 0
    t.dispose(&s, &d);
    t.dispose(&s, &d);
    #else
    for (int i = 0; i < 10; i++) {
    //     // msg_t msg(&s, &d);
        t.dispose(&s, &d);
    }
    #endif
    t.print();
    return 0;
}