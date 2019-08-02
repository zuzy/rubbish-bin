#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

// #define printf(...) NULL
char* simplifyPath(char* path) {
    int len = strlen(path);
    char *ret = calloc(1, len + 1);
    char *res = path;
    char *des = ret;
    while(*res) {
        if(*res == '/') {
            printf("! %s\n", res);
            if((*(res + 1) == '/' || *(res + 1) == 0)) {
                printf("find /\n");
                ++res;
                printf("---> %s\n", ret);
                continue;
            }
            if(memcmp(res, "/./", 3) == 0) {
                // printf("find /./!\n");
                res +=2;
                printf("---> %s\n", ret);
                continue;
            }
            if(strcmp(res, "/.") == 0){
                if(des == ret) {
                    *des = '/';
                }
                break;
            }
            if((memcmp(res, "/../", 4) == 0) || strcmp(res, "/..") == 0) {
                printf("find /../\n");
                res += 3;
                printf("des - 1 %s\n", des - 1);
                printf("---> %s\n", ret);
                if(*(des-1) == '/') {
                    printf("cut\n");
                    *(des - 1) = 0;
                }
                printf("---> %s\n", ret);

                des = strrchr(ret, '/');
                if(des == NULL) {
                    des = ret;
                }
                // *des = 0;
                int i = 0;
                while(des[i]) {
                    des[i] = 0;
                    ++i;
                }
                printf("---> %s\n", ret);
                continue;
            } 
        }
        // printf(": %c\n", *res);
        *des++ = *res++;
        printf("ret now is %s\n", ret);
    }
    des = 0;
    if(*ret == 0 && *path) {
        ret[0] = '/';
        ret[1] = 0;
    }
    return ret;
}

// char **des = {
//     "///TJbrd/owxdG//",
//     "",
// };

int main(int argc, char *argv[])
{
    // char *des = "/a/./b/../../c/";
    // char *des = "///TJbrd/owxdG//";
    // char *des = "/a/./b///../c/../././../d/..//../e/./f/./g/././//.//h///././/..///";
    // char *des = "//..//../e/";
    char *des = "/home/of/foo/../../bar/../../is/./here/.";
    // char *des = "/home/../../..";
    // char *des = "/..";
    char *ret = simplifyPath(des);
    printf("des %s\nret %s\n", des, ret);
    return 0;
}