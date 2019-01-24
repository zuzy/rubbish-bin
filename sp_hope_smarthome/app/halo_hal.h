#ifndef __HALO_HAL_H__
#define __HALO_HAL_H__

#include "common.h"
#define HALO_HAL_DEBUG 0
#if HALO_HAL_DEBUG == 1
    #define print_hal(format, arg...)   do { printf("\033[31m[halo_hal]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_hal(format, arg...)   do {} while (0)
#endif

#define LOCAL_IGN_DIR   "ign"

enum
{
    LOOP_REPEAT_SHUFFLE = 1,
    LOOP_REPEAT_ALL,
    LOOP_REPEAT_ONE,
    LOOP_SEQUENCE,
};

enum 
{
    STATUS_ALL = 0,
    STATUS_PLAY,
    STATUS_SRC,
    STATUS_VOL,
    STATUS_MODEL,
};

enum
{
    SRC_LOCALE = 0,
    SRC_BLE,
    SRC_SCENE,
    SRC_ALINK,
};

enum
{
    CTRL_PAUSE = 0,
    CTRL_PLAY,
    CTRL_STOP,
};

enum 
{
    HALO_STATUS = 0, 

    HALO_PLAY_CTRL,
    HALO_CUT_CTRL,
    HALO_VOL_CTRL,
    HALO_MUTE_CTRL,

    HALO_LOOP_SET,
    HALO_SRC_SET,
    HALO_VOL_SET,
    HALO_SEEK,
};
/** 
 * @brief  set loop mode with hope protocol
 * @note   just as splayer, index of loop enum change to hope designed
 * @param  loop: 
 * @retval 
 */
int halo_loop_set(int loop);

/** 
 * @brief  combine resume, play and set index.
 * @note   check if mode is playlist, and if status is pause. call set_mode or resume or play
 * @retval 
 */
int hope_music_play();

/** 
 * @brief  goto locale mode and set music with index to play
 * @note   check whether is playlist mode now, and set the index to play
 * @param  index: playlist index
 * @retval 
 */
int hope_music_play_index(int index);

/** 
 * @brief  seek to the target process (ms)
 * @note   just as same as splayer
 * @param  msec: des process (ms)
 * @retval 
 */
int halo_seek(int msec);

/** 
 * @brief  set volume,
 * @note   if en_persent is not 0, vol will be set as ratio of 100%;
 *          otherwise volume will be setted as default value, 0-40
 * @param  vol: 
 * @param  en_persent: 
 * @retval 
 */
int halo_vol(int vol, int en_persent);

/** 
 * @brief  silence mode or not
 * @note   silence: restore the volume and set volume to 0;
 *          no-silence: reset the volume or default volume(16) restored before, 
 * @param  ctrl: 0 -> no-silence; else -> silence
 * @retval 
 */
int halo_mute(int ctrl);

/** 
 * @brief  increase or decrease the volume
 * @note   volume +/- 3
 * @param  ctrl: 0 -> volume - 3; else -> volume + 3;
 * @retval 
 */
int halo_vol_ctrl(int ctrl);

/** 
 * @brief  prev / next song
 * @note   call the prev or next song of the list
 * @param  ctrl: 0 -> call the prev song; else -> call the next song
 * @retval 
 */
int halo_cut(int ctrl);

/** 
 * @brief  play / pause / stop the song
 * @note   just fit the hope clould protocol
 * @param  ctrl: CTRL_PAUSE/ CTRL_PLAY/ CTRL_STOP
 * @retval 
 */
int halo_play(int ctrl);

/** 
 * @brief   uncompleted,   maybe useful in the feture
 * @note   hal combine all api above
 * @param  cmd: 
 * @retval 
 */
int halo_hal(st_cmd cmd);

/** 
 * @brief  play index in local mode
 * @note   get the uri from index and set local mode uri, play only once
 * @param  index: index from playlist
 * @retval play ret
 */
int halo_local_index(int index);

/** 
 * @brief  check if is alarm mode now
 * @note   check if is alarm 
 * @retval 1-> is_alarm now; 0-> not_alarm
 */
int check_is_alarm();
#endif