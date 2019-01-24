#ifndef __LUA_COMMON_H__
#define __LUA_COMMON_H__

#define LUA_COMMON_DEBUG 1
#if LUA_COMMON_DEBUG
    #define print_lua(format, arg...)   do { printf("\033[31m[-lua_common-]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                        while (0)
#else
    #define print_lua(format, arg...)   NULL
#endif

#define HPIO_MSG        "hpio.msg"
#define HPIO_PLAYER     "hpio.player"
#define HPIO_WIFI       "hpio.wifi"
#define HPIO_CONFIG     "hpio.config"
#define HPIO_MLIST      "hpio.mlist"

#include <applib.h>
// #include <app_define.h>
#include <applib/psp_msg.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "net/wifi_manager.h"
#include "net/config.h"
#include "act_debug.h"
#include "media_navi_api.h"
#include "smart_player_api.h"
#include "smart_player_hal.h"

void hpio_start();

void hpio_stop();

int luaopen_hpio(lua_State *L);

void hpio_call(const char *filename,
                    const char *func,
                    const char *format,
                    ...);

#endif
