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

#include "demo_serial.h"
     
//宏定义    
#define FALSE  -1    
#define TRUE   0    
#define RS485_CREATE(cmd)   {cmd, set_##cmd}
#define RS485_BUILD(cmd)    [cmd]=RS485_CREATE(cmd)
static uint8_t buf[1024];
static int get_ser_cmd(char *buf, int len, ser_data_t *data);

#define MAX_RBUF_SIZE 100
struct _st_rbuf{
    char *buf;
    int in;
    int out;
    size_t size;
};

static  struct _st_rbuf _rbuf;


void set_RS485_CMD_OFF(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_ON(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_STU(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_CH_ADDR(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_PLAY(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_PAUSE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_PRE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_NEXT(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_VOL_INC(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_VOL_DEC(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_SRC(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_VOL(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_EFFECT(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_CHL(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_BLT(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_BLE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_PIN(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_ADDR_SCAN(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_ADDR_REPLY(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_ADDR_CONFIRM(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_ALL_INFO(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_RIGHT_INFO(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_CH_ROOM(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_HALT(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_MUTE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_DEMUTE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_MODEL(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_SONG(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_SCREEN(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_HDMI(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_REBOOT(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_LIST_SONG(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_VERSION(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_MAX_CHL(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_FM(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_SWITCH_CHARGE(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_BATTERY(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}
void set_RS485_CMD_PAUSE_F(uint8_t *body, uint8_t len){
    printf("get cmd  %02x, body: " ,len);
    int i;
    for(i = 0; i < len; ++i){
        printf(" %02x", body[i]);
    }
    printf("\r\n");
}


ser_dis_t dis_tree[MAX_CMD_NUM] = {
    #if 1
    RS485_BUILD(RS485_CMD_OFF),
    RS485_BUILD(RS485_CMD_ON),
    RS485_BUILD(RS485_CMD_STU),
    RS485_BUILD(RS485_CMD_CH_ADDR),
    RS485_BUILD(RS485_CMD_PLAY),
    RS485_BUILD(RS485_CMD_PAUSE),
    RS485_BUILD(RS485_CMD_PRE),
    RS485_BUILD(RS485_CMD_NEXT),
    RS485_BUILD(RS485_CMD_VOL_INC),
    RS485_BUILD(RS485_CMD_VOL_DEC),
    RS485_BUILD(RS485_CMD_SRC),
    RS485_BUILD(RS485_CMD_VOL),
    RS485_BUILD(RS485_CMD_EFFECT),
    RS485_BUILD(RS485_CMD_CHL),
    RS485_BUILD(RS485_CMD_BLT),
    RS485_BUILD(RS485_CMD_BLE),
    RS485_BUILD(RS485_CMD_PIN),
    RS485_BUILD(RS485_CMD_ADDR_SCAN),
    RS485_BUILD(RS485_CMD_ADDR_REPLY),
    RS485_BUILD(RS485_CMD_ADDR_CONFIRM),
    RS485_BUILD(RS485_CMD_ALL_INFO),
    RS485_BUILD(RS485_CMD_RIGHT_INFO),
    RS485_BUILD(RS485_CMD_CH_ROOM),
    RS485_BUILD(RS485_CMD_HALT),
    RS485_BUILD(RS485_CMD_MUTE),
    RS485_BUILD(RS485_CMD_DEMUTE),
    RS485_BUILD(RS485_CMD_MODEL),
    RS485_BUILD(RS485_CMD_SONG),
    RS485_BUILD(RS485_CMD_SCREEN),
    RS485_BUILD(RS485_CMD_HDMI),
    RS485_BUILD(RS485_CMD_REBOOT),
    RS485_BUILD(RS485_CMD_LIST_SONG),
    RS485_BUILD(RS485_CMD_VERSION),
    RS485_BUILD(RS485_CMD_MAX_CHL),
    RS485_BUILD(RS485_CMD_FM),
    RS485_BUILD(RS485_CMD_SWITCH_CHARGE),
    RS485_BUILD(RS485_CMD_BATTERY),
    RS485_BUILD(RS485_CMD_PAUSE_F),
    #else
    [RS485_CMD_OFF] = RS485_CREATE(RS485_CMD_OFF),
    [RS485_CMD_ON] = RS485_CREATE(RS485_CMD_ON),
    [RS485_CMD_STU] = RS485_CREATE(RS485_CMD_STU),
    [RS485_CMD_CH_ADDR] = RS485_CREATE(RS485_CMD_CH_ADDR),
    [RS485_CMD_PLAY] = RS485_CREATE(RS485_CMD_PLAY),
    [RS485_CMD_PAUSE] = RS485_CREATE(RS485_CMD_PAUSE),
    [RS485_CMD_PRE] = RS485_CREATE(RS485_CMD_PRE),
    [RS485_CMD_NEXT] = RS485_CREATE(RS485_CMD_NEXT),
    [RS485_CMD_VOL_INC] = RS485_CREATE(RS485_CMD_VOL_INC),
    [RS485_CMD_VOL_DEC] = RS485_CREATE(RS485_CMD_VOL_DEC),
    [RS485_CMD_SRC] = RS485_CREATE(RS485_CMD_SRC),
    [RS485_CMD_VOL] = RS485_CREATE(RS485_CMD_VOL),
    [RS485_CMD_EFFECT] = RS485_CREATE(RS485_CMD_EFFECT),
    [RS485_CMD_CHL] = RS485_CREATE(RS485_CMD_CHL),
    [RS485_CMD_BLT] = RS485_CREATE(RS485_CMD_BLT),
    [RS485_CMD_BLE] = RS485_CREATE(RS485_CMD_BLE),
    [RS485_CMD_PIN] = RS485_CREATE(RS485_CMD_PIN),
    [RS485_CMD_ADDR_SCAN] = RS485_CREATE(RS485_CMD_ADDR_SCAN),
    [RS485_CMD_ADDR_REPLY] = RS485_CREATE(RS485_CMD_ADDR_REPLY),
    [RS485_CMD_ADDR_CONFIRM] = RS485_CREATE(RS485_CMD_ADDR_CONFIRM),
    [RS485_CMD_ALL_INFO] = RS485_CREATE(RS485_CMD_ALL_INFO),
    [RS485_CMD_RIGHT_INFO] = RS485_CREATE(RS485_CMD_RIGHT_INFO),
    [RS485_CMD_CH_ROOM] = RS485_CREATE(RS485_CMD_CH_ROOM),
    [RS485_CMD_HALT] = RS485_CREATE(RS485_CMD_HALT),
    [RS485_CMD_MUTE] = RS485_CREATE(RS485_CMD_MUTE),
    [RS485_CMD_DEMUTE] = RS485_CREATE(RS485_CMD_DEMUTE),
    [RS485_CMD_MODEL] = RS485_CREATE(RS485_CMD_MODEL),
    [RS485_CMD_SONG] = RS485_CREATE(RS485_CMD_SONG),
    [RS485_CMD_SCREEN] = RS485_CREATE(RS485_CMD_SCREEN),
    [RS485_CMD_HDMI] = RS485_CREATE(RS485_CMD_HDMI),
    [RS485_CMD_REBOOT] = RS485_CREATE(RS485_CMD_REBOOT),
    [RS485_CMD_LIST_SONG] = RS485_CREATE(RS485_CMD_LIST_SONG),
    [RS485_CMD_VERSION] = RS485_CREATE(RS485_CMD_VERSION),
    [RS485_CMD_MAX_CHL] = RS485_CREATE(RS485_CMD_MAX_CHL),
    [RS485_CMD_FM] = RS485_CREATE(RS485_CMD_FM),
    [RS485_CMD_SWITCH_CHARGE] = RS485_CREATE(RS485_CMD_SWITCH_CHARGE),
    [RS485_CMD_BATTERY] = RS485_CREATE(RS485_CMD_BATTERY),
    [RS485_CMD_PAUSE_F] = RS485_CREATE(RS485_CMD_PAUSE_F),
    #endif
};

/*******************************************************************  
* 名称：                  UART0_Open  
* 功能：                打开串口并返回串口设备文件描述  
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2)  
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int UART0_Open(int fd,char* port){
    fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
    // fd = open( port, O_RDWR|O_NOCTTY);
    // fd = open( port, O_RDWR);
    if (FALSE == fd){
        perror("Can't Open Serial Port");    
        return(FALSE);    
    }    
    //恢复串口为阻塞状态                                   
    if(fcntl(fd, F_SETFL, 0) < 0){    
        printf("fcntl failed!\n");    
        return(FALSE);    
    }         
    else{    
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));    
    }    
    //测试是否为终端设备        
    if(0 == isatty(STDIN_FILENO)){    
        printf("standard input is not a terminal device\n");    
        return(FALSE);    
    }else{    
        printf("isatty success!\n");    
    }                  
    printf("fd->open=%d\n",fd);    
    return fd;    
}    
/*******************************************************************  
* 名称：                UART0_Close  
* 功能：                关闭串口并返回串口设备文件描述  
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2)  
* 出口参数：        void  
*******************************************************************/    
     
void UART0_Close(int fd)    
{    
    close(fd);    
}    
     
/*******************************************************************  
* 名称：                UART0_Set  
* 功能：                设置串口数据位，停止位和效验位  
* 入口参数：        fd        串口文件描述符  
*                              speed     串口速度  
*                              flow_ctrl   数据流控制  
*                           databits   数据位   取值为 7 或者8  
*                           stopbits   停止位   取值为 1 或者2  
*                           parity     效验类型 取值为N,E,O,,S  
*出口参数：          正确返回为1，错误返回为0  
*******************************************************************/    
int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)    
{    
       
    int   i;    
    int   status;    
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};    
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};    
             
    struct termios options;    
       
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.  
    */    
    if( tcgetattr( fd,&options)  !=  0)    
    {    
        perror("SetupSerial 1");        
        return(FALSE);     
    }    
      
    //设置串口输入波特率和输出波特率    
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)    
    {    
        if  (speed == name_arr[i])    
        {                 
            cfsetispeed(&options, speed_arr[i]);     
            cfsetospeed(&options, speed_arr[i]);      
        }    
    }         
       
    //修改控制模式，保证程序不会占用串口    
    options.c_cflag |= CLOCAL;    
    //修改控制模式，使得能够从串口中读取输入数据    
    options.c_cflag |= CREAD;    
      
    //设置数据流控制    
    switch(flow_ctrl)    
    {    
          
        case 0 ://不使用流控制    
              options.c_cflag &= ~CRTSCTS;    
              break;       
          
        case 1 ://使用硬件流控制    
              options.c_cflag |= CRTSCTS;    
              break;    
        case 2 ://使用软件流控制    
              options.c_cflag |= IXON | IXOFF | IXANY;    
              break;    
    }    
    //设置数据位    
    //屏蔽其他标志位    
    options.c_cflag &= ~CSIZE;    
    switch (databits)    
    {      
        case 5    :    
                     options.c_cflag |= CS5;    
                     break;    
        case 6    :    
                     options.c_cflag |= CS6;    
                     break;    
        case 7    :        
                 options.c_cflag |= CS7;    
                 break;    
        case 8:        
                 options.c_cflag |= CS8;    
                 break;      
        default:       
                 fprintf(stderr,"Unsupported data size\n");    
                 return (FALSE);     
    }    
    //设置校验位    
    switch (parity)    
    {      
        case 'n':    
        case 'N': //无奇偶校验位。    
                 options.c_cflag &= ~PARENB;     
                 options.c_iflag &= ~INPCK;        
                 break;     
        case 'o':      
        case 'O'://设置为奇校验        
                 options.c_cflag |= (PARODD | PARENB);     
                 options.c_iflag |= INPCK;                 
                 break;     
        case 'e':     
        case 'E'://设置为偶校验      
                 options.c_cflag |= PARENB;           
                 options.c_cflag &= ~PARODD;           
                 options.c_iflag |= INPCK;          
                 break;    
        case 's':    
        case 'S': //设置为空格     
                 options.c_cflag &= ~PARENB;    
                 options.c_cflag &= ~CSTOPB;    
                 break;     
        default:      
                 fprintf(stderr,"Unsupported parity\n");        
                 return (FALSE);     
    }     
    // 设置停止位     
    switch (stopbits)    
    {      
        case 1:       
                 options.c_cflag &= ~CSTOPB; break;     
        case 2:       
                 options.c_cflag |= CSTOPB; break;    
        default:       
                       fprintf(stderr,"Unsupported stop bits\n");     
                       return (FALSE);    
    }    
       
    //修改输出模式，原始数据输出    
    options.c_oflag &= ~OPOST;    
      
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);    
    //options.c_lflag &= ~(ISIG | ICANON);    
       
    //设置等待时间和最小接收字符    
    options.c_cc[VTIME] = 0; /* 读取一个字符等待1*(1/10)s */      
    options.c_cc[VMIN] = 0; /* 读取字符的最少个数为1 */    
       
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读    
    tcflush(fd,TCIFLUSH);    
       
    //激活配置 (将修改后的termios数据设置到串口中）    
    if (tcsetattr(fd,TCSANOW,&options) != 0)      
    {    
        perror("com set error!\n");      
        return (FALSE);     
    }    
    return (TRUE);     
}    
/*******************************************************************  
* 名称：                UART0_Init()  
* 功能：                串口初始化  
* 入口参数：        fd       :  文件描述符     
*               speed  :  串口速度  
*                              flow_ctrl  数据流控制  
*               databits   数据位   取值为 7 或者8  
*                           stopbits   停止位   取值为 1 或者2  
*                           parity     效验类型 取值为N,E,O,,S  
*                        
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)    
{    
    int err;    
    //设置串口数据帧格式    
    if (UART0_Set(fd,speed,flow_ctrl,databits,stopbits,parity) == FALSE)    
    {                                                             
        return FALSE;    
    }    
    else    
    {    
        return  TRUE;    
    }    
}    
     
/*******************************************************************  
* 名称：                  UART0_Recv  
* 功能：                接收串口数据  
* 入口参数：        fd                  :文件描述符      
*                              rcv_buf     :接收串口中数据存入rcv_buf缓冲区中  
*                              data_len    :一帧数据的长度  
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int UART0_Recv(int fd, char *rcv_buf,int data_len)    
{    
    int len,fs_sel;    
    // fd_set fs_read;    
       
    // struct timeval time;    
       
    // FD_ZERO(&fs_read);    
    // FD_SET(fd,&fs_read);    
       
    // time.tv_sec = 10;    
    // time.tv_usec = 0;    
       
    // //使用select实现串口的多路通信    
    // fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);    
    // printf("fs_sel = %d\n",fs_sel);    
    // if(fs_sel)    
    // {    
        // len = read(fd,rcv_buf,data_len);    
        len = recv(fd, rcv_buf, data_len, NULL);
        // printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);    
        return len;    
    // }    
    // else    
    // {    
    //     printf("Sorry,I am wrong!");    
    //     return FALSE;    
    // }         
}    
/********************************************************************  
* 名称：                  UART0_Send  
* 功能：                发送数据  
* 入口参数：        fd                  :文件描述符      
*                              send_buf    :存放串口发送数据  
*                              data_len    :一帧数据的个数  
* 出口参数：        正确返回为1，错误返回为0  
*******************************************************************/    
int UART0_Send(int fd, char *send_buf,int data_len)    
{    
    int len = 0;    
       
    len = write(fd,send_buf,data_len);    
    if (len == data_len )    
    {    
        printf("send data is %s\n",send_buf);  
        return len;    
    }         
    else       
    {    
                   
        tcflush(fd,TCOFLUSH);    
        return FALSE;    
    }    
       
}    


static int init_rbuf(struct _st_rbuf *rbuf, size_t len){
    rbuf->buf = (char *)malloc(len);
    if(rbuf->buf == NULL){
        rbuf->size = 0;
        return -1;
    }
    rbuf->size = len;
    return 0;
}

static int delete_rbuf(struct _st_rbuf *rbuf){
    if(rbuf->buf != NULL){
        free(rbuf->buf);
    }
    memset(rbuf, 0, sizeof(*rbuf));
    return 0;
}

static int input_rbuf(struct _st_rbuf *rbuf, char *buf, size_t len){
    char *p = buf;
    if(len > rbuf->size){
        perror("ring buff is not long enough\n");
        return -1;
    }
    if(len <= rbuf->size - rbuf->in){
        memcpy(rbuf->buf + rbuf->in, p, len);
        rbuf->in += len;
    }else{
        if(rbuf->size != rbuf->in){
            int l = rbuf->size - rbuf->in;    
            len -= l;
            memcpy(rbuf->buf + rbuf->in, p, l);
            p += l;
        }
        memcpy(rbuf->buf, p, len);
        rbuf->in = len;
    }
    return 0;
}

static int output_rbuf(struct _st_rbuf *rbuf, char *buf, size_t len){
    if(rbuf->in == rbuf->out){
        return 0;
    }
    int l = (int)rbuf->in - rbuf->out;
    if(l > 0){
        if(l > len){
            l = len;
        }
        memcpy(buf, rbuf->buf + rbuf->out, l);
        rbuf->out += l;
    }else{
        l += rbuf->size;
        if(l > len){
            l = len;            
        }
        int m = rbuf->size - rbuf->out;
        if(l > m){
            memcpy(buf, rbuf->buf + rbuf->out, m);
            memcpy(buf + m,rbuf->buf, l - m);
            rbuf->out = l - m;
        }else{
            memcpy(buf, rbuf->buf + rbuf->out, l);
            rbuf->out += l;
        }
    }
    return l;
}

     
int main(int argc, char **argv)    
{    
    int fd;                            //文件描述符    
    int err;                           //返回调用函数的状态    
    int len;                            
    int i;    
    char rcv_buf[100];  
    char p_buf[100];
    fd_set recv_fds; 
    struct timeval t;
    int sel_ret;
    //char send_buf[20]="tiger john";    
    char send_buf[20]="tiger john"; 
    ser_data_t data; 
    struct _st_rbuf buf;
    init_rbuf(&buf, 16);
    printf("zizy hello\n");
    if(argc != 2)    
    {    
        printf("Usage: %s /dev/ttyXXX \n",argv[0]);    
        return FALSE;    
    }    
    _rbuf.buf = (char*)malloc(MAX_RBUF_SIZE);
    _rbuf.size = MAX_RBUF_SIZE;
    fd = UART0_Open(fd,argv[1]); //打开串口，返回文件描述符    
    // fd = UART0_Open(fd,"/dev/ttyUSB0"); //打开串口，返回文件描述符    
    do  
    {    
        err = UART0_Init(fd,9600,0,8,1,'N');    
        printf("Set Port Exactly!\n");    
    }while(FALSE == err || FALSE == fd);    
       
                                        
    while (1) //循环读取数据    
    {      
        FD_ZERO(&recv_fds);
        FD_SET(fd, &recv_fds);
        t.tv_sec = 2;
        t.tv_usec = 0;
        sel_ret = select(fd+1, &recv_fds, NULL, NULL, &t);

        if(sel_ret > 0){
            if(FD_ISSET(fd, &recv_fds)){
                len = read(fd, rcv_buf, 99);    
                printf("recv : %s\n", rcv_buf);
            }
            // input_rbuf(&buf, rcv_buf, len);
            // memset(p_buf, 0, sizeof(p_buf));
            // len = output_rbuf(&buf, p_buf, 16);
            // printf("hex buf :");
            // int i = 0;
            // for(i; i < len; i++){
            //     printf(" [%d: %.02x]", i, p_buf[i]);
            // }
            // printf("\n");
            // if(len > 0)    
            // {    
            //     rcv_buf[len] = '\0';    
            //     printf("receive data is %s\n",rcv_buf);    
            //     printf("len = %d\n",len);
            //     if(get_ser_cmd(rcv_buf, len, &data)){
            //         printf("recv failed\r\n");
            //     }else{
            //         dis_tree[data.body.cmd].callback(data.body.body, data.head.len);
            //     }
            //     // if(dis_tree[*rcv_buf].callback){
            //     //     dis_tree[*rcv_buf].callback(rcv_buf+1, 2);
            //     // }else{
            //     //     printf("null\r\n");
            //     // }
            // }    
            // else    
            // {    
            //     printf("cannot receive data\n");
            // }
        }else if (0 == sel_ret){
            UART0_Send(fd, send_buf, 10);
        }else{
            continue;
        }
        usleep(50);
    }
    UART0_Close(fd);
}

uint8_t Crc8CheckSum(uint8_t *ptr, uint8_t length) 
{
  uint8_t i;     
  uint8_t Crc8=0;
  
  while(length--)
  {
    for(i=0x80; i; i>>=1)
    {
      if((Crc8&0x80)!=0)
      {
        Crc8<<=1;
        Crc8^=0x31;
      }
      else
      {
        Crc8<<=1;
      }
      
      if(*ptr&i)
      {
        Crc8^=0x31;
      } 
    }
    ptr++; 
  }
  return Crc8;
}

static int get_ser_cmd(char *buf, int len, ser_data_t *data){
    char *buf_ptr = buf;
    int i = len;
    if(len < 8){
        return -1;
    }
    while(i){
        if(*buf_ptr == 0xaa){
            break;
        }
        ++buf_ptr;
        --i;
    }

    memcpy(&data->head, buf_ptr, sizeof(data->head));
    buf_ptr += sizeof(data->head);
    if(len - (buf_ptr - buf) < data->head.len){
        return -2;
    }
    memcpy(&data->body, buf_ptr, data->head.len);
    buf_ptr += data->head.len;
    data->check = Crc8CheckSum((uint8_t*)&data, data->head.len + 7);
    if(data->check == *buf_ptr){
        printf("check sum ok\r\n");
        return 0;
    }else{
        printf("checksum error: (cu:get) (%.02x: %.02x)\r\n", data->check, *buf_ptr);
        return -3;
    }
}
