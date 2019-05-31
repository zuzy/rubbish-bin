#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <case_msg.h>
#include "systime_api.h"
#include "applib.h"

#include "smart_player_api.h"
#include "common.h"
#include "halo_hal.h"
#include "serial_rs485.h"
#include "rs485_dispatch.h"

static st_rs485 s_recv_body;
static uint8_t _addr;

int init_485_addr()
{
    int addr;
    if(get_config(CFG_RS485_ADDR, &addr, sizeof(addr)) == -1){
        syslog(LOG_INFO, "rs485 addr get failed\n");
        _addr = 0xff;
        addr = 0xff;
        set_config(CFG_RS485_ADDR, &addr, sizeof(addr));
        sync_config();
    }else{
        _addr = (unsigned int)addr;
        syslog(LOG_INFO, "rs addr %d\n", _addr);
    }
    syslog(LOG_INFO, "rs485 addr get %.02x\n",_addr);
    return 0;
}

int check_485_addr(uint8_t des)
{
    return _addr == des;
}

int check_addr_pro(st_proto *pro)
{
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    return rs->head.des_ad == _addr;
}

static void _switch_addr(st_rs485 *data)
{
    uint8_t addr = data->head.des_ad;
    uint8_t ch = data->head.des_ch;

    data->head.des_ad = data->head.src_ad;
    data->head.src_ad = addr;
    data->head.des_ch = data->head.src_ch;
    data->head.src_ch = ch;
}

static void _switch_addr_pro(st_proto *pro)
{
    st_rs485 *p = (st_rs485 *)pro->recv_buf.payload;
    _switch_addr(p);
}

uint8_t Crc8CheckSum(uint8_t *data, uint8_t length) 
{
    uint8_t i;     
    uint8_t Crc8=0;
    uint8_t *ptr = data;

    while(length--) {
        for(i=0x80; i; i>>=1) {
            if((Crc8&0x80)!=0) {
                Crc8<<=1;
                Crc8^=0x31;
            } else {
                Crc8<<=1;
            }

            if(*ptr&i) {
                Crc8^=0x31;
            } 
        }
        ptr++; 
    }
    return Crc8;
}

int rs485_check(st_rs485 *input)
{
    uint8_t chk = input->check;
    uint8_t cal_chk = Crc8CheckSum(input, input->head.len + sizeof(input->head));
    print_serial("check %.02x %.02x", chk, cal_chk);
    return chk != cal_chk;
}

static int _off_cb(void *arg)
{
    return 0;
}

static int _on_cb(void *arg)
{
    return 0;
}

static int _status_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    splayer_status_t status;
    splayer_get_status(&status);
    
    int is_all = 1;
    uint8_t *body_ptr = rs->body.body;
    switch(rs->body.body[0]){
        case 0:{
            memset(rs->body.body, 0, rs->head.len - 1);
            rs->head.len = 7;
            is_all = 0;
        }
        case 1:{
            if(status.status == SPLAYER_STATUS_PLAYING){
                *body_ptr++ = 1;
            }else{
                *body_ptr++ = 2;
            }
            if(is_all) break;
        }
        case 2:{
            switch(status.mode){
                case SPLAYER_MODE_PLAYLIST:{
                    int mode = 0;
                    get_config(CFG_MUSIC_LIST_MODE, &mode, sizeof(mode));
                    if(mode){
                        *body_ptr++ = 5;
                    }else{
                        *body_ptr++ = 2;
                    }
                    break;
                }
                case SPLAYER_MODE_BT:{
                    *body_ptr++ = 1;
                    break;
                }
                case SPLAYER_MODE_SAIR:
                case SPLAYER_MODE_AIMUSIC:{
                    *body_ptr++ = 6;
                    break;
                }
                default:{
                    *body_ptr++ = 0;
                    break;
                }
            }
            if(is_all) break;
        }
        case 3:{
            *body_ptr++ = (status.volume * 5) >> 1;
            if(is_all) break;
        }
        case 4:{
            switch(status.loop_mode){
                case SPLAYER_LOOP_SHUFFLE:{
                    *body_ptr++ = 1;
                    break;
                }
                case SPLAYER_LOOP_REPEAT_ALL:{
                    *body_ptr++ = 2;
                    break;
                }
                case SPLAYER_LOOP_REPEAT_ONE:{
                    *body_ptr++ = 3;
                    break;
                }
                default:{
                    *body_ptr++ = 0;
                    break;
                }
            }
            if(is_all) break;
        }
        case 5:{
            *body_ptr++ = 0;
            if(is_all) break;
        }
        case 0xF5:{
            *body_ptr++ = 0;
            if(is_all)break;
        }
        default:{
            break;
        }
    }
    pro->recv_buf.len = sizeof(st_rs485_head) + rs->head.len + 1;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _change_addr_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    if(_addr != rs->body.body[0]) {
        _addr = rs->body.body[0];
        set_config(CFG_RS485_ADDR, &_addr, sizeof(_addr));
        sync_config();
    }
    rs->head.des_ad = _addr;
    rs->body.body[0] = 0;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _play_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    splayer_status_t status;
    splayer_get_status(&status);
    if(status.mode == SPLAYER_MODE_PLAYLIST) {
        switch(status.status) {
            case SPLAYER_STATUS_PAUSE: {
                splayer_resume();
                break;
            }
            case SPLAYER_STATUS_STOP: {
                splayer_set_index(0);
                splayer_play();
                break;
            }
            case SPLAYER_STATUS_PLAYING: 
            default: {
                break;
            }
        }
    } else {
        halo_play(CTRL_PLAY);
    }

    // if(status.status == SPLAYER_STATUS_PLAYING) {
    //     rs->body.body[0] = 1;
    // } else {
    //     // 协议版本
    //     // rs->body.body[0] = 2;
    //     // 测试命令版本
    //     rs->body.body[0] = 0;
    // }
    rs->body.body[0] = 1;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _pause_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    #if 0
    if(halo_play(CTRL_PAUSE) == 0) {
        splayer_status_t status;
        splayer_get_status(&status);
        if(status.status == SPLAYER_STATUS_PLAYING) {
            rs->body.body[0] = 1;
        } else {
            rs->body.body[0] = 2;
        }
    } else {
        rs->body.body[0] = 2;
    }
    #else
    halo_play(CTRL_PAUSE);
    rs->body.body[0] = 0;
    #endif
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _pre_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    #if 0
    if(halo_cut(0) == 0) {
        splayer_status_t status;
        splayer_get_status(&status);
        if(status.status == SPLAYER_STATUS_PLAYING) {
            rs->body.body[0] = 1;
        } else {
            rs->body.body[0] = 2;
        }
    } else {
        rs->body.body[0] = 2;
    }
    #else
    halo_cut(0);
    rs->body.body[0] = 0;
    #endif
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _next_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    #if 0
    if(halo_cut(1) == 0) {
        splayer_status_t status;
        splayer_get_status(&status);
        if(status.status == SPLAYER_STATUS_PLAYING) {
            rs->body.body[0] = 1;
        } else {
            rs->body.body[0] = 2;
        }
    } else {
        rs->body.body[0] = 2;
    }
    #else
    halo_cut(1);
    rs->body.body[0] = 0;
    #endif
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _vol_inc_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    #if 0
    if(halo_vol_ctrl(1) == 0) {
        splayer_status_t status;
        splayer_get_status(&status);
        rs->body.body[0] = (status.volume * 5) >> 1;
    } else {
        rs->body.body[0] = 0;
    }
    #else
    halo_vol_ctrl(1);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = (status.volume * 5) >> 1;
    #endif
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _vol_dec_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    #if 0
    if(halo_vol_ctrl(0) == 0) {
        splayer_status_t status;
        splayer_get_status(&status);
        rs->body.body[0] = (status.volume * 5) >> 1;
    } else {
        rs->body.body[0] = 0;
    }
    #else
    halo_vol_ctrl(0);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = (status.volume * 5) >> 1;
    #endif
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _src_cb(void *arg)
{
    static uint64_t time_now = 0;
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    // if(get_now_us() - time_now < 1000000ll){
    //     print_serial("src changes too quickly!");
    //     memset(rs->body.body, 0, sizeof(rs->body.body));
    //     _switch_addr_pro(pro);
    //     rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    //     rs->body.body[rs->head.len - 1] = rs->check;
    //     return 0;
    // }
    // time_now = get_now_us();
    msg_apps_t msg;
    msg.type = MSG_MUSIC_SCENE_CHANGE;
    msg.content[0] = rs->body.body[0];
    int index = rs->body.body[1];
    memcpy(msg.content+1, &index, sizeof(index));
    broadcast_msg(&msg);

    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _vol_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    halo_vol(rs->body.body[0], 1);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = status.volume * 10 / 4;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _effect_cb(void *arg)
{
    return 0;
}

static int _halt_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    splayer_stop();
    msg_apps_t msg;
    msg.type = MSG_RS485_HALT;
    rs->body.body[0] = 0;
    broadcast_msg(&msg);
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _mute_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    halo_mute(1);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = status.volume * 10 / 4;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _demute_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    halo_mute(0);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = status.volume * 10 / 4;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _model_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    int loop = 0;
    switch (rs->body.body[0]) {
        case 1 :{
            loop = LOOP_REPEAT_SHUFFLE;
            break;
        }
        case 2: {
            // loop = LOOP_SEQUENCE;
            loop = LOOP_REPEAT_ALL;
            break;
        }
        case 3: {
            loop = LOOP_REPEAT_ONE;
            break;
        }
        default: {
            return 0;
        }
    }
    halo_loop_set(loop);
    print_serial("set loop %d", loop);
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _song_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    uint16_t index = (rs->body.body[1] << 8) + rs->body.body[0];
    hope_music_play_index(index);
    usleep(300000);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = status.playlist_index & 0xff;
    rs->body.body[1] = (status.playlist_index >> 8) & 0xff;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _screen_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    if(rs->body.body[0]) {
        systime_exit_screensaver();
    } else {
        systime_enter_screensaver();
    }
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

static int _hdmi_cb(void *arg)
{
    return 0;
}

static int _reboot_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    system("reboot");
    return 0;
}

static int _list_song_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_rs485 *rs = (st_rs485 *)pro->recv_buf.payload;
    uint16_t index = (rs->body.body[1] << 8) + rs->body.body[0];
    hope_music_play_index(index);
    splayer_status_t status;
    splayer_get_status(&status);
    rs->body.body[0] = status.playlist_index & 0xff;
    rs->body.body[1] = (status.playlist_index >> 8) & 0xff;
    _switch_addr_pro(pro);
    rs->check = Crc8CheckSum(rs, rs->head.len + sizeof(rs->head));
    rs->body.body[rs->head.len - 1] = rs->check;
    return 0;
}

struct _st_rs485_dis {
    uint8_t cmd;
    callback dis_cb;
    char name[MAX_NAME_LEN];
} rs_dis[] = {
    {RS485_CMD_OFF, _off_cb, "off"},
    {RS485_CMD_ON, _on_cb, "on"},
    {RS485_CMD_STU, _status_cb, "status"},
    {RS485_CMD_CH_ADDR, _change_addr_cb, "chgAddr"},
    {RS485_CMD_PLAY, _play_cb, "play"},
    {RS485_CMD_PAUSE, _pause_cb, "pause"},
    {RS485_CMD_PRE, _pre_cb, "pre"},
    {RS485_CMD_NEXT, _next_cb, "next"},
    {RS485_CMD_VOL_INC, _vol_inc_cb, "vol inc"},
    {RS485_CMD_VOL_DEC, _vol_dec_cb, "vol dec"},
    {RS485_CMD_SRC, _src_cb, "source"},
    {RS485_CMD_VOL, _vol_cb, "volume"},
    {RS485_CMD_EFFECT, _effect_cb, "effect"},
    {RS485_CMD_HALT, _halt_cb, "halt"},
    {RS485_CMD_MUTE, _mute_cb, "mute"},
    {RS485_CMD_DEMUTE, _demute_cb, "demute"},
    {RS485_CMD_MODEL, _model_cb, "model"},
    {RS485_CMD_SONG, _song_cb, "song"},
    {RS485_CMD_SCREEN, _screen_cb, "screen"},
    {RS485_CMD_HDMI, _hdmi_cb, "hdmi"},
    {RS485_CMD_REBOOT, _reboot_cb, "reboot"},
    {RS485_CMD_LIST_SONG, _list_song_cb, "list song"},
    {0, NULL},
};

int rs485_dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    memset(&s_recv_body, 0, sizeof(s_recv_body));
    memcpy(&s_recv_body, pro->recv_buf.payload, sizeof(s_recv_body));
    s_recv_body.check = s_recv_body.body.body[s_recv_body.head.len - 1];
    REQUIRE(rs485_check(&s_recv_body), Error);
    pro->private = &s_recv_body;
    struct _st_rs485_dis *dis_ptr = rs_dis;
    while(dis_ptr->dis_cb != NULL) {
        if(dis_ptr->cmd == s_recv_body.body.cmd) {
            print_serial("%.02X, %s trigger", dis_ptr->cmd, dis_ptr->name);
            return dis_ptr->dis_cb(pro);
        }
        ++dis_ptr;
    }
    // memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    return 0;
Error:
    NULL;
    return -1;
}

