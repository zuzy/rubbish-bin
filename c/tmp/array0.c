#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct _st_demo
{
    uint16_t arg1;
    uint16_t arg2;
    uint8_t arg3;
    uint8_t arg4;
    uint16_t len;

    uint8_t body[];
    // uint8_t body[0];  almost same
} st_demo;

int main(int argc, char *argv[])
{
#if 1
    st_demo *demo = malloc(sizeof(st_demo) + 10);
    memset(demo, 1, sizeof(*demo));
    demo->len = 10;
    int i;
    for(i = 0; i < 10; i++) {
        demo->body[i] = i;
    }
    uint8_t *ptr = (uint8_t *)demo;
    printf("[start]\n");
    for(i = 0; i < sizeof(st_demo); i++) {
        printf("%.02X ", *(ptr+i));
    }
    printf("\n");
    for(i = 0; i < 10; i++) {
        printf("%.02X ", demo->body[i]);
    }
    printf("\n[end]\n");
    free(demo);
#else
    st_demo demo;
    memset(&demo, 1, sizeof(demo));
    demo.len = 10;
    // demo.body = malloc(10);

    int i;
    for(i = 0; i < 10; i++) {
        demo.body[i] = i;
    }
    uint8_t *ptr = (uint8_t *)&demo;
    printf("[start]\n");
    for(i = 0; i < sizeof(st_demo); i++) {
        printf("%.02X ", *(ptr+i));
    }
    printf("\n");
    for(i = 0; i < 10; i++) {
        printf("%.02X ", demo.body[i]);
    }
    printf("\n[end]\n");
    // free(demo);
#endif
    return 0;
}