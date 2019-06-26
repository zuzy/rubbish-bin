#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//#define printf(...) NULL
typedef struct {
    char *key;
    int val;
    void *next;
    void *prev;
} MapSum;

/** Initialize your data structure here. */

MapSum* mapSumCreate() {
    MapSum *obj = malloc(sizeof(MapSum));
    obj->key = NULL;
    obj->next = obj->prev = obj;
    return obj;
}

void mapSumInsert(MapSum* obj, char * key, int val) {
    MapSum *tail = obj;
    printf("insert %s %d\n", key, val);
    do{
        MapSum *tmp = obj->next;
        if( obj->key && (strcmp(obj->key, key) == 0)) {
            obj->val = val;
            return;
        }
        obj = tmp;
    } while(obj != tail);
    

    MapSum *tar = malloc(sizeof(MapSum));
    if(tar) {
        tar->key = strdup(key);
        tar->val = val;

        ((MapSum*)obj->prev)->next = tar;
        tar->next = obj;
        tar->prev = obj->prev;
        obj->prev = tar;
    }
}

int mapSumSum(MapSum* obj, char * prefix) {
    MapSum *tail = obj;
    int i = 0;
    size_t len = strlen(prefix);
    int index = 0;
    do{
        MapSum *tmp = obj->next;
        if(obj->key && (strlen(obj->key) >= len)) {
            if(memcmp(obj->key, prefix, len) == 0) {
                i += obj->val;
            }
        }
        obj = tmp;
    }while(obj != tail);
    return i;
}

void mapSumFree(MapSum* obj) {
    MapSum *tail = obj;
    do{
        MapSum *tmp = obj->next;
        if(obj->key) {
            free(obj->key);
        }
        free(obj);
        obj = tmp;
    } while(obj != tail);
}


/**
 * Your MapSum struct will be instantiated and called as such:
 * MapSum* obj = mapSumCreate();
 * mapSumInsert(obj, key, val);
 
 * int param_2 = mapSumSum(obj, prefix);
 
 * mapSumFree(obj);
*/

/*
["MapSum"
"insert"
"sum"
"insert"
"sum"
"sum"
"insert"
"sum"
"sum"
"sum"
"insert"
"sum"
"sum"
"sum"
"sum"
"sum"
"insert"
"insert"
"insert"
"sum"
"sum"
"sum"]
[[]
["aa",3]
["a"]
["aa",2]
["a"]
["aa"]
["aaa"
3]
["aaa"]
["bbb"]
["ccc"]
["aewfwaefjeoawefjwoeajfowajfoewajfoawefjeowajfowaj"
111]
["aa"]
["a"]
["b"]
["c"]
["aewfwaefjeoawefjwoeajfowajfoewajfoawefjeowajfowaj"]
["bb"
143]
["cc"
144]
["aew"
145]
["bb"]
["cc"]
["aew"]]
 */


char* genRandomString(int length)  
{  
    int flag, i;  
    char* string;  
    // srand((unsigned) time(NULL ));  
    if ((string = (char*) malloc(length)) == NULL )  
    {  
        return NULL ;  
    }  
  
    for (i = 0; i < length - 1; i++)  
    {  
        flag = rand() % 3;  
        switch (flag)  
        {  
            case 0:  
                string[i] = 'A' + rand() % 26;  
                break;  
            case 1:  
                string[i] = 'a' + rand() % 26;  
                break;  
            case 2:  
                string[i] = '0' + rand() % 10;  
                break;  
            default:  
                string[i] = 'x';  
                break;  
        }  
    }  
    string[length - 1] = '\0';  
    return string;  
}  



int main(int argc, char *argv[])
{
    MapSum *head = mapSumCreate();
    srand(time(NULL));
    int i = 0;
    for(; i < 1000; i++) {
        int num = rand() % 10;
        char *str = genRandomString(num + 2);
        mapSumInsert(head, str, num);
    }


    int r = mapSumSum(head, "a");
    printf("ret is %d\n", r);
    mapSumFree(head);
    return 0;
}