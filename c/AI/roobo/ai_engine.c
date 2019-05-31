
#include "include_psp.h"
#include "include_case.h"
#include "common_api.h"
#include "ai_api.h"
#include "roobo_ai_v1.h"
#include <wifi_api.h>

#define AI_AGENTID            "NWY4MDRhYjBhYzQ2"
#define AI_TOKEN              "242f812ba979a61a087caf21e78d7f364936"
#define AI_PRODUCTION         "yuantest123"
#define AI_NAME               "yuantest123"

const char *gtips[3] =
{
	"小主人，我也糊涂了，换个话题吧",
	"小主人，我没有听清楚，再说一次吧",
	"这个问题太难了，聊点别的吧"
};

typedef struct
{
    int (*TTSTextPut)(const char *);
    void (*ReportLoc)(rb_wifi_info_t *, int);
    int (*AudioWrite)(const void *, unsigned int, int);
    int (*SessionEnd)(void);
    int (*SessionBegin)(const char *, asr_callback, ai_answer_callback);
}roobo_ops_t;

typedef struct
{
	roobo_ops_t ops;
    void *dl_handle;
}roobo_lib_t;

typedef struct
{
    ai_callback ai_event_cb;
    AI_SERVICE_TYPE service_type;
}ai_inner_t;

static ai_inner_t ai_inner ={0,0};
static roobo_lib_t lib_roobo = {0};
static int gtag = RBAI_AUDIO_SAMPLE_INIT;

#define ROOBO_LIB_NAME "librooboai.so"

static int lib_init(void)
{
    if (lib_roobo.dl_handle != NULL)
        return 0;

    lib_roobo.dl_handle = dlopen(ROOBO_LIB_NAME, RTLD_LAZY);

    if(lib_roobo.dl_handle != NULL)
    {
        lib_roobo.ops.TTSTextPut = dlsym(lib_roobo.dl_handle, "RooboTTSTextPut");
        lib_roobo.ops.AudioWrite = dlsym(lib_roobo.dl_handle, "RooboAudioWrite");
        lib_roobo.ops.ReportLoc = dlsym(lib_roobo.dl_handle, "RooboReportLoc");
        lib_roobo.ops.SessionBegin = dlsym(lib_roobo.dl_handle, "RooboSessionBegin");
        lib_roobo.ops.SessionEnd = dlsym(lib_roobo.dl_handle, "RooboSessionEnd");
        return 0;
    }
    else
    {
        printf("dlopen %s error \n",ROOBO_LIB_NAME);
        return -1;
    }
}


static int lib_deinit(void)
{
    if (lib_roobo.dl_handle != NULL)
    {
        dlclose(lib_roobo.dl_handle);
        lib_roobo.dl_handle = NULL;
    }
}

static void ai_asr_cb(char *pText, int textLen, int errCode)
{
    ai_result_t ai_result;

    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.err_code = errCode;

    if (ai_inner.ai_event_cb)
        ai_inner.ai_event_cb(AI_EVENT_ASR, &ai_result);
}

static void ai_answer_cb(char *pText, int textLen, char **pUrl, int errCode)
{
    ai_result_t ai_result;
    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.urlgroup= pUrl;
    ai_result.err_code = errCode;

    if (errCode && gtag == RBAI_AUDIO_SAMPLE_CANCEL)
        return;

    if(errCode)
    {
        int time = 0;
        struct timeval now;

        gettimeofday(&now, NULL);
        time = now.tv_sec*1000 + now.tv_usec/1000;
        if (lib_roobo.ops.TTSTextPut)
            lib_roobo.ops.TTSTextPut(gtips[(time & 0x0fffffff)%3]);
    }
    else
    {
        if (ai_inner.ai_event_cb)
            ai_inner.ai_event_cb(AI_EVENT_ANSWER, &ai_result);
    }
}
static void _upload_wifi()
{
    int i = 0, ret = 0;
    rb_wifi_info_t wifiInfo[WIFI_CACHE_SCANRESULTS_COUNT];
    wifi_scan_results_t scan_results;

    memset(&scan_results, 0, sizeof(scan_results));
    memset(wifiInfo, 0, sizeof(wifiInfo));
    ret = wifi_get_scan_results(&scan_results);
    if (ret < 0 || scan_results.count == 0)
    {
        printf("no wifi ap found %d %d\n", ret, scan_results.count);
        return;
    }

    for (i = 0; i < scan_results.count; i++) {
        strcpy(wifiInfo[i].bssid, scan_results.result[i].bssid);
        strcpy(wifiInfo[i].ssid, scan_results.result[i].ssid);
        wifiInfo[i].level = scan_results.result[i].rssi;
    }
    if (lib_roobo.ops.ReportLoc)
    	lib_roobo.ops.ReportLoc(wifiInfo, scan_results.count);
}

void* ai_engine_init(void)
{
    char clientid[256] = {0};
    char  params[1024];
    if (sys_read_debug_config("ROOBOAI_SN", clientid, sizeof(clientid)) != 0)
    {
        if (sair_read_config(clientid, sizeof(clientid)) != 0)
        {
            printf("[sair] pls write ai sn.\n");
            return NULL;
        }
    }

    sprintf(params, "clientid=%s,agentid=%s,token=%s,production=%s,name=%s",
		clientid, AI_AGENTID, AI_TOKEN, AI_PRODUCTION, AI_NAME);

    printf("[sair] %s\n", params);
    int ret = -1;;
    if (lib_roobo.ops.SessionBegin)
    	ret = lib_roobo.ops.SessionBegin(params, ai_asr_cb, ai_answer_cb);

    if (0 != ret)
    {
        print_err("\nRooboSessionBegin failed! error code:%d +++ \n", ret);
    }
    else
    {
         _upload_wifi();
         return (void*)&ai_inner;
    }
    return NULL;
}

int ai_engine_finalize(void* handle)
{
    FUNC_ENTER;
    if (lib_roobo.ops.SessionEnd)
        lib_roobo.ops.SessionEnd();

    FUNC_LEFT;
    return 0;
}

int ai_engine_feed_data(void* handle, AI_DATA_TYPE data_type, char* data, int data_size)
{
    int ret = -1;
    int tag = 0;
    switch(data_type)
    {
		case AI_DATA_BEGIN:
			if (ai_inner.service_type == 0)
				gtag = RBAI_AUDIO_SAMPLE_FIRST;
			else
				gtag = RBAI_AUDIO_CN_EN_TRANSLATE_FIRST;
			break;
		case AI_DATA_CONTINUE:
			gtag = RBAI_AUDIO_SAMPLE_CONTINUE;
			break;
		default:
			gtag = RBAI_AUDIO_SAMPLE_LAST;
			break;
    }
    if (data && lib_roobo.ops.AudioWrite)
    	ret = lib_roobo.ops.AudioWrite((const void *)data, data_size, gtag);

    return ret;
}

int ai_engine_interrupt(void* handle)
{
    //not sure it can interupt ?
    char data[256];
    if (lib_roobo.ops.AudioWrite)
    {
        if ( gtag == RBAI_AUDIO_SAMPLE_CONTINUE || gtag == RBAI_AUDIO_SAMPLE_FIRST)	
        {
            lib_roobo.ops.AudioWrite((const void *)data, 0, RBAI_AUDIO_SAMPLE_LAST);
            printf("RBAI_AUDIO_SAMPLE_CANCEL \n");
            gtag = RBAI_AUDIO_SAMPLE_CANCEL;
            return lib_roobo.ops.AudioWrite((const void *)data, 0, RBAI_AUDIO_SAMPLE_CANCEL);			  
        }
    }
    return -1;
}


int ai_engine_get_property(AI_PROPERTY_TYPE property_type, char* value, int length)
{
    if(value == NULL)
    {
        printf("null parameter");
        return -1;		
    }

    memset(value, 0, length);

    if (AIPropertyInitOnce == property_type)
    {
        *value = 1;
    }

    return 0;
}


AI_ERROR_CODE ai_engine_set_params(AI_PARAM_TYPE param_type, unsigned long value_size, void *value)
{
    switch(param_type)
    {
        case AI_PARAM_EVENT_CB :
        {
            ai_inner.ai_event_cb = value;
            break;
        }
        case AI_PARAM_SERVICE_TYPE:
        {
            if (value_size == sizeof(AI_SERVICE_TYPE))
            {
                memcpy(&ai_inner.service_type, value, value_size);
                break;
            }
            else
            {
                return AI_ERROR_INVALID_PARAM;
            }
        }
        default:
            return AI_ERROR_INVALID_PARAM;

    }

    return AI_OK;
}

static int __attribute__((constructor)) so_init(void)
{
    lib_init() ;
    return 0;
}
static int __attribute__((destructor)) so_exit(void)
{
    lib_deinit();
    return 0;
}


