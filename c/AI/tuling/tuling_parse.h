/*
 * tuling_parse.h
 *
 *  Created on: 2017-12-10
 *      Author: linjintian
 */

#ifndef TULING_PARSE_H_
#define TULING_PARSE_H_


typedef enum
{
    POST_DATA_FIRST = 0,
    POST_DATA_CONTINUE,
    POST_DATA_END,
}tuling_post_tag;

typedef enum
{
    TULING_SPEACH_SERVICE = 0,
}tuling_service_e;

#define CFG_SAIR_TOKEN_RECORD    "sair_token_record"

typedef void (*tuling_asr_callback)(char *pText, int textLen, int errCode);
typedef void (*tuling_answer_callback)(char *pText, int textLen, char **url_group, int errCode);

int tuling_post_data(short *pcm, int samples, tuling_post_tag tag, tuling_service_e mode);
int tuling_begin(char *usrID, char *apiKey, char *aesKey, tuling_asr_callback fun1, tuling_answer_callback fun2);
int tuling_end(void);
void tuling_interrupt(void);
#endif
