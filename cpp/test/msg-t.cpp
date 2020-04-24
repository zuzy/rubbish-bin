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


class Test 
{
private:
    struct msg_t {
        struct str_t {
            char *p;
            size_t len;

            str_t(mg_str *tar)
            : len(tar->len)
            {
                p = (char *)malloc(tar->len);
                memcpy(p, tar->p, tar->len);
            }

            str_t(const str_t & str)
            : len(str.len)
            {
                p = (char *)malloc(len);
                mempcpy(p, str.p, len);
            }

            ~str_t(void)
            {
                if(p) free(p);
            }
        } topic, payload;
        
        msg_t(mg_str *t, mg_str *p)
        :topic(t), payload(p)
        {
        }
    };
    vector<msg_t*> m_sub_payloads;
    // vector<msg_t> m_pub_payloads;
    void push_back(msg_t *msg);
    Test::msg_t *pop_front(void);
    Test::msg_t *pop_back(void);
public:
    void print(void);
    void dispose(mg_str *t, mg_str *p);
    ~Test(void);
};


Test::~Test(void)
{
    while(m_sub_payloads.size() > 0) {
        msg_t *msg = m_sub_payloads.back();
        m_sub_payloads.pop_back();
        delete msg;
    }
}

void Test::push_back(msg_t *msg)
{
    m_sub_payloads.push_back(msg);
}

void Test::dispose(mg_str *t, mg_str *p)
{
    msg_t *msg = new msg_t(t, p);
    push_back(msg);
}

Test::msg_t *Test::pop_back(void)
{
    msg_t *msg = m_sub_payloads.back();
    m_sub_payloads.pop_back();
    return msg;
}

Test::msg_t *Test::pop_front(void)
{
    msg_t *msg = m_sub_payloads[0];
    m_sub_payloads.erase(m_sub_payloads.begin());
    return msg;
}

void Test::print()
{
    for(int i = 0; i < m_sub_payloads.size(); ++i) {
        printf("%d\t%d:%s -> %d:%s\n", i, (int)m_sub_payloads[i]->topic.len, m_sub_payloads[i]->topic.p, (int)m_sub_payloads[i]->payload.len, m_sub_payloads[i]->payload.p);
    }
}

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