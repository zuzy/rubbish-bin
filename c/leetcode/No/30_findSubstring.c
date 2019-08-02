#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
#if 0
typedef struct {
    int *loc;
    int size;
    int act_num;
}vac;

int* findSubstring(char* s, char** words, int wordsSize, int* returnSize) {
    vac *map = calloc(sizeof(vac), wordsSize);
    int i = 0;
    int length = strlen(s);
    int max = 0;
    for(; i < wordsSize; ++i) {
        map[i].size = strlen(words[i]);
        map[i].loc = malloc((length / map[i].size) >> 2);
        map[i].act_num = 0;
        char *p = s;
        int *l = map[i].loc;
        printf("start to match %s, %d\n\t", words[i], map[i].size);
        while((p = strstr(p, words[i])) != NULL) {
            *l = p - s;
            printf("%d ", *l);
            ++l;
            ++map[i].act_num;
            p += map[i].size;
        }
        printf("\n");
        if(!map[i].act_num) {
            goto null_return;
        }
        max = max > map[i].act_num? max : map[i].act_num;
    }
    int *ret = malloc(max * sizeof(int));
    *returnSize = 0;
    for(i = 0; i < wordsSize; i++) {
        int j = 0;
        for(; j < map[i].act_num; j++) {
            printf("(%d,%d->%d)\n", i, j, map[i].loc[j]);
        }
    }
null_return:
    for(i = 0; i < wordsSize; i++) {
        if(map[i].loc) {
            free(map[i].loc);
        }
    }
    free(map);
    return NULL;
}
#else

typedef struct st_loc_word{
    int index;
    int loc;
    int size;
    struct st_loc_word *prev;
    struct st_loc_word *next;
}loc_word;

int* findSubstring(char* s, char** words, int wordsSize, int* returnSize) {


    // loc_word *head = malloc(sizeof(loc_word));
    loc_word *head, *p;
    int i = 0;
    p = head = NULL;
    for(; i < wordsSize; i++){
        loc_word *ptr = malloc(sizeof(loc_word));
        char *_p = strstr(s, words[i]);
        if(_p == NULL) {
            goto null_ret;
        }
        ptr->index = i;
        ptr->loc = _p - s;
        ptr->size = strlen(words[i]);
        if(head == NULL) {
            head = ptr;
            head->next = head->prev = head;
            p = ptr;
        } else {
            p->next = ptr;
            ptr->prev = p;
            p = ptr;
            ptr->next = head;
            head->prev = ptr;
        }
    }

null_ret:
    return NULL;
}
#endif
/*
"barfoofoothefoobarman"
["foo","bar"]
*/

int main(int argc, char *argv[])
{
    char *s = "barfoofoothefoobarman";
    char *word[] = {"foo","bar"};
    int size;
    int *ret = findSubstring(s, word, 2, &size);
    return 0;
}