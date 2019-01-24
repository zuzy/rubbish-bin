/*
 * Copyright (C) 2018 Actions
 * Author:lishiyuan
 */
 
#include "common_api.h"
#include "ai_api.h"
#include "aiengine.h"
#include <json-c/json.h>
#include <dlfcn.h>
#include "ai_json_parser.h"

typedef struct
{
    struct aiengine * (*aiengine_new)(const char *cfg);
    int (*aiengine_delete)(struct aiengine *engine);
    int (*aiengine_start)(struct aiengine *engine, const char *param, char id[64], aiengine_callback callback, const void *usrdata);
    int  (*aiengine_feed)(struct aiengine *engine, const void *data, int size);
    int  (*aiengine_stop)(struct aiengine *engine);
    int  (*aiengine_cancel)(struct aiengine *engine);
    int  (*aiengine_opt)(struct aiengine *engine, int opt, char *data, int size);
}ops_t;

typedef struct
{
    ops_t ops;
    void *dl_handle;
}lib_t;

typedef struct _engine_info 
{
    void* enginer;
    int status;
    long start_time;
    long end_time;
    char ask_buffer[512];
    char answer_buffer[1024]; 
    char music_url[512]; 	
} engine_info_t;


typedef struct
{
    ai_callback ai_event_cb;
    AI_SERVICE_TYPE service_type;
    void* handle;
    engine_info_t info;	
}engine_t;

#define RESOURCE_PATH "/usr/local/resource/speech/"

#define APPKEY   "151427543545836d"
#define SECRETKEY  "d1f23b714f875fa87af07e2905cf2897"

#define LUAPATH  RESOURCE_PATH"luabin.lub"
#define PROVISION  RESOURCE_PATH"aiengine-2.9.4-151427543545836d.provision"

#define SERIALNUMBER  "/tmp/provision.serialNumber"
#define OUTPUT  "/tmp/aiengine.log"
#define SERVER  "ws://s.api.aispeech.com:1028,ws://s.api.aispeech.com"

#define LIB_NAME "libspeech_engine.so"

static engine_t g_enginer ={0};
static lib_t lib_engine = {0};


static int lib_init(void)
{
    if (lib_engine.dl_handle != NULL)
        return 0;

    lib_engine.dl_handle = dlopen(LIB_NAME, RTLD_LAZY);

    if(lib_engine.dl_handle != NULL)
    {
        lib_engine.ops.aiengine_new = dlsym(lib_engine.dl_handle, "aiengine_new");
        lib_engine.ops.aiengine_delete = dlsym(lib_engine.dl_handle, "aiengine_delete");
        lib_engine.ops.aiengine_start = dlsym(lib_engine.dl_handle, "aiengine_start");
        lib_engine.ops.aiengine_feed = dlsym(lib_engine.dl_handle, "aiengine_feed");
        lib_engine.ops.aiengine_stop = dlsym(lib_engine.dl_handle, "aiengine_stop");
        lib_engine.ops.aiengine_cancel = dlsym(lib_engine.dl_handle, "aiengine_cancel");
        lib_engine.ops.aiengine_opt = dlsym(lib_engine.dl_handle, "aiengine_opt");		
        return 0;
    }
    else
    {
        printf("dlopen %s error \n",LIB_NAME);
        return -1;
    }
}


static int lib_deinit(void)
{
    if (lib_engine.dl_handle != NULL)
    {
        dlclose(lib_engine.dl_handle);
        lib_engine.dl_handle = NULL;
    }
    return 0;	
}

static void ai_asr_cb(char *pText, int textLen, int errCode)
{
    ai_result_t ai_result;

    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.err_code = errCode;		
		
    if (g_enginer.ai_event_cb)
        g_enginer.ai_event_cb(AI_EVENT_ASR, &ai_result);	
}

static void ai_answer_cb(char *pText, int textLen, char *pUrl[3], int errCode)
{
    ai_result_t ai_result;
    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.urlgroup= pUrl;
		
    ai_result.err_code = errCode;		
		
    if (g_enginer.ai_event_cb)
        g_enginer.ai_event_cb(AI_EVENT_ANSWER, &ai_result);	
}


static int cfg_cloud_sds(char* buffer, int buffer_size)
{
    if (buffer == NULL)
        return -1;

    char *buf;
    int ret = 0;
    json_object *object;
    json_object *object1;
    json_object *object2;
	
    object = json_object_new_object();
    json_object_object_add(object, "luaPath", json_object_new_string(LUAPATH));
    json_object_object_add(object, "appKey", json_object_new_string(APPKEY));
    json_object_object_add(object, "secretKey", json_object_new_string(SECRETKEY));
    json_object_object_add(object, "provision", json_object_new_string(PROVISION));
    json_object_object_add(object, "serialNumber", json_object_new_string(SERIALNUMBER));


    object1 = json_object_new_object();
    json_object_object_add(object1, "enable", json_object_new_int(1));
    json_object_object_add(object1, "output", json_object_new_string(OUTPUT));
    json_object_object_add(object, "prof", object1);


    object2 = json_object_new_object();
    json_object_object_add(object2, "server", json_object_new_string(SERVER));
    json_object_object_add(object, "cloud", object2);
	
    buf = (char *)json_object_to_json_string_ext(object,JSON_C_TO_STRING_PLAIN);

    if (buffer_size < strlen(buf))
    {
        printf("%s, %d buffer_size too small. \n", __FUNCTION__, __LINE__);
        ret = -1;
    }
    else
    {
        memcpy(buffer, buf, strlen(buf));
    }

EXIT:	 
    json_object_put(object);
    json_object_put(object1);
    json_object_put(object2);

    return ret;
}



static int param_cloud_sds(char* buffer, int buffer_size)
{
    if (buffer == NULL)
        return -1;

    char *buf;
    int ret = 0;
    json_object *object;
    json_object *object1;
    json_object *object2;
    json_object *object3;
    json_object *object4;
	
	
    object = json_object_new_object();
    json_object_object_add(object, "coreProvideType", json_object_new_string("cloud"));
    json_object_object_add(object, "vadEnable", json_object_new_int(0));

    object1 = json_object_new_object();
    json_object_object_add(object1, "audioType", json_object_new_string("wav"));
    json_object_object_add(object1, "channel", json_object_new_int(1));
    json_object_object_add(object1, "sampleBytes", json_object_new_int(2));
    json_object_object_add(object1, "sampleRate", json_object_new_int(16000));
    json_object_object_add(object1, "compress", json_object_new_string("speex"));
    json_object_object_add(object, "audio", object1);


    object2 = json_object_new_object();
    json_object_object_add(object2, "userId", json_object_new_string("aispeech"));
    json_object_object_add(object, "app", object2);


    object3 = json_object_new_object();
    json_object_object_add(object3, "coreType", json_object_new_string("cn.sds"));
    json_object_object_add(object3, "res", json_object_new_string("aihome"));

    object4 = json_object_new_object();
    json_object_object_add(object4, "prevdomain", json_object_new_string(""));
    json_object_object_add(object4, "lastServiceType", json_object_new_string(""));
    json_object_object_add(object3, "sdsExpand", object4);
	
    json_object_object_add(object, "request", object3);
	
    buf = (char *)json_object_to_json_string_ext(object,JSON_C_TO_STRING_PLAIN);

    if (buffer_size < strlen(buf))
    {
        printf("%s, %d buffer_size too small. \n", __FUNCTION__, __LINE__);
        ret = -1;
    }
    else
    {
        memcpy(buffer, buf, strlen(buf));
    }

EXIT:	 
    json_object_put(object);
    json_object_put(object1);
    json_object_put(object2);
    json_object_put(object3);
    json_object_put(object4);

    return ret;
}

static int32_t auth_success(char* buffer)
{
/*{"success": "auth success"}*/

    json_object *o = NULL;
    json_object *res = NULL;
    json_tokener *tok = NULL;
    json_object *new_obj = NULL;
    int ret = -1;

    if (buffer == NULL)
        return -1;

    printf("buf : %s\n",buffer);

    tok = json_tokener_new();
    if (tok == NULL)
    {
        printf("[AI] tok new error\n");
        return -1;
    }
    json_tokener_set_flags(tok, 0);
    new_obj = json_tokener_parse_ex(tok, buffer, strlen(buffer));
    if (new_obj == NULL)
    {
        printf("[AI]  expected valid object\n");
        goto Exit;
    }

    if (json_object_object_get_ex(new_obj, "success", &o) && json_object_is_type(o, json_type_string))
    {
        if (strcmp(json_object_get_string(o), "auth success") == 0)
        {
        	ret = 1;
        	goto Exit;
        }
    }
Exit:
	if (new_obj)
		json_object_put(new_obj);
	if (tok)
		json_tokener_free(tok);
    return ret;
}

static long int _get_time() 
{
    long int at = 0;    
    struct timespec now;    
    clock_gettime(CLOCK_MONOTONIC, &now);    
    at = now.tv_sec * 1000 + now.tv_nsec / 1000000;    
    return at; 
}

static int engine_callback(void *usrdata, const char *id, int type, const void *message, int size) 
{
    engine_info_t *info = (engine_info_t *)usrdata; 
    engine_t* enginer = (engine_t*) info->enginer;
	
    printf("[AI]  %.*s\n", size, message);    
    if (type == AIENGINE_MESSAGE_TYPE_JSON) 
    {        
        info->end_time = _get_time();        
        long int delay = info->end_time - info->start_time;        
        printf("[AI]  start_time - end_time = %ldms\n",delay);  

        int ret;	
        abuffer_t ask_abuffer,answer_abuffer,  music_abuffer;

        ask_abuffer.data = info->ask_buffer;
        ask_abuffer.size = sizeof (info->ask_buffer);
        answer_abuffer.data = info->answer_buffer;
        answer_abuffer.size = sizeof (info->answer_buffer);
        music_abuffer.data = info->music_url;
        music_abuffer.size = sizeof (info->music_url);	

        memset(info->ask_buffer, 0, sizeof(info->ask_buffer));		
        memset(info->answer_buffer, 0, sizeof(info->answer_buffer));		
        memset(info->music_url, 0, sizeof(info->music_url));		

        if ((ret = ai_json_parser(message, &ask_abuffer, &answer_abuffer, &music_abuffer)) > 0)
        {
            if (ret&BIT_ANSWER_ABUFFER)
            {
                ai_asr_cb(info->answer_buffer, strlen(info->answer_buffer), 0);
                cntts_engine_start(enginer->handle, info->answer_buffer, strlen(info->answer_buffer));				
            }	
        }
        info->status = ret;  
		
    }
    return 0;
}


static void tts_callback(int status, char* tts_url, void* userdata)
{
    if (userdata == NULL)
        return;

    engine_info_t *info = (engine_info_t *)userdata; 
    engine_t* enginer = (engine_t*) info->enginer;
	
    if (info->status	 > 0)	
    {
        cntts_engine_stop(enginer->handle);
			
        char *pUrl[3]={0};

        pUrl[0]= tts_url;

        if (info->status&BIT_MUSIC_ABUFFER)
            pUrl[1]= info->music_url;	

        ai_answer_cb(info->ask_buffer, strlen(info->ask_buffer), pUrl, 0);	
    }

}

void* ai_engine_init(void)
{
    FUNC_ENTER;

    struct aiengine *engine = NULL;
    char buffer[1024] = {0};
    int ret;
    int authed = 0;

    if (cfg_cloud_sds(buffer, sizeof(buffer)) != 0)
    {
        FUNC_LEFT;
        return NULL;
    }

    printf("cfg: %s\n", buffer);

    if (lib_engine.ops.aiengine_new)
        engine = lib_engine.ops.aiengine_new(buffer);

    if (engine == NULL) 
    {
        printf("[AI]  new engine error.\n");
        return NULL;
    }

    memset(buffer, 0, sizeof(buffer));

    //check auth
    if (lib_engine.ops.aiengine_opt)    
        ret = lib_engine.ops.aiengine_opt(engine, 10, buffer, 1024); //11为主动授权，10为检查授权

    if (ret == -1) {
        printf("[AI]  check auth error\n");
    } else {
        authed = auth_success(buffer);		
    }

    if (authed != 1) {
        memset(buffer, 0, sizeof(buffer));

        if (lib_engine.ops.aiengine_opt)    
            ret = lib_engine.ops.aiengine_opt(engine, 11, buffer, 1024); //11为主动授权，10为检查授权

        if (ret == -1) {
            printf("[AI]  do auth error\n");
        } else {
            authed = auth_success(buffer);		
        }
    }	

    if (authed  != 1)
    {
        if (lib_engine.ops.aiengine_delete)
            lib_engine.ops.aiengine_delete(engine);

        FUNC_LEFT;
        return NULL;
    }

    cntts_set_engine_api(lib_engine.ops.aiengine_start, lib_engine.ops.aiengine_stop, tts_callback, &(g_enginer.info));

    g_enginer.info.enginer = &g_enginer;

    g_enginer.handle = (void*)engine;

    FUNC_LEFT;
    return (void*)&g_enginer;
}

int ai_engine_finalize(void* handle)
{
    if (handle == NULL)
        return -1;
  
    engine_t* handler = (engine_t*)handle;
    struct aiengine *engine = (struct aiengine* )handler->handle;

    if (engine == NULL)
        return -1;	

    FUNC_ENTER;

    if (lib_engine.ops.aiengine_delete)
        lib_engine.ops.aiengine_delete(engine);

    FUNC_LEFT;
    return 0;
}

int ai_engine_feed_data(void* handle, AI_DATA_TYPE data_type, char* data, int data_size)
{
    if (handle == NULL)
        return -1;
  
    engine_t* handler = (engine_t*)handle;
    struct aiengine *engine = (struct aiengine* )handler->handle;

    if (engine == NULL)
        return -1;
	
    engine_info_t* info = &(handler->info);	
    int ret = -1;
    int tag = data_type;

    switch(data_type)
    {
        case AI_DATA_BEGIN:
        {
            char param[1024] = {0};
            if (param_cloud_sds(param, sizeof(param)) != 0)
            {
               return -1;
            }	
            char uuid[64];    
            memset(uuid, 0, sizeof(uuid)); 
            if (lib_engine.ops.aiengine_start)	
            ret = lib_engine.ops.aiengine_start(engine, param, uuid, engine_callback, info);

            if (ret == -1) 
            {
                printf("[AI]  start engine error.\n"); 
                break;
            }
            printf("[AI]  uuid--->%s\n", uuid);
            info->status = 0;
            info->start_time = _get_time();     
            info->end_time = 0;		   

            if (data &&lib_engine.ops.aiengine_feed)	
                ret = lib_engine.ops.aiengine_feed(engine, data, data_size);			    
        }
            break;		
        case AI_DATA_END:
        {
            if (lib_engine.ops.aiengine_stop)   	
                ret = lib_engine.ops.aiengine_stop(engine);
            if (ret == -1) 
            {
                printf("[AI]  stop engine error.\n");
            }
        }
            break;
        default:
            if (data &&lib_engine.ops.aiengine_feed)	
                ret = lib_engine.ops.aiengine_feed(engine, data, data_size);	
        break;
    }

    return ret;
}

int ai_engine_interrupt(void* handle)
{
    if (handle == NULL)
        return -1;
  
    engine_t* handler = (engine_t*)handle;
    struct aiengine *engine = (struct aiengine* )handler->handle;

    if (engine == NULL)
        return -1;

    if (lib_engine.ops.aiengine_cancel)   	
        lib_engine.ops.aiengine_cancel(engine);	
	
    return 0;
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
        *value = 0;
    }

    return 0;
}


AI_ERROR_CODE ai_engine_set_params(AI_PARAM_TYPE param_type, unsigned long value_size, void *value)
{
    switch(param_type)
    {
        case AI_PARAM_EVENT_CB :
        {
            g_enginer.ai_event_cb = value;
            break;
        }
        case AI_PARAM_SERVICE_TYPE:
        {
            if (value_size == sizeof(AI_SERVICE_TYPE))
            {
                memcpy(&g_enginer.service_type, value, value_size);
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


