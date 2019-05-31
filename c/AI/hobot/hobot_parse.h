/*
 * hobot_parse.h
 *
 *  Created on: 2017-10-10
 *      Author: linjintian
 */

#ifndef HOBOT_PARSE_H_
#define HOBOT_PARSE_H_


typedef enum
{
    POST_DATA_FIRST = 0,
    POST_DATA_CONTINUE,
    POST_DATA_END,
}hobot_post_tag;

typedef enum
{
    HOTOBT_SPEACH_SERVICE = 0,
    HOTBOT_TRANSLATE_SERVICE,
}hobot_service_e;

#define CFG_SAIR_TOKEN_RECORD    "sair_token_record"

typedef void (*hobot_asr_callback)(char *pText, int textLen, int errCode);
typedef void (*hobot_answer_callback)(char *pText, int textLen, char *pUrl[2], int errCode);

int hobot_post_data(short *pcm, int samples, hobot_post_tag tag, hobot_service_e mode);
int hobot_begin(char *appID, char *devID, char *lisence, hobot_asr_callback fun1, hobot_answer_callback fun2);
int hobot_end(void);
void hobot_interrupt(void);
#endif /* HOBOT_PARSE_H_ */
