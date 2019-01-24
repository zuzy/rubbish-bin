/*
 * Copyright (C) 2018 Actions
 * Author:lishiyuan
 */
 
#ifndef _AI_JSON_PARSER_H_
#define _AI_JSON_PARSER_H_

#include "common_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT_ASK_ABUFFER         (0x1 << 0)
#define BIT_ANSWER_ABUFFER         (0x1 << 1)
#define BIT_MUSIC_ABUFFER         (0x1 << 2)


int ai_json_parser(char* json_string, abuffer_t* ask_abuffer, abuffer_t* answer_abuffer, abuffer_t* music_abuffer);


#ifdef __cplusplus
}
#endif

#endif /*_AI_JSON_PARSER_H_*/



