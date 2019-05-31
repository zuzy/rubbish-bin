#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAXLINE 100
int main(void)
{
    char line[MAXLINE];
    FILE *fpin;
    if((fpin = popen("./myuclc", "r")) == NULL) {
        printf("popen error\n");
    }
    while(1) {
        fputs("zizy> ", stdout);
        fflush(stdout);
        if(fgets(line, MAXLINE, fpin) == NULL) {
            break;
        }
        if(fputs(line, stdout) == EOF) {
            printf("fputs error ot pipe\n");
        }
    }
    if(pclose(fpin) == -1){
        printf("pclose error\n");
    }
    putchar('\n');
    exit(0);
}