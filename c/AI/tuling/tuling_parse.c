/*
 * tuling_parse.c
 *
 *  Created on: 2017-12-10
 *      Author: linjintian
 */
#include "Alooper/Errors.h"
#include "Alooper/Looper.h"
#include "libc.h"
#include "ghttp/ghttp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include <json-c/json.h>
#include "tuling_parse.h"
#include "debug.h"
#include <pthread.h>
#include "cJSON.h"

#define ENABLE_TL_DEBUG   (0)
#define ENABLE_DUMP          (0)

#define OPUS_ENCODE_SAMPLE_RATE 16000
#define OPUS_ENCODE_BITRATE 20
#define OPUS_ENCODE_CHANNELS 1
#define OPUS_ENCODE_MODE 2048
#define OPUS_ENCODE_COMPLEXITY 10
#define OPUS_PER_FRAME_SAMPLES  320
#define TULING_MAX_SEND_BUF   (1024)
#define MEM_POOL_SIZE  (2*1024)

#define TULING_BOUNDARY_H   "------AiWiFiBoundary"

#if ENABLE_TL_DEBUG
#define TULING_DEBUG(format, arg...)   do { printf("[%d,%s]"format,__LINE__,__func__, ## arg);}while (0)
#else
#define TULING_DEBUG(format, arg...)   do {} while (0)
#endif

typedef struct
{
    /*! 采样率，单位hz */
    int sample_rate;
    /*! 声道数 */
    int channels;
    /*! 歌曲比特率 */
    int bitrate;
    /*! 编码库格式，一个加载的编码库里可能包含多个格式，例：IMADPCM */
    int audio_format;
    /*! 一次获取到的音频编码后数据播放时长，0说明不限制，单位ms */
    int chunk_time;
}enc_audio_t;

typedef struct
{
    /*! 编码一帧所需的样本数 */
    int samples_per_frame;
    /*! payload/packet大小，单位字节 */
    int chunk_size;
    /*! 初始写入文件起始的数据 */
    /*! 数据指针 */
    void *buf;
    /*! 数据长度 */
    int buf_len;
}audioenc_attribute_t;


/*!
 * \brief
 *      定义最大的输入声道数
 */
#define MAX_CHANNEL_IN     2
/*!
 * \brief
 *      音频编码插件帧解码的输入或音频输入模块的输出
 */
typedef struct
{
    /*! pcm数据指针数组 */
    int pcm[MAX_CHANNEL_IN];
    /*! 当前输入的channel数 */
    int channels;
    /*! 当前输入的sample数 */
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
    char usrID[128];
    char apikey[128];
    token_record_t token;

    tuling_asr_callback asr_cb;
    tuling_answer_callback an_cb;

    void *lib_handle;
    audioenc_plugin_t *plugin_info; /* 解码插件信息 */
    void *plugin_handle;
    void *http_req;
    char *send_buf;
    int send_len;
    int flag;
    int run;
    int tag;
    int interrupt;
    int http_st;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t tid;

    tuling_service_e mode;
}tuling_inner_t;

static tuling_inner_t g_parser;
#if ENABLE_DUMP
static int g_fd_post = -1;
static int g_w_count = 0;
#endif

static char upload_parameters[] =
	"Content-Disposition: form-data; name=\"parameters\"\r\n\r\n";

static char upload_speech[] =
    "Content-Disposition: form-data; name=\"speech\"; filename=\"test.opus\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n";

static int _post_interrupt(void *param)
{
    tuling_inner_t *h = (tuling_inner_t *)param;

    if(h && 0 == h->interrupt)
    {
        return 0;
    }
    return 1;
}


static int _parse_result(const char *src, int len)
{
    json_object *new_obj = NULL;
    json_object *res = NULL;
    json_object *o = NULL;
    json_tokener *tok = NULL;
    int code = 0;
    char *asr = 0;
    int asr_len = 0;
    char *text = 0;
    int text_len = 0;
    char *ttsURL = 0;
    char *resURL = 0;
    char *URL[10];
    int url_num = 0;
	int i = 0;

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
    if (json_object_object_get_ex(new_obj, "code", &o) && json_object_is_type(o, json_type_int))
    {
        code = json_object_get_int(o);
    }

    if (json_object_object_get_ex(new_obj, "tts", &o) && json_object_is_type(o, json_type_string))
    {
        text = (char *)json_object_get_string(o);
        text_len = json_object_get_string_len(o);
    }

    if (json_object_object_get_ex(new_obj, "asr", &o) && json_object_is_type(o, json_type_string))
    {
    	asr = (char *)json_object_get_string(o);
    	asr_len = json_object_get_string_len(o);
    }

    if (json_object_object_get_ex(new_obj, "nlp", &o) && json_object_is_type(o, json_type_array))
    {
    	url_num = json_object_array_length(o);
    	json_object *tmp = 0;
    	for(i=0; i<url_num && i<9; i++)
    	{
    		tmp = json_object_array_get_idx(o, i);
    		if (json_object_is_type(tmp, json_type_string))
    		{
    			URL[i] = (char *)json_object_get_string(tmp);
    		}
    	}
    }
    if (json_object_object_get_ex(new_obj, "func", &o) && json_object_object_get_ex(o, "url", &res)
    		&& json_object_is_type(res, json_type_string))
    {
    	URL[i] = (char *)json_object_get_string(res);
    }

    printf("url num %d\n", i);
    if (i <= 0)
    {
        if (g_parser.an_cb)
        	g_parser.an_cb(0, 0, 0, 1);
        goto P_RES_EXIT;
    }

    if (asr && g_parser.asr_cb)
    {
        g_parser.asr_cb(asr, asr_len, 0);
    }
    if (i && g_parser.an_cb)
    {
        g_parser.an_cb(text, text_len, URL, 0);
    }

    if (json_object_object_get_ex(new_obj, "token", &o) && json_object_is_type(o, json_type_string))
    {
    	snprintf(g_parser.token.token, sizeof(g_parser.token.token), "%s", (char *)json_object_get_string(o));
        set_config(CFG_SAIR_TOKEN_RECORD, (char*)&(g_parser.token.token), sizeof(token_record_t));
        sync_config();
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
    static int remain_smp = 0;      //opus要求每一帧都要凑够320个点
    static short last_buf[OPUS_PER_FRAME_SAMPLES] = {0};

    audioin_pcm_t ai;

    ai.channels = 1 ;
    ai.samples = OPUS_PER_FRAME_SAMPLES;

    int i = 0;
    int j = 0;
    int k = 0;;

    TULING_DEBUG("enter _encode_one_frame,remain:%d,samples:%d\n", remain_smp, samples);
    if (tail && remain_smp > 0)
    {
        resv = OPUS_PER_FRAME_SAMPLES-remain_smp;
        memset(&(last_buf[remain_smp]), 0, resv*sizeof(short));
        ai.pcm[0] = (int)last_buf;
        ret = plugin->frame_encode(plugin_handle, &ai, &(out[len]), &bytes_used);
        len += bytes_used;
        TULING_DEBUG("******* encode the tail data len:%d\n", len);
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
        TULING_DEBUG("++++encode last remain ret:%d,len%d,bytes_used:%d\n", ret,len,bytes_used);
        len += bytes_used;
        pos = 0;
    }

    if (loop_pos >= 0)
    {
        for(i=0; i<k; i++)
        {
            ai.pcm[0] = (int)&(pcm[loop_pos + i*OPUS_PER_FRAME_SAMPLES]);
            ret = plugin->frame_encode(plugin_handle, &ai, &(out[len]), &bytes_used);
            //TULING_DEBUG("====encode frame ret:%d,i:%d,len%d,bytes_used:%d\n", ret,i,len,bytes_used);
            len += bytes_used;
        }
        if (j > 0)
        {
            remain_smp = j;
            memcpy((char*)&(last_buf[pos]), (char*)&(pcm[loop_pos+k*OPUS_PER_FRAME_SAMPLES]), remain_smp*sizeof(short));
            TULING_DEBUG("****** remain pos:%d, byt:%d, k:%d\n", pos, remain_smp, k);
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

static int _tuling_set_header(ghttp_request *request)
{
    char tmp[128] = {0};

    if (ghttp_set_uri(request, "http://smartdevice.ai.tuling123.com/speech/chat") != 0)
    {
        print_err("set uri err !!\n");
        return -1;
    }

    snprintf(tmp, sizeof(tmp), "multipart/form-data; boundary=%s", TULING_BOUNDARY_H);
    ghttp_set_header(request, "Content-Type", tmp);
    ghttp_set_header(request, "Connection", "Keep-Alive");
    ghttp_set_header(request, "Cache-Control", "no-cache");
    ghttp_set_header(request, "Accept", "*/*");
    ghttp_set_header(request, "Accept-Encoding", "gzip, deflate");
    ghttp_set_header(request, "Accept-Language", "en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4,zh-TW;q=0.2,es;q=0.2");
    ghttp_set_header(request, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
    ghttp_set_type(request, ghttp_type_post);
    return 0;
}

static ghttp_request *_open_connection(void *context, int service_mode)
{
    ghttp_request *req = 0;
    char* twoHyphens = "--";

    req = ghttp_request_new(_post_interrupt, context);
    if (req == NULL)
    {
        print_err("request new err !!\n");
        return NULL;
    }
    return req;

err_connect:
	if (req)
	{
		ghttp_request_destroy(req);
	}
	return NULL;
}

static void get_rand_str(char s[], int number)
{
    char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i,lstr;
    char ss[2] = {0};
    lstr = strlen(str);
    srand((unsigned int)time((time_t *)NULL));
    for(i = 1; i <= number; i++)
    {
       sprintf(ss,"%c",str[(rand()%lstr)]);
       strcat(s,ss);
    }
}

static ghttp_status _tuling_post(ghttp_request *request, char *datas, int data_len, int tag)
{
	static int index = 1;
	char *buf = 0;
	char pre[2048];
	int tlen,plen = 0;
	static char identify[20] = {0};
	char *end_boundary = "\r\n--"TULING_BOUNDARY_H"--\r\n";
	char *second_boundary = "\r\n--"TULING_BOUNDARY_H"\r\n";
	char *first_boundary = "\r\n--"TULING_BOUNDARY_H"\r\n";
	ghttp_status status = ghttp_error;

	if (!request)
	{
		return status;
	}

	ghttp_clean(request);
	if (_tuling_set_header(request) < 0)
	{
		return status;
	}
	if (1 == index)
	{
		memset(identify, 0, sizeof(identify));
		get_rand_str(identify,sizeof(identify)-1);
	}
	else if (tag == POST_DATA_END)
	{
		index = (-index);
	}

	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root,"ak",g_parser.apikey);
	cJSON_AddStringToObject(root,"uid",g_parser.usrID);
	cJSON_AddStringToObject(root,"token", g_parser.token.token);
	cJSON_AddNumberToObject(root,"asr", 4);
	cJSON_AddNumberToObject(root,"tts", 3);
    cJSON_AddNumberToObject(root,"flag", 3);
    cJSON_AddStringToObject(root,"identify",identify);
    cJSON_AddNumberToObject(root,"realTime", 1);
    cJSON_AddNumberToObject(root,"index", index);
    cJSON_AddNumberToObject(root,"type", 0);
	char* str_js = cJSON_Print(root);
    cJSON_Delete(root);
    if (!str_js)
    {
    	return status;
    }
    snprintf(pre,sizeof(pre),"%s%s%s%s%s",first_boundary,upload_parameters,str_js,second_boundary,upload_speech);
	//TULING_DEBUG("%s\n", pre);
	free(str_js);
	plen = strlen(pre);
	tlen = data_len+plen+strlen(end_boundary);
	TULING_DEBUG("tlen:%d,plen:%d,dlen:%d\n", tlen, plen, data_len);
	buf = malloc((tlen+1)*sizeof(char));
	if (!buf)
	{
		TULING_DEBUG("malloc err\n");
		return status;
	}
	memcpy(buf,pre,plen);
	memcpy(&buf[plen],datas,data_len);
#if ENABLE_DUMP
	if (g_fd_post >= 0)
	{
		write(g_fd_post, datas, data_len);
	}
#endif
	memcpy(&buf[plen+data_len],end_boundary,strlen(end_boundary));

	ghttp_set_body(request, buf, tlen);
    ghttp_prepare(request);
	status = ghttp_connect(request, 0ll, 0ll);
	printf("ghttp_connect ret:%d, req:0x%x\n", status, request);

	if (tag != POST_DATA_END)
	{
		index++;
	}
	else
	{
		index = 1;
#if ENABLE_DUMP
		if (g_fd_post >=0)
		{
			close(g_fd_post);
			g_fd_post = -1;
		}
#endif
	}

	if((status != ghttp_done) && (status != ghttp_error_no_support_range))
	{
		print_err("http fail\n");
		index = 1;
	}
exit_tuling_post_one:

	free(buf);
	return status;
}

static void _wait_write_buf(tuling_inner_t *handle, char *in, int bytes, int tag)
{
    if (!handle->flag)
    {
        return ;
    }
    pthread_mutex_lock(&handle->mutex);
    while(handle->send_len+bytes > MEM_POOL_SIZE)
    {
        printf("write wait send len:%d\n", handle->send_len);
        pthread_cond_wait(&handle->cond, &handle->mutex);
    }
    TULING_DEBUG("send len:%d,byte:%d,flag:%d,tag:%d\n", handle->send_len, bytes,handle->flag,tag);
    if (handle->flag)
    {
		memcpy(&(handle->send_buf[handle->send_len]), in, bytes);
		handle->send_len += bytes;
		handle->tag = tag;
    }
    pthread_cond_signal(&handle->cond);
    pthread_mutex_unlock(&handle->mutex);
}

void tuling_interrupt(void)
{
    if (!g_parser.interrupt)
    {
        g_parser.interrupt = 1;//上次的还没结束
        pthread_mutex_lock(&g_parser.mutex);
        pthread_cond_broadcast(&g_parser.cond);
        pthread_mutex_unlock(&g_parser.mutex);
    }
}

int tuling_post_data(short *pcm, int samples, tuling_post_tag tag, tuling_service_e mode)
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
                g_parser.interrupt = 1;//上次的还没结束
                pthread_mutex_lock(&g_parser.mutex);
                pthread_cond_broadcast(&g_parser.cond);
                pthread_mutex_unlock(&g_parser.mutex);
            }
            usleep(10);
        }
        TULING_DEBUG("start post first\n");
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
        snprintf(path, sizeof(path), "/mnt/card/postdata_%d.opus", fd_c);
        if (-1 == g_fd_post)
        	g_fd_post = open(path, O_RDWR | O_CREAT, 777);
        fd_c++;
        if (fd1 >= 0)
            write(fd1, (char*)pcm, samples*sizeof(short));
        g_w_count = 0;
#endif
        bytes_used = _encode_one_frame(g_parser.plugin_handle, g_parser.plugin_info, pcm, samples, write_buf, 0);
        //TULING_DEBUG("+++++tuling_post_data frame encode bytes:%d, samples:%d\n", bytes_used, samples);
        if (bytes_used > 0)
        {
#if ENABLE_DUMP
            if (fd2 >= 0)
                write(fd2, write_buf, bytes_used);
            g_w_count += bytes_used;
            TULING_DEBUG("+++++++gwcount:%d\n", g_w_count);
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
        TULING_DEBUG("-----tuling_post_data frame encode bytes:%d, samples:%d\n", bytes_used, samples);
        if (bytes_used > 0)
        {
#if ENABLE_DUMP
            if (fd2 >= 0)
                write(fd2, write_buf, bytes_used);
            g_w_count += bytes_used;
            TULING_DEBUG("+++++++gwcount:%d\n", g_w_count);
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
            g_w_count += bytes_used;
            TULING_DEBUG("+++++++gwcount:%d\n", g_w_count);
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

static void _close_encoder(tuling_inner_t *h)
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

static int _open_encoder(tuling_inner_t *h)
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
    printf("=========tuling finish open encoder, att :%d,%d,%x\n", attribute.samples_per_frame, attribute.chunk_size, g_parser.plugin_handle);
    return 0;
begin_err:
    _close_encoder(h);
    return -1;
}


static void *buf_send_thread(void *handle)
{
    tuling_inner_t *obj = (tuling_inner_t*)handle;
    ghttp_status status = ghttp_error;
    char io[MEM_POOL_SIZE];
    int io_len = 0;
    int tag = 0;
    while(obj->run)
    {
        if (!obj->flag)
        {
            usleep(5);
            continue;
        }
        if (obj->tag == POST_DATA_FIRST)
        {
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
            //TULING_DEBUG("http req has destory in post, tag:%d\n", obj->tag);
            pthread_mutex_unlock(&obj->mutex);
            continue;
        }
        if (!obj->run)
        {
            goto exit_post_data;
        }
        while(obj->send_len < TULING_MAX_SEND_BUF && obj->tag < POST_DATA_END && 0 == obj->interrupt)
        {
            TULING_DEBUG("thread wait send len:%d\n", obj->send_len);
            pthread_cond_wait(&obj->cond, &obj->mutex);
        }
        tag = obj->tag;
		io_len = obj->send_len;
		memcpy(io, obj->send_buf, io_len);
		obj->send_len = 0;
		TULING_DEBUG("****** post data,send len:%d,tag:%d\n", io_len,tag);

		pthread_cond_signal(&obj->cond);
		pthread_mutex_unlock(&obj->mutex);

        if (obj->interrupt)
        {
            goto exit_post_data;
        }

		status = _tuling_post(obj->http_req, io, io_len, tag);
        if (tag == POST_DATA_END)
        {
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
                char result[1024*4];
                int len = ghttp_read(obj->http_req, result, sizeof(result)-1);
                if(len < 0)
                {
                    print_err("http get data fail, ret=%d\n", len);
                }
                else if(!obj->interrupt)
                {
                    result[len] = 0;
                    printf("tuling result:%s\n", result);
                    _parse_result(result, len);
                }
            }
        }

exit_post_data:
		pthread_mutex_lock(&obj->mutex);
        obj->http_st = status;
        if((status != ghttp_done) && (status != ghttp_error_no_support_range))
        {
            print_err("post not finish\n");
            if (!obj->interrupt && g_parser.an_cb)
                g_parser.an_cb(0, 0, 0, 1);  //网络问题给的提示

            if (obj->http_req)
            {
                ghttp_request_destroy(obj->http_req);
                obj->http_req = 0;
            }
            obj->flag = 0;
            obj->send_len = 0;
        }
		if (tag == POST_DATA_END)
		{
            if (obj->http_req)
            {
                ghttp_request_destroy(obj->http_req);
                obj->http_req = 0;
            }
			obj->flag = 0;
			obj->send_len = 0;
			tag = 0;
		}
        pthread_cond_signal(&obj->cond);
        pthread_mutex_unlock(&obj->mutex);
    }
    return NULL;
}

int tuling_begin(char *usrID, char *apiKey, char *aesKey, tuling_asr_callback fun1, tuling_answer_callback fun2)
{
    int ret;
    pthread_mutexattr_t mutexattr;
    pthread_condattr_t condattr;
    token_record_t token_t;

    memset(&g_parser, 0, sizeof(g_parser));

    snprintf(g_parser.apikey, sizeof(g_parser.apikey), "%s", apiKey);

	unsigned char in[17];
	unsigned char out[64] = {'0'};
	unsigned char aes_key_1[17];
	unsigned char iv[17]={0};

	memset(in, 0, sizeof(in));
	memset(aes_key_1, 0, sizeof(aes_key_1));
	memset(iv, 0, sizeof(iv));

	snprintf(in, sizeof(in), "%s", usrID);
	snprintf(aes_key_1, sizeof(aes_key_1), "%s", aesKey);

	memcpy(iv, apiKey, 16);
	AES128_CBC_encrypt_buffer(out, in, 16, aes_key_1, iv);
	unsigned char outStr[64] = {'0'};
	int i,aseLen=0;
	for(i=0;i < 16;i++)
	{
		aseLen+=snprintf(outStr+aseLen,64,"%.2x",out[i]);
	}
	TULING_DEBUG("begin usrid:%s\n", outStr);
	snprintf(g_parser.usrID, sizeof(g_parser.usrID), "%s", (char*)outStr);

    g_parser.asr_cb = fun1;
    g_parser.an_cb = fun2;
    g_parser.tid = -1;

	g_parser.send_buf = (char *)malloc(MEM_POOL_SIZE*sizeof(char));
	if (!g_parser.send_buf)
	{
		print_err("malloc err\n");
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
	tuling_end();
	return -1;
}

int tuling_end(void)
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
