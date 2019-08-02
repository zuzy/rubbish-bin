#include <stdio.h>
#include <unistd.h>

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        printf("tick\n");
        fflush(stdout);
        sleep(1);
    }
    printf("BOOM!\n");

    printf("Enter string >");
    fflush(stdout);
    char buffer[1024] = "\0";
    if(fgets(buffer, 1024, stdin) == NULL) {
        printf("some error occured\n");
        fflush(stdout);
    }
    printf("recv %s", buffer);
    
    return 0;
}
