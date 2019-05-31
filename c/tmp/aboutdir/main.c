#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <alloca.h>
#include <sys/stat.h>

#include "list.h"

char *path = "./";



int get_music_name(char *uri, char *name, int maxlen)
{
    char *ptr = uri;
    char *tail;
    ptr = strrchr(uri, '/');
    ++ptr;
    tail = strrchr(uri, '.');
    if(ptr && tail && tail-ptr) {
        strncpy(name, ptr, (tail-ptr > maxlen) ? maxlen : tail-ptr);
        // print_zizy("get name %s[end]",name);
    }
    return 0;
}

int sep_file_suffix(char *uri, char *name, int max_len, char *suffix)
{
    if(uri == NULL || strlen(uri) <= 0 || max_len <= 3) {
        return -1;
    }
    char *ptr = strrchr(uri, '.');
    if(ptr == NULL) {
        return -1;
    }
    strncpy(suffix, ptr, 6);
    if(strchr(uri, '/') != NULL) {
        get_music_name(uri, name, max_len);
    } else {
        *ptr = '\0';
        strncpy(name, uri, max_len);
    }
    return 0;
}

int sep_to_index_suffix(char *uri, char *suffix)
{
    char name[20] = "";
    if(sep_file_suffix(uri, name, 20, suffix) < 0) goto Error;
    printf("name : %s\n", name);
    char *p_s = suffix;
    while(*p_s != '\0') {
        *p_s++ = tolower(*p_s);
    }
    int num = 0;
    char *p = name;
    if(*p <= '9' && *p >= '0') {
        while(*p <= '9' && *p >= '0') {
            num *= 10;
            num += *p++ - '0';
        }
        return num;
    }
    Error:{
        return -1;
    }
}

typedef struct _st_dir
{
    struct list_head list;
    long mtime;
    char name[];
}st_dir;



int add_dir(struct list_head *head, st_dir *d)
{
    st_dir *p, *n;
    n = NULL;
    list_for_each_entry(p, head, list) {
        if(p->mtime < d->mtime) {
            break;
        } 
        n = p;
    }
    if(n == NULL) {
        list_add(&d->list, head);
    } else {
        list_add(&d->list, &n->list);
    }
    return 0;
}

void del_dir(st_dir *d)
{
    list_del(&d->list);
    free(d);
    d = NULL;
}

void clean_dir_list(struct list_head *head)
{
    st_dir *p, *t;
    list_for_each_entry_safe(p, t, head, list) {
        del_dir(p);
    }
}

void bye(void) 
{
    printf("bye\n");
}

int main(int argc, char const *argv[])
{
    atexit(bye);
    struct list_head head = LIST_HEAD_INIT(head);
    DIR *dir = opendir(path);
    char ppp[1024] = "";
    if(dir == NULL) {
        perror("open dir failed");
        return 1;
    }
    char suffix[6] = "";
    int index = 0;
    struct dirent *dent = NULL;
    struct stat *buf = (struct stat*)alloca(sizeof(struct stat));
    printf("1");
    while(NULL != (dent = readdir(dir))) {
        sprintf(ppp, "%s%s", path, dent->d_name);
        stat(ppp, buf);

        printf("%s mtime %lld\n", ppp, buf->st_mtime);
        st_dir *d = (st_dir *)malloc(sizeof(st_dir)+strlen(dent->d_name)+1);
        d->mtime = buf->st_mtime;
        strcpy(d->name, dent->d_name);
        add_dir(&head,  d);
        if(dent->d_type == DT_REG) {
            
            printf("file : %s %lld\n", dent->d_name, dent->d_off);
            // index = sep_to_index_suffix(dent->d_name, suffix);
            // printf("get index : %d, %s\n", index, suffix);
        } else {
            printf("else : %s\n", dent->d_name);
        }
    }
    st_dir *p;
    list_for_each_entry(p, &head, list){
        printf("%d %s \n",p->mtime, p->name);
    }
    clean_dir_list(&head);
    closedir(dir);
    
    // while(1) {
    //     sleep(2);
    // }
    /* code */
    return 0;
}

