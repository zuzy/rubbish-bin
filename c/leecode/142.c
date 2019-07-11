#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

#if 1
struct ListNode *detectCycle(struct ListNode *head) 
{
    struct ListNode *p, *fast;
    p = fast = head;
    if(!head) return NULL;
    do{
        p = p->next;
        if(fast->next) {
            fast = fast->next->next;
        } else {
            return NULL;
        }
    } while(fast && fast != p);
    if(fast) {
        p = head;
        while(p!=fast) {
            p = p->next;
            fast = fast->next;
        }
        return p;
    }
    return NULL;
}
#else
struct ListNode *detectCycle(struct ListNode *head) 
{
    struct _node {
        void *addr;
        struct _node *next;
        struct _node *prev;
    };
    struct ListNode *p = head;
    struct _node t = {NULL, &t, &t};
    struct ListNode *ret = NULL;
    while(p) {
        struct _node *count = t.next;
        while(count != &t) {
            if(count->addr == p) {
                ret = p;
                goto End;
            }
            count = count->next;
        }
        struct _node *_t = malloc(sizeof(struct _node));
        _t->addr = p;
        _t->next = &t;
        _t->prev = t.prev;
        t.prev->next = _t;
        t.prev = _t;
        p = p->next;
    }
    End: {
        struct _node *p, *tmp;
        p = t.next;
        while(p != &t) {
            tmp = p->next;
            free(p);
            p = tmp;
        }
        return ret;
    }
}
#endif

int main(int argc, char *argv[])
{
    return 0;
}