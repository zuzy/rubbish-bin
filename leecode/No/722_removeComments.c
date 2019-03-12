#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */

char *get_str(char *input, int cmt_flag) 
{
    char *start, *end;
    start = end = input;
    char *ret = NULL;
    do {
        if(cmt_flag) {
            char *cmt_end = strstr(start, "*/");
            start += 2;
        } else {
            char *cmt_start = strstr(end, "/*");
        }
    } while(1);
    return ret;
}

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
                char *cmt_start = strstr(des, "/*");
                if(cmt_start) {
                    if(*ptr) {
                        *ptr = realloc(*ptr, cmt_start - des + 1 + strlen(*ptr));
                        
                    }
                }
            }

        }
    }
}