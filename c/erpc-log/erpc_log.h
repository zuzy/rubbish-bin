/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _EMBEDDED_RPC__LOG_H_
#define _EMBEDDED_RPC__LOG_H_

#include <syslog.h>

#define ERPC_DEBUG(fmt, ...)  syslog(LOG_DEBUG, fmt, ## __VA_ARGS__)
#define ERPC_NOTE(fmt, ...) syslog(LOG_NOTICE, fmt, ## __VA_ARGS__)
#define ERPC_WARN(fmt, ...) syslog(LOG_WARNING,fmt, ## __VA_ARGS__)
#define ERPC_ERROR(fmt, ...)  syslog(LOG_ERR, fmt, ## __VA_ARGS__)

enum {
    ERPC_DEBUG,
    ERPC_NOTICE,
    ERPC_WARNING,
    ERPC_ERR,
};

int init_log(const char *des);
void deinit_log(void);
void log(const char *fmt, ...);

#define erpc_log(enable, format, ...)   \
            do { \
                if(enable) \
                    log("\033[31m[-zizy-]\033[0m:%s:%d -> " format "\n",__FILE__,__LINE__, ## __VA_ARGS__); \
            } while (0)



#endif // _EMBEDDED_RPC__LOG_H_
