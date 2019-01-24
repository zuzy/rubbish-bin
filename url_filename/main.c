#include<stdio.h>      /*标准输入输出定义*/    
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    
#include<errno.h>      /*错误号定义*/    
#include<string.h>    

int get_music_name(char *uri, char *name){
    // print_zizy("get uri");
    char *ptr = uri;
    char *tail;
    // while(1){
    //     char *p = strstr(ptr, "\/");
    //     if(p != NULL){
    //         ptr = p+1;
    //     }else{
    //         break;
    //     }
    // }
    ptr = strrchr(uri, '/');
    ++ptr;
    tail = strrchr(uri, '.');
    if(ptr && tail && tail-ptr) {
        printf("len is %d\n", tail-ptr);
        strncpy(name, ptr, tail-ptr);
        printf("name is %s[end]\n", name);
    }
    // print_zizy("get uri now %s", ptr);
    // strcpy(tail, ptr);
    // ptr = tail;
    // ptr += strlen(tail);
    // while(*ptr != '.'){
    //     --ptr;
    //     // print_zizy("--ptr");
    // }
    // *ptr = '\0';
    // // print_zizy("tail now is %s", tail);
    // strcpy(name, tail);
    // free(tail);
    return 0;
}

void main(){
    char *str = "sdfsdf/1231131kjsdf/eirwj/北京北京12312.mp3";
    char *ss = (char*)malloc(20);
    get_music_name(str, ss);
    printf("befor:\n%s[end]\nafter:\n%s[end]\n", str, ss);
}