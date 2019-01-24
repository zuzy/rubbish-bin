#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
char** removeComments(char** source, int sourceSize, int* returnSize) {
    char **ret = (char **)calloc(sizeof(char *), sourceSize);
    char **ptr = ret;
    int i = 0;
    int cmt_flag = 0;
    for(; i < sourceSize; i++) {
        if(source[i]) {
            char *des = source[i];
            if(cmt_flag) {
                char *cmt_end = strstr(source[i], "*/");
                if(cmt_end) {
                    if(cmt_end[3]) {
                        if(*ptr) {
                            *ptr = realloc(*ptr, 1 + strlen(*ptr) + strlen(cmt_end+3));
                            strcat(*ptr, cmt_end + 3);
                        } else {
                            *ptr = strdup(cmt_end + 3);
                        }
                    }
                    
                    des = cmt_end + 3;
                    cmt_flag = 0;
                }
            }

            if(!cmt_flag) {
                
            }

        }
    }
}