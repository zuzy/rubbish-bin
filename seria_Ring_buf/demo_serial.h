#ifndef __DEMO_SERIAL_H__
#define __DEMO_SERIAL_H__

#define uint8_t unsigned char
#define MAX_BODY_LEN    100
#define MAX_CMD_NUM     (0XFF+1)


typedef void (*rs485_cb_t)(uint8_t *, int );


typedef struct{
    uint8_t guid;
    uint8_t des_ad;
    uint8_t des_ch;
    uint8_t src_ad;
    uint8_t src_ch;
    uint8_t dev_type;
    uint8_t len;
}ser_head_t;

typedef struct{
    uint8_t cmd;
    uint8_t body[MAX_BODY_LEN];
}ser_body_t;

typedef struct{
    ser_head_t head;
    ser_body_t body;
    uint8_t check;
}ser_data_t;

typedef struct{
    int cmd;
    rs485_cb_t callback;
}ser_dis_t;

enum{
    RS485_CMD_OFF = 0,
    RS485_CMD_ON = 0X64,
    RS485_CMD_STU = 0XB5,
    RS485_CMD_CH_ADDR,
    RS485_CMD_PLAY,
    RS485_CMD_PAUSE,
    RS485_CMD_PRE,
    RS485_CMD_NEXT,
    RS485_CMD_VOL_INC,
    RS485_CMD_VOL_DEC,
    RS485_CMD_SRC,
    RS485_CMD_VOL = 0XBE,
    RS485_CMD_EFFECT = 0XC1,
    RS485_CMD_CHL,
    RS485_CMD_BLT,
    RS485_CMD_BLE,
    RS485_CMD_PIN,
    RS485_CMD_ADDR_SCAN = 0XDD,
    RS485_CMD_ADDR_REPLY = 0XDE,
    RS485_CMD_ADDR_CONFIRM = 0XDF,
    RS485_CMD_ALL_INFO = 0XE1,
    RS485_CMD_RIGHT_INFO,
    RS485_CMD_CH_ROOM,
    RS485_CMD_HALT,
    RS485_CMD_MUTE = 0XE7,
    RS485_CMD_DEMUTE,
    RS485_CMD_MODEL,
    RS485_CMD_SONG = 0XEA,
    RS485_CMD_SCREEN,
    RS485_CMD_HDMI,
    RS485_CMD_REBOOT,
    RS485_CMD_LIST_SONG = 0XEF,
    RS485_CMD_VERSION = 0XF0,
    RS485_CMD_MAX_CHL,
    RS485_CMD_FM,
    RS485_CMD_SWITCH_CHARGE,
    RS485_CMD_BATTERY,
    RS485_CMD_PAUSE_F = 0XFF,    
};



#endif