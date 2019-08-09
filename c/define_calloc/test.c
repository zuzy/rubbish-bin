#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_test_t{
    char *s;
    int a;
}test_t;

void my_malloc(test_t *t)
{
    if(!t) {
        printf("t is null!");
        return;
    }
    if(t->s != NULL) {
        printf("free\n");
        free(t->s);
    }
    t->s = malloc(10);
    return;
}

#define INIT(f) do{ \
            my_malloc((f)); \
        }while(0)

int main(int argc, char *argv[])
{
    test_t f = {.s = NULL};
    // memset(&f, 0, sizeof(f));
    my_malloc(&f);
    int i;
    for(i = 0; i < 100; i++)
        INIT(&f);

    return 0;
}