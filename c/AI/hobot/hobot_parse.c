/*
 * hobot_parse.c
 *
 *  Created on: 2017-10-10
 *      Author: linjintian
 */
#include "Alooper/Errors.h"
#include "Alooper/Looper.h"
#include "libc.h"
#include "ghttp/ghttp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include <json-c/json.h>
#include "hobot_parse.h"
#include "debug.h"
#include <pthread.h>



#define ENABLE_HOBOT_DEBUG   (0)
#define ENABLE_DUMP          (0)

#define OPUS_ENCODE_SAMPLE_RATE 16000
#define OPUS_ENCODE_BITRATE 20
#define OPUS_ENCODE_CHANNELS 1
#define OPUS_ENCODE_MODE 2048
#define OPUS_ENCODE_COMPLEXITY 10//5
#define OPUS_PER_FRAME_SAMPLES  320
#define HOBOT_MAX_SEND_BUF   (1024)
#define MEM_POOL_SIZE  (15*1024)
//#define ENABLE_PCM_POST

#if ENABLE_HOBOT_DEBUG
#define HOTBOT_DEBUG(format, arg...)   do { printf("[%d,%s]"format,__LINE__,__func__, ## arg);}while (0)
#else
#define HOTBOT_DEBUG(format, arg...)   do {} while (0)
#endif

typedef struct
{
    /*! �����ʣ���λhz */
    int sample_rate;
    /*! ������ */
    int channels;
    /*! ���������� */
    int bitrate;
    /*! ������ʽ��һ�����صı��������ܰ��������ʽ������IMADPCM */
    int audio_format;
    /*! һ�λ�ȡ������Ƶ��������ݲ���ʱ����0˵�������ƣ���λms */
    int chunk_time;
}enc_audio_t;

typedef struct
{
    /*! ����һ֡����������� */
    int samples_per_frame;
    /*! payload/packet��С����λ�ֽ� */
    int chunk_size;
    /*! ��ʼд���ļ���ʼ������ */
    /*! ����ָ�� */
    void *buf;
    /*! ���ݳ��� */
    int buf_len;
}audioenc_attribute_t;


/*!
 * \brief
 *      ������������������
 */
#define MAX_CHANNEL_IN     2
/*!
 * \brief
 *      ��Ƶ������֡������������Ƶ����ģ������
 */
typedef struct
{
    /*! pcm����ָ������ */
    int pcm[MAX_CHANNEL_IN];
    /*! ��ǰ�����channel�� */
    int channels;
    /*! ��ǰ�����sample�� */
    int samples;
} audioin_pcm_t;

typedef struct
{
    char extension[8];

    void *(*open)(enc_audio_t *enc_audio);
    int (*get_attribute)(void *handle, audioenc_attribute_t *attribute);
    int (*update_header)(void *handle, char **header_buf, int *header_len);
    int (*frame_encode)(void *handle, audioin_pcm_t *ain, char *output, int *bytes_used);
    void (*close)(void *handle);
}audioenc_plugin_t;

typedef struct
{
    int expire_t;
    char token[128];
}token_record_t;

typedef struct
{
    char appID[128];
    char devID[128];
    char lisence[128];
    token_record_t token;

    hobot_asr_callback asr_cb;
    hobot_answer_callback an_cb;

    void *lib_handle;
    audioenc_plugin_t *plugin_info; /* ��������Ϣ */
    void *plugin_handle;
    void *http_req;
    unsigned char *send_buf;
    int send_len;
    int flag;
    int run;
    int tag;
    int interrupt;
    int http_st;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t tid;

    hobot_service_e mode;
}hobot_inner_t;

static hobot_inner_t g_parser;


static int _post_interrupt(void *param)
{
    hobot_inner_t *h = (hobot_inner_t *)param;

    if(h && 0 == h->interrupt)
    {
        return 0;
    }
    return 1;
}

static int _get_unix_time_stamp(const char *string)
{
    struct tm lt;
    time_t t;
    char *mon[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char *week[7] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    int len = 0;
    int i = 0;
    int j = -1;
    int k = 0;
    int count = 0;
    char bDig = 0;
    char bEng = 0;
    char temp[12];

    if (!string)
    {
        return -1;
    }
    len = strlen(string);
    for (i=0; i<len;)
    {
        if (string[i] == 'G')  //����GMT
        {
            if (i+3 <= len)
            {
                i += 3;
                continue;
            }
            else
            {
                break;
            }
        }

        if (string[i] >= '0' && string[i] <= '9')
        {
            bEng = 0;
            bDig = 1;
            if (-1 == j)
            {
                j = i;
            }
        }
        else if((string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= 'a' && string[i] <= 'z'))
        {
            bDig = 0;
            bEng = 1;
            if (-1 == j)
            {
                j = i;
            }
        }

        if ((string[i] == ' ' || string[i] == ',') && j != -1)
        {
            if (bDig)
            {
                memset(temp, 0, sizeof(temp));
                memcpy(temp, &(string[j]), i-j);
                if (j > 0 && string[j-1] == ':')
                {
                    lt.tm_sec = atoi(temp);
                }
                else if (strlen(temp) >= 4)
                {
                    lt.tm_year = atoi(temp);
                }
                else
                {
                    lt.tm_mday = atoi(temp);
                }
                bDig = 0;
            }
            else if(bEng)
            {
                memset(temp, 0, sizeof(temp));
                memcpy(temp, &(string[j]), i-j);
                for (k=0; k<12; k++)
                {
                    if (0 == strcmp(temp, mon[k]))
                    {
                        lt.tm_mon = k;
                    }
                }
                if (k >= 12)
                {
                    for (k=0; k<7; k++)
                    {
                        if (0 == strcmp(temp, week[k]))
                        {
                            lt.tm_wday = k;
                        }
                    }
                }
                bEng = 0;
            }
            j = -1;
        }
        else if(string[i] == ':' && j != -1 && bDig)
        {
            bDig = 0;
            ++count;
            memset(temp, 0, sizeof(temp));
            memcpy(temp, &(string[j]), i-j);
            if (1 == count)
            {
                lt.tm_hour = atoi(temp);
            }
            else if(2 == count)
            {
                lt.tm_min = atoi(temp);
            }
            j = -1;
        }

        i++;
    }
    HOTBOT_DEBUG("get date :%d-%d-%d, %d, %d:%d:%d\n", lt.tm_year, lt.tm_mon+1, lt.tm_mday, lt.tm_wday+1, lt.tm_hour, lt.tm_min, lt.tm_sec);
    lt.tm_year -= 1900;
    t = mktime(&lt);
    HOTBOT_DEBUG("timestamp=%d\n",t);
    return (int)t;
}

int _get_gmt_timestamp(void *context)
{
    ghttp_request *req;
    ghttp_status status;
    int statusCode;
    int ret = -1;

    req = ghttp_request_new(_post_interrupt, context);
    if (req == NULL)
    {
        print_err("request new err !!\n");
        return -1;
    }
    if (ghttp_set_uri(req, "http://www.baidu.com") != 0)
    {
        print_err("set uri err !!\n");
        goto EXIT;
    }
    if (ghttp_set_type(req, ghttp_type_get) != 0)
    {
        print_err("set type err !!\n");
        goto EXIT;
    }
    ghttp_prepare(req);
    status = ghttp_connect(req, 0ll, 0ll);
    if((status != ghttp_done) && (status != ghttp_error_no_support_range))
    {
        print_err("http fail\n");
        goto EXIT;
    }
    statusCode = ghttp_status_code(req);
    if((statusCode / 100) != 2)
    {
        print_err("http fail, statusCode=%d\n", statusCode);
        goto EXIT;
    }

    const char *date_string = ghttp_get_resp_header(req, "Date");
    if (!date_string)
    {
        goto EXIT;
    }
    ret = _get_unix_time_stamp(date_string);
EXIT:
    ghttp_request_destroy(req);
    return ret;
}

static int hobot_get_token(void *h, char *get_token, int size)
{
    ghttp_request *req = 0;
    char hexsum[64];
    char string[1024];
    char result[1024];
    ghttp_status status;
    int statusCode;
    int ret = -1;
    int succes = -1;
    json_object *new_obj = NULL;
    json_object *res = NULL;
    json_object *o = NULL;
    json_tokener *tok = NULL;
    token_record_t token_t;

    memset(&token_t, 0, sizeof(token_record_t));
    int t = _get_gmt_timestamp(h);
    if (-1 == t)
    {
        print_err("get time err !!\n");
        return -1;
    }

//    if (get_config(CFG_SAIR_TOKEN_RECORD, (char *)&token_t, sizeof(token_record_t)) >= 0)
//    {
//        printf("expire_t :%d, now:%d, token:%s\n", token_t.expire_t, t, token_t.token);
//        if (t+(60*60*4) < token_t.expire_t)
//        {
//            succes = 1;
//            snprintf(get_token, size, "%s", token_t.token);
//            goto TOK_EXIT;
//        }
//    }

    memset(hexsum, 0, sizeof(hexsum));
    memset(string, 0, sizeof(string));
    //snprintf(string, sizeof(string), "%s%s%d", g_parser.appID, g_parser.lisence, t);
    snprintf(string, sizeof(string), "%s%s%s%d", g_parser.appID, g_parser.lisence, g_parser.devID, t);
    printf("md5 string:%s\n", string);
    md5_hash(string, strlen(string), hexsum);

    req = ghttp_request_new(_post_interrupt, &g_parser);
    if (req == NULL)
    {
        print_err("request new err !!\n");
        return -1;
    }
    if (ghttp_set_uri(req, "http://doraemon.horizon-robotics.com:9090/token") != 0)
    {
        print_err("set uri err !!\n");
        goto TOK_EXIT;
    }
    ghttp_set_header(req, "Accept", "*/*");
    ghttp_set_header(req, "Content-Type", "application/json");
    ghttp_set_type(req, ghttp_type_post);

    memset(string, 0, sizeof(string));
    snprintf(string, sizeof(string), "{\"appID\":\"%s\",\"deviceID\":\"%s\",\"signature\":\"%s\",\"timestamp\":%d}",
            g_parser.appID, g_parser.devID, hexsum, t);
    printf("hotbo get token body:%s\n", string);

    ghttp_set_body(req, string, strlen(string));
    ghttp_prepare(req);
    status = ghttp_connect(req, 0ll, 0ll);
    if((status != ghttp_done) && (status != ghttp_error_no_support_range))
    {
        print_err("http fail\n");
        goto TOK_EXIT;
    }
    statusCode = ghttp_status_code(req);
    if((statusCode / 100) != 2)
    {
        print_err("http fail, statusCode=%d, err:%s\n", statusCode, ghttp_get_error(req));
        goto TOK_EXIT;
    }
    ret = ghttp_read(req, result, sizeof(result)-1);
    if(ret < 0)
    {
        print_err("http get data fail, ret=%d\n", ret);
        goto TOK_EXIT;
    }
    result[ret] = 0;
    printf("hobot get token:\n%s\n", result);
    tok = json_tokener_new();
    if (tok == NULL)
    {
        print_err("tok new error\n");
        goto TOK_EXIT;
    }
    json_tokener_set_flags(tok, 0);
    new_obj = json_tokener_parse_ex(tok, result, strlen(result));
    if (new_obj == NULL)
    {
        print_err("ERROR: expected valid object\n");
        goto TOK_EXIT;
    }

    if (!json_object_object_get_ex(new_obj, "result", &res))
    {
        print_err("Fields result does not exist\n");
        goto TOK_EXIT;
    }

    if (json_object_object_get_ex(res, "token", &o) && json_object_is_type(o, json_type_string))
    {
        memset(token_t.token, 0, sizeof(token_t.token));
        snprintf(token_t.token, sizeof(token_t.token), "%s", (char *)json_object_get_string(o));
        snprintf(get_token, size, "%s", token_t.token);
        succes = 1;
    }
    if (json_object_object_get_ex(res, "expiredTime", &o) && json_object_is_type(o, json_type_int))
    {
        token_t.expire_t = json_object_get_int(o);
    }
    if (succes)
    {
        int k = set_config(CFG_SAIR_TOKEN_RECORD, (char*)&token_t, sizeof(token_record_t));
        sync_config();
    }
TOK_EXIT:
    if (succes)
    {
        memcpy(&g_parser.token, &token_t, sizeof(token_record_t));
    }
    if (new_obj)
        json_object_put(new_obj);
    if (tok)
        json_tokener_free(tok);

    if (req)
    {
        ghttp_request_destroy(req);
    }
    return succes;
}

static int _parse_result(const char *src, int len)
{
    json_object *new_obj = NULL;
    json_object *res = NULL;
    json_object *o = NULL;
    json_tokener *tok = NULL;
    int code = 0;
    char *tts = 0;
    int tts_len = 0;
    char *text = 0;
    int text_len = 0;
    char *ttsURL = 0;
    char *resURL = 0;
    char *URL[2];
    tok = json_tokener_new();
    if (tok == NULL)
    {
        print_err("tok new error\n");
        goto P_RES_EXIT;
    }
    json_tokener_set_flags(tok, 0);
    new_obj = json_tokener_parse_ex(tok, src, len);
    if (new_obj == NULL)
    {
        print_err("ERROR: expected valid object\n");
        goto P_RES_EXIT;
    }
    if (!json_object_object_get_ex(new_obj, "status", &res))
    {
        print_err("Fields status does not exist\n");
        goto P_RES_EXIT;
    }
    if (json_object_object_get_ex(res, "code", &o) && json_object_is_type(o, json_type_int))
    {
        code = json_object_get_int(o);
    }

    if (json_object_object_get_ex(new_obj, "text", &res) && json_object_is_type(res, json_type_string))
    {
        text = (char *)json_object_get_string(res);
        text_len = json_object_get_string_len(res);
    }

    if (!json_object_object_get_ex(new_obj, "result", &res))
    {
        print_err("Fields result does not exist\n");
        goto P_RES_EXIT;
    }
    if (json_object_object_get_ex(res, "tts", &o) && json_object_is_type(o, json_type_string))
    {
        tts = (char *)json_object_get_string(o);
        tts_len = json_object_get_string_len(o);
    }
    if (json_object_object_get_ex(res, "ttsURL", &o) && json_object_is_type(o, json_type_string))
    {
        ttsURL = (char *)json_object_get_string(o);
    }
    if (json_object_object_get_ex(res, "resource", &o) && json_object_is_type(o, json_type_string))
    {
        resURL = (char *)json_object_get_string(o);
    }

    if (tts && g_parser.asr_cb)
    {
        g_parser.asr_cb(tts, tts_len, code);
    }
    if (text && g_parser.an_cb)
    {
        URL[0] = ttsURL;
        URL[1] = resURL;
        g_parser.an_cb(text, text_len, URL, code);
    }
    if (4 == code)
    {
        hobot_get_token(&g_parser, g_parser.token.token, sizeof(g_parser.token.token));
    }
P_RES_EXIT:
    if (new_obj)
        json_object_put(new_obj);
    if (tok)
        json_tokener_free(tok);
    return 0;
}

static int _encode_one_frame(void *plugin_handle, audioenc_plugin_t *plugin, short *pcm, int samples, char *out, int tail)
{
    int len = 0;
    int pos = -1;
    int loop_pos = -1;
    int ret;
    int bytes_used = 0;
    int resv = 0;
    char cp_last = 0;
    static int remain_smp = 0;      //opusҪ��ÿһ֡��Ҫ�չ�320����
    static short last_buf[OPUS_PER_FRAME_SAMPLES] = {0};

    audioin_pcm_t ai;

    ai.channels = 1 ;
    ai.samples = OPUS_PER_FRAME_SAMPLES;

    int i = 0;
    int j = 0;
    int k = 0;;

    HOTBOT_DEBUG("enter _encode_one_frame,remain:%d,samples:%d\n", remain_smp, samples);
    if (tail && remain_smp > 0)
    {
        resv = OPUS_PER_FRAME_SAMPLES-remain_smp;
        memset(&(last_buf[remain_smp]), 0, resv*sizeof(short));
        ai.pcm[0] = (int)last_buf;
        ret = plugin->frame_encode(plugin_handle, &ai, &(out[len]), &bytes_used);
        len += bytes_used;
        HOTBOT_DEBUG("******* encode the tail data len:%d\n", len);
        remain_smp = 0;
        return len;
    }

    if (samples < OPUS_PER_FRAME_SAMPLES)
    {
        if (remain_smp)
        {
            pos = remain_smp;
            remain_smp += samples;
        }
        else
        {
            pos = 0;
            remain_smp = samples;
        }

        if (remain_smp > OPUS_PER_FRAME_SAMPLES)
        {
            resv = OPUS_PER_FRAME_SAMPLES-pos;
            memcpy((char*)&(last_buf[pos]), pcm, resv*sizeof(short));
            remain_smp = remain_smp-OPUS_PER_FRAME_SAMPLES;
            pos = resv;
            cp_last = 1;
        }
        else if(OPUS_PER_FRAME_SAMPLES == remain_smp)
        {
            memcpy((char*)&(last_buf[pos]), pcm, samples*sizeof(short));
            cp_last = 1;
            remain_smp = 0;
        }
        else
        {
            memcpy((char*)&(last_buf[pos]), pcm, samples*sizeof(short));
        }
    }
    else
    {
        if (remain_smp <= 0)
        {
            k = samples/OPUS_PER_FRAME_SAMPLES;
            j = samples% OPUS_PER_FRAME_SAMPLES;
            loop_pos = 0;
	pos = 0;
        }
        else
        {
            pos = remain_smp;
            resv = OPUS_PER_FRAME_SAMPLES-pos;
            memcpy((char*)&(last_buf[pos]), pcm, resv*sizeof(short));
            loop_pos = resv;
            resv = samples-resv;
            k = resv/OPUS_PER_FRAME_SAMPLES;
            j = resv% OPUS_PER_FRAME_SAMPLES;
            cp_last = 1;
        }
    }

    if (cp_last)
    {
        ai.pcm[0] = (int)last_buf;
        ret = plugin->frame_encode(plugin_handle, &ai, &(out[len]), &bytes_used);
        HOTBOT_DEBUG("++++encode last remain ret:%d,len%d,bytes_used:%d\n", ret,len,bytes_used);
        len += bytes_used;
        pos = 0;
    }

    if (loop_pos >= 0)
    {
        for(i=0; i<k; i++)
        {
            ai.pcm[0] = (int)&(pcm[loop_pos + i*OPUS_PER_FRAME_SAMPLES]);
            ret = plugin->frame_encode(plugin_handle, &ai, &(out[len]), &bytes_used);
            //HOTBOT_DEBUG("====encode frame ret:%d,i:%d,len%d,bytes_used:%d\n", ret,i,len,bytes_used);
            len += bytes_used;
        }
        if (j > 0)
        {
            remain_smp = j;
            memcpy((char*)&(last_buf[pos]), (char*)&(pcm[loop_pos+k*OPUS_PER_FRAME_SAMPLES]), remain_smp*sizeof(short));
            HOTBOT_DEBUG("****** remain pos:%d, byt:%d, k:%d\n", pos, remain_smp, k);
        }
        else
        {
            remain_smp = 0;
        }
    }
    else if (cp_last && remain_smp > 0 && pos > 0)
    {
        printf("!!!!!!!_encode_one_frame special cp last pos:%d\n", pos);
        memcpy((char*)last_buf, (char*)&(pcm[pos]), remain_smp*sizeof(short));
    }
    return len;
}

static ghttp_request *_open_connection(void *context, int service_mode)
{
    ghttp_request *req = 0;
    ghttp_status status = ghttp_error;
    char sessionID[128] = {0};

    req = ghttp_request_new(_post_interrupt, context);
    if (req == NULL)
    {
        print_err("request new err !!\n");
        return NULL;
    }
    if (ghttp_set_uri(req, "http://doraemon.horizon-robotics.com:9090/toy") != 0)
    {
        print_err("set uri err !!\n");
        goto err_connect;
    }
    ghttp_set_header(req, "Content-Type", "audio/opus;bit=16;rate=16000");
    ghttp_set_header(req, "Transfer-Encoding", "chunked");
    ghttp_set_header(req, "Connection", "Keep-Alive");

    struct timeval now;
    gettimeofday(&now, NULL);
    snprintf(sessionID, sizeof(sessionID), "SID-5e4a81a0-945d-11e7-a7c5-a%db", (int)now.tv_sec);
    ghttp_set_header(req, "appID", g_parser.appID);
    ghttp_set_header(req, "deviceID", g_parser.devID);
    //ghttp_set_header(req, "sessionID", sessionID);
    ghttp_set_header(req, "token", g_parser.token.token);
    if (HOTBOT_TRANSLATE_SERVICE == service_mode)
    {
        ghttp_set_header(req, "Service-Mode", "2");
    }
    ghttp_set_chunked_mode(req);

    ghttp_set_type(req, ghttp_type_post);
    ghttp_prepare(req);

    status = ghttp_connect(req, 0ll, 0ll);
    printf("ghttp_connect ret:%d, req:0x%x\n", status, req);

    if((status != ghttp_done) && (status != ghttp_error_no_support_range))
    {
        print_err("http fail\n");
        goto err_connect;
    }
    return req;

err_connect:
    if (req)
    {
        ghttp_request_destroy(req);
    }
    return NULL;
}


static void _wait_write_buf(hobot_inner_t *handle, char *in, int bytes, int tag)
{
    if (!handle->flag)
    {
        return ;
    }

    pthread_mutex_lock(&handle->mutex);
    while(handle->send_len+bytes > MEM_POOL_SIZE)
    {
        HOTBOT_DEBUG("write wait send len:%d\n", handle->send_len);
        pthread_cond_wait(&handle->cond, &handle->mutex);
    }
    if (handle->flag)
    {
		memcpy(&(handle->send_buf[handle->send_len]), in, bytes);
		handle->send_len += bytes;
		handle->tag = tag;
    }
    pthread_cond_signal(&handle->cond);
    pthread_mutex_unlock(&handle->mutex);
}

void hobot_interrupt(void)
{
    if (!g_parser.interrupt)
    {
        g_parser.interrupt = 1;//�ϴεĻ�û����
        pthread_mutex_lock(&g_parser.mutex);
        pthread_cond_broadcast(&g_parser.cond);
        pthread_mutex_unlock(&g_parser.mutex);
    }
}

int hobot_post_data(short *pcm, int samples, hobot_post_tag tag, hobot_service_e mode)
{
    int ret;
    int bytes_used = 0;
#if ENABLE_DUMP
    static int fd1 = -1;
    static int fd2 = -1;
    static int fd_c = 1;
#endif
    char write_buf[4*1024];

    if (tag == POST_DATA_FIRST)
    {
        while(g_parser.flag)
        {
            if (!g_parser.interrupt)
            {
                g_parser.interrupt = 1;//�ϴεĻ�û����
                pthread_mutex_lock(&g_parser.mutex);
                pthread_cond_broadcast(&g_parser.cond);
                pthread_mutex_unlock(&g_parser.mutex);
            }
            usleep(10);
        }
        g_parser.mode = mode;
        g_parser.flag = 1;
        g_parser.interrupt = 0;
#if ENABLE_DUMP
        char path[128];
        snprintf(path, sizeof(path), "/mnt/card/test_%d.pcm", fd_c);
        if (-1 == fd1)
            fd1 = open(path, O_RDWR | O_CREAT, 777);
        snprintf(path, sizeof(path), "/mnt/card/test_%d.opus", fd_c);
        if (-1 == fd2)
            fd2 = open(path, O_RDWR | O_CREAT, 777);
        fd_c++;
        if (fd1 >= 0)
            write(fd1, (char*)pcm, samples*sizeof(short));
#endif
        bytes_used = _encode_one_frame(g_parser.plugin_handle, g_parser.plugin_info, pcm, samples, write_buf, 0);
        HOTBOT_DEBUG("+++++hobot_post_data frame encode bytes:%d, samples:%d\n", bytes_used, samples);
        if (bytes_used > 0)
        {
#if ENABLE_DUMP
            if (fd2 >= 0)
                write(fd2, write_buf, bytes_used);
#endif
            _wait_write_buf(&g_parser, write_buf, bytes_used, tag);
        }

    }
    else if(tag == POST_DATA_CONTINUE)
    {
#if ENABLE_DUMP
        if (fd1 >= 0)
            write(fd1, (char*)pcm, samples*sizeof(short));
#endif
        bytes_used = _encode_one_frame(g_parser.plugin_handle, g_parser.plugin_info, pcm, samples, write_buf, 0);
        HOTBOT_DEBUG("-----hobot_post_data frame encode bytes:%d, samples:%d\n", bytes_used, samples);
        if (bytes_used > 0)
        {
#if ENABLE_DUMP
            if (fd2 >= 0)
                write(fd2, write_buf, bytes_used);
#endif
            _wait_write_buf(&g_parser, write_buf, bytes_used, tag);
        }
    }
    else
    {
        bytes_used = _encode_one_frame(g_parser.plugin_handle, g_parser.plugin_info, NULL, 0, write_buf, 1);
        if (bytes_used > 0)
        {
#if ENABLE_DUMP
            if (fd2 >= 0)
                write(fd2, write_buf, bytes_used);
#endif
        }
        _wait_write_buf(&g_parser, write_buf, bytes_used, tag);
#if ENABLE_DUMP
        if (fd1 >= 0)
            close(fd1);
        fd1 = -1;
        if (fd2 >= 0)
            close(fd2);
        fd2 = -1;
#endif
    }
    return 0;
}

static void _close_encoder(hobot_inner_t *h)
{
    if (h->plugin_info)
    {
        if (h->plugin_handle)
        {
            h->plugin_info->close(h->plugin_handle);
            h->plugin_handle = NULL;
        }

        h->plugin_info = NULL;
    }
    if (h->lib_handle)
    {
        dlclose(h->lib_handle);
        h->lib_handle = NULL;
    }
}

static int _open_encoder(hobot_inner_t *h)
{
    enc_audio_t enc_audio;
    void *(* func_handle)(void);
    audioenc_attribute_t attribute;
    int ret;

    h->lib_handle = dlopen("aeOPUS.so", RTLD_LAZY);
    if (h->lib_handle == NULL)
    {
        print_err("error: %s: %s\n", __FILE__, dlerror());
        goto begin_err;
    }
    func_handle = dlsym(h->lib_handle, "get_plugin_info");
    if (func_handle == NULL)
    {
        goto begin_err;
    }
    h->plugin_info = (audioenc_plugin_t *)func_handle();
    if (h->plugin_info == NULL)
    {
        goto begin_err;
    }
    enc_audio.sample_rate = OPUS_ENCODE_SAMPLE_RATE;
    enc_audio.bitrate = OPUS_ENCODE_BITRATE;
    enc_audio.channels = 1;
    enc_audio.audio_format = OPUS_ENCODE_MODE;
    enc_audio.chunk_time = OPUS_ENCODE_COMPLEXITY;

    h->plugin_handle = h->plugin_info->open(&enc_audio);
    if (h->plugin_handle == NULL)
    {
        printf("%s: open plugin error\n", __FILE__);
        goto begin_err;
    }
    ret = h->plugin_info->get_attribute(h->plugin_handle, &attribute);
    if (ret < 0)
    {
        printf("%s: unsupport\n", __FILE__);
        goto begin_err;
    }
    printf("=========hobot finish open encoder, att :%d,%d,%x\n", attribute.samples_per_frame, attribute.chunk_size, g_parser.plugin_handle);
    return 0;
begin_err:
    _close_encoder(h);
    return -1;
}


static void *buf_send_thread(void *handle)
{
    hobot_inner_t *obj = (hobot_inner_t*)handle;
    ghttp_status status = ghttp_error;

    while(obj->run)
    {
        if (!obj->flag)
        {
            usleep(5);
            continue;
        }
        if (obj->tag == POST_DATA_FIRST)
        {
            if (obj->mode == HOTBOT_TRANSLATE_SERVICE && obj->http_req)
            {
                HOTBOT_DEBUG("transle need reconnect\n");
                ghttp_request_destroy(obj->http_req);
                obj->http_req = 0;
            }
            if (!obj->http_req)
            {
                obj->http_req = _open_connection(obj, obj->mode);
            }
            if (!obj->http_req)
            {
                if (g_parser.an_cb)
                	g_parser.an_cb(0, 0, 0, 1);
                obj->http_st = ghttp_error;
                obj->flag = 0;
                pthread_mutex_lock(&obj->mutex);
                obj->send_len = 0;
                pthread_cond_signal(&obj->cond);
                pthread_mutex_unlock(&obj->mutex);
                continue;
            }
        }

        pthread_mutex_lock(&obj->mutex);
        status = ghttp_error;
        if (!obj->http_req)
        {
            HOTBOT_DEBUG("http req has destory in post, tag:%d\n", obj->tag);
            pthread_mutex_unlock(&obj->mutex);
            continue;
        }
        if (!obj->run)
        {
            goto exit_post_data;
        }
        while(obj->send_len < HOBOT_MAX_SEND_BUF && obj->tag < POST_DATA_END && 0 == obj->interrupt)
        {
            HOTBOT_DEBUG("thread wait send len:%d\n", obj->send_len);
            pthread_cond_wait(&obj->cond, &obj->mutex);
        }
        if (obj->interrupt)
        {
            goto exit_post_data;
        }
        if (obj->tag < POST_DATA_END)
        {
            HOTBOT_DEBUG("******1 post data,send len:%d\n", obj->send_len);
            status = ghttp_chunked_post_body(obj->http_req, obj->send_buf, HOBOT_MAX_SEND_BUF);
            obj->send_len -= HOBOT_MAX_SEND_BUF;
            memmove(obj->send_buf, &(obj->send_buf[HOBOT_MAX_SEND_BUF]), obj->send_len);
        }
        else
        {
            HOTBOT_DEBUG("******2 post data,send len:%d\n", obj->send_len);
            status = ghttp_chunked_post_body(obj->http_req, obj->send_buf, obj->send_len);
            obj->send_len = 0;
            if((status != ghttp_done) && (status != ghttp_error_no_support_range))
            {
                print_err("post not finish\n");
                goto exit_post_data;
            }
            status = ghttp_chunked_post_body(obj->http_req, 0, 0);
            if((status != ghttp_done) && (status != ghttp_error_no_support_range))
            {
                print_err("post not finish\n");
                goto exit_post_data;
            }

            int statusCode = ghttp_status_code(obj->http_req);
            if((statusCode / 100) != 2)
            {
                print_err("http fail, statusCode=%d, err:%s\n", statusCode, ghttp_get_error(obj->http_req));
                if (!obj->interrupt && g_parser.an_cb)
                	g_parser.an_cb(0, 0, 0, 1);
            }
            else if(!obj->interrupt)
            {
                char result[1024*2];
                int len = ghttp_read(obj->http_req, result, sizeof(result)-1);
                if(len < 0)
                {
                    print_err("http get data fail, ret=%d\n", len);
                }
                else if(!obj->interrupt)
                {
                    result[len] = 0;
                    printf("hobot result:%s\n", result);
                    _parse_result(result, len);
                }
            }
        }

exit_post_data:
        obj->http_st = status;
        if((status != ghttp_done) && (status != ghttp_error_no_support_range))
        {
            print_err("post not finish\n");
            if (!obj->interrupt && g_parser.an_cb)
                g_parser.an_cb(0, 0, 0, 1);  //�������������ʾ
            if (obj->http_req)
            {
                ghttp_request_destroy(obj->http_req);
                obj->http_req = 0;
            }
            obj->flag = 0;
            obj->send_len = 0;
        }
		if (obj->tag == POST_DATA_END)
		{
            if (obj->http_req)
            {
                ghttp_request_destroy(obj->http_req);
                obj->http_req = 0;
            }
			obj->flag = 0;
			obj->send_len = 0;
		}
        pthread_cond_signal(&obj->cond);
        pthread_mutex_unlock(&obj->mutex);
    }
    return NULL;
}

int hobot_begin(char *appID, char *devID, char *secret, hobot_asr_callback fun1, hobot_answer_callback fun2)
{
    int ret;
    pthread_mutexattr_t mutexattr;
    pthread_condattr_t condattr;

    memset(&g_parser, 0, sizeof(g_parser));

    snprintf(g_parser.appID, sizeof(g_parser.appID), "%s", appID);
    snprintf(g_parser.devID, sizeof(g_parser.devID), "%s", devID);
    snprintf(g_parser.lisence, sizeof(g_parser.lisence), "%s", secret);

    g_parser.asr_cb = fun1;
    g_parser.an_cb = fun2;
    g_parser.tid = -1;

	g_parser.send_buf = (char *)malloc(MEM_POOL_SIZE*sizeof(char));
	if (!g_parser.send_buf)
	{
		print_err("malloc err\n");
		goto err_exit_begin;
	}
    ret = hobot_get_token(&g_parser, g_parser.token.token, sizeof(g_parser.token.token));
    if (1 != ret)
    {
    	goto err_exit_begin;
    }
    g_parser.http_req = _open_connection(&g_parser, 0);
    if (!g_parser.http_req)
    {
        print_err("open connection err\n");
        goto err_exit_begin;
    }
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&g_parser.mutex, &mutexattr);
    pthread_condattr_init(&condattr);
    pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&g_parser.cond, &condattr);


    g_parser.run = 1;
    if (pthread_create(&g_parser.tid, NULL, buf_send_thread, (void *)&g_parser) < 0)
    {
        print_err("start send thread err\n");
        goto err_exit_begin;
    }
    ret = _open_encoder(&g_parser);
    if (-1 == ret)
    {
    	goto err_exit_begin;
    }
    return ret;
err_exit_begin:
	hobot_end();
	return -1;
}

int hobot_end(void)
{
    if (-1 != g_parser.tid)
    {
        g_parser.run = 0;
        g_parser.interrupt = 1;
        pthread_mutex_lock(&g_parser.mutex);
        pthread_cond_broadcast(&g_parser.cond);
        pthread_mutex_unlock(&g_parser.mutex);
        pthread_join(g_parser.tid, NULL);
    }
    if (g_parser.send_buf)
    {
        free(g_parser.send_buf);
        g_parser.send_buf = 0;
    }
    if (g_parser.http_req)
    {
        ghttp_request_destroy(g_parser.http_req);
        g_parser.http_req = 0;
    }
    pthread_cond_destroy(&g_parser.cond);
    pthread_mutex_destroy(&g_parser.mutex);
    _close_encoder(&g_parser);
    return 0;
}
