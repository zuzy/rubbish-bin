#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_BUF_LEN (80)
#define MAX_PBUF_LEN (MAX_BUF_LEN+10)

#define FORMAT "%04d\t | %s"
#define FORMAT_NIL "\t | %s"

struct st_stack {
    char *buf;
    int size;
};
/**
 * @brief sllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll>80
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */


int main(int argc, char *argv[])
{
    char buf[MAX_BUF_LEN] = "";
    int index = 0;
    struct st_stack *stack = calloc(1, sizeof(struct st_stack));
    assert(stack);
    stack->buf = calloc(1, MAX_PBUF_LEN);
    assert(stack->buf);
    stack->size = MAX_PBUF_LEN;
    
    int en = 0;

    while(fgets(buf, MAX_BUF_LEN, stdin)) {
        #if 1
        if(en) {
            snprintf(stack->buf, stack->size, FORMAT_NIL, buf);
        } else {
            snprintf(stack->buf, stack->size, FORMAT, ++index, buf);
        }
        if(buf[MAX_BUF_LEN - 2]) {
            en = 1;
            strcat(stack->buf, "\n");
        } else {
            en = 0;
        }
        fwrite(stack->buf, 1, strlen(stack->buf), stdout);
        memset(stack->buf, 0, stack->size);
        #else
        if(buf[MAX_BUF_LEN - 2]) {
            stack->size += MAX_PBUF_LEN;
            stack->buf = realloc(stack->buf, stack->size);
            assert(stack->buf);
            if(*stack->buf) {
                strncat(stack->buf, buf, MAX_BUF_LEN);
            } else {
                sprintf(stack->buf, FORMAT, ++index, buf);
            }
        } else {
            if(*stack->buf) {
                strncat(stack->buf, buf, MAX_BUF_LEN);
            } else {
                snprintf(stack->buf, stack->size, FORMAT, ++index, buf);
            }
            fwrite(stack->buf, 1, strlen(stack->buf), stdout);
            memset(stack->buf, 0, stack->size);
        }
        #endif
        memset(buf, 0, sizeof(buf));
    }
    if(stack) {
        if(stack->buf) 
            free(stack->buf);
        free(stack);
    }
    // fwrite("\n", 1, 1, stdout);
    return 0;
}
