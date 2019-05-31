//串口相关的头文件    
#include<stdio.h>      /*标准输入输出定义*/    
#include<stdlib.h>     /*标准函数库定义*/    
#include<unistd.h>     /*Unix 标准函数定义*/    
#include<sys/types.h>     
#include<sys/stat.h>       
#include<fcntl.h>      /*文件控制定义*/    
#include<termios.h>    /*PPSIX 终端控制定义*/    
#include<errno.h>      /*错误号定义*/    
#include<string.h>    


char str[] = "{\"albumName\":\"狂想曲(夏日狂欢庆功限量影音版)\",\"authorName\":\"萧敬腾\",\"displayName\":\"06.怎么说我不爱你\",\"musicName\":\"06.怎么说我不爱你\",\"musicId\":20,\"musicTime\":268}]}";


static size_t _str_u8_len(char *str){
    char *ptr = str;
    unsigned char tmp = 0;
    size_t len = 0;
    int u8_len = 0;
    int num = 0;
    while(*ptr != '\0'){
        if(*ptr & 0x80){
            ++u8_len;
            #if 0
            num = 0;
            while(*ptr & 0x80){
                ++ptr;
                ++num;
            }
            printf("num is %d\n", num);
            #else
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80){
                ++num;
                tmp <<= 1;
            }
            printf("num is %d\n", num);
            ptr += num;
            #endif
        }else{
            ++ptr;
        }
        ++len;
    }
    printf("strlen is %d, u8 is %d, final is %d\n", strlen(str), u8_len, len);
    return len;
}


void main(){
    unsigned int i = 0;
    unsigned int out;
    unsigned int back;
    for(i; i <= 15; i++){
        out = (i << 3) / 3;
        back = (out * 3 + 2) >> 3;
        printf("[%d]: (%d:%d)\n", i, out, back);
    }
}