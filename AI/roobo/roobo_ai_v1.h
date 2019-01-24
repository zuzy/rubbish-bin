
/*****************************************************************************
 *
 * File Name : roobo_ai_v1.h
 *
 * Description:
 *
 * Copyright (c) 2017     Intelligent Steward Co.,Ltd.
 * All rights reserved.
 *
 * Author : wanghui01@roobo.com
 *
 * Date : 2017-08-15
 *
 *****************************************************************************/

#ifndef __ROOBO_AI_V1__
#define __ROOBO_AI_V1__

#ifdef __cplusplus
extern "C" {
#endif

#define SSID_LEN 64
#define BSSID_LEN 20

enum
{
    RBAI_AUDIO_SAMPLE_INIT = 0x00,
    RBAI_AUDIO_SAMPLE_FIRST = 0x01,    // 开始写pcm数据
    RBAI_AUDIO_SAMPLE_CONTINUE = 0x02, // 继续写pcm数据
    RBAI_AUDIO_SAMPLE_LAST = 0x04,     // 结束写pcm数据
    RBAI_AUDIO_SAMPLE_CANCEL = 0x08,   // 取消本次语音识别
    RBAI_AUDIO_CN_EN_TRANSLATE_FIRST = 0x10,
};

// Wifi信息结构体，用于report location.
typedef struct
{
    char ssid[SSID_LEN]; //The network name.

    char bssid[BSSID_LEN]; //The address of the access point.

    int level; //The detected signal level in dBm.

} rb_wifi_info_t;

//===========回调函数=============

/*
 * 函数名称: asr_callback
 * 函数功能: 语音识别回调函数
 * 输入参数: char *pText 语音识别结果
            int textLen 语音识别结果长度
            int errCode 如果失败了返回错误码，0代表成功
 *
 */
typedef void (*asr_callback)(const char *pText, int textLen, int errCode);

/*
 * 函数名称: ai_answer_callback
 * 函数功能: AI结果回调函数
 * 输入参数: char *pText 语音识别结果
            int textLen 语音识别结果长度
            char *pUrl[3] AI结果
                  purl[0]返回tts，NULL时不处理
                  purl[1]指向资源文件，NULL时不处理，不NULL时调用播放器播放url
                  purl[2]指向返回结果的原始json串
            int errCode 如果失败了返回错误码，0代表成功
 *
 */
typedef void (*ai_answer_callback)(const char *pText, int textLen, char *pUrl[3], int errCode);

/*
 * 函数名称: register_device_callback
 * 函数功能: 设备注册回调函数
 * 输入参数: char *token 设备token
            char *clientId 设备SN号
            int errCode 如果失败了返回错误码，0代表成功
 *
 */
typedef void (*register_device_callback)(const char *token, const char *clientId, int errCode);

//===========回调函数=============

/*
 * 函数名称: RooboSessionBegin
 * 函数功能: 初始化session
 * 输入参数: char* params 初始化session的参数，格式如下：
                "clientid=%s,agentid=%s,token=%s,production=%s,name=%s"
            asr_callback asr_func ASR回调函数
            ai_answer_callback ai_func AI回调函数
 * 返回值: 0(调用成功),其他失败
 *
 */
int RooboSessionBegin(const char *params, asr_callback asr_func, ai_answer_callback ai_func);

/*
 * 函数名称: RooboSetRegisterDeviceCallback
 * 函数功能: 设置设备注册回调函数
 * 输入参数: register_device_callback new_callback 设备注册回调函数
 *
 */
void RooboSetRegisterDeviceCallback(register_device_callback new_callback);

/*
 * 函数名称: RooboAudioWrite
 * 函数功能: 语音识别
 * 输入参数: const void *waveData 音频数据
            unsigned int waveLen 音频数据长度
            int audioStatus 指示音频段落标记
                RBAI_AUDIO_SAMPLE_FIRST     开始写pcm数据，并传入第一帧，开始语音识别
                RBAI_AUDIO_SAMPLE_CONTINUE  继续写pcm数据
                RBAI_AUDIO_SAMPLE_LAST      写入最后一段pcm数据，结束本次语音识别
                RBAI_AUDIO_SAMPLE_CANCEL    取消本次语音识别
 * 返回值: 0(调用成功),-1(没有初始化或者初始化失败)
 *
 */
int RooboAudioWrite(const void *waveData, unsigned int waveLen, int audioStatus);

/*
 * 函数名称: RooboAudioWriteEx
 * 函数功能: 语音识别扩展，支持指定识别语音，支持翻译功能
            如果translate不为NULL，将提供翻译功能
 * 输入参数: const void *waveData 音频数据
            unsigned int waveLen 音频数据长度
            int audioStatus 指示音频段落标记
                RBAI_AUDIO_SAMPLE_FIRST     开始写pcm数据，并传入第一帧，开始语音识别
                RBAI_AUDIO_SAMPLE_CONTINUE  继续写pcm数据
                RBAI_AUDIO_SAMPLE_LAST      写入最后一段pcm数据，结束本次语音识别
                RBAI_AUDIO_SAMPLE_CANCEL    取消本次语音识别
            const char *language ASR识别语言，如果传入音频是英文，则设为en，如果是中文，则设为zh，设置为NULL则默认为zh，暂时只支持中英文
            const char *translate 翻译类型设置
                如果一键汉译英则设置为zh_en
                如果一键英译汉则设置为en_zh
 * 返回值: 0(调用成功),-1(没有初始化或者初始化失败)
 *
 */
int RooboAudioWriteEx(const void *waveData, unsigned int waveLen, int audioStatus, const char *language, const char *translate);

/*
 * 函数名称: RooboRegisterDevice
 * 函数功能: 设备注册
            调用前先设置register_device_callback，相关注册结果在回调函数中接收和处理
 * 输入参数: const char *agentId    // 产品id，预先分配
            const char *token      // 静态token,同agentid一样预先分配
            const char *production // 同一个产品id下的产品分类,无分类则填写产品名称，不能为NULL
            const char *name       // 第三方产品的唯一标识,没有，则内容同production，不能为NULL
            long timestamp         // 时间戳
 * 返回值: 0(调用成功),-1(未初始化，或者初始化失败)
 *
 */
int RooboRegisterDevice(const char *agentId, const char *token, const char *production, const char *name, long timestamp);

/*
 * 函数名称: RooboSessionEnd
 * 函数功能: 程序将要退出时调用，释放内部空间
 *
 */
int RooboSessionEnd();

/*
 * 函数名称: RooboTTSTextPut
 * 函数功能: 将文本转化为TTS。
            在ai_answer_callback回调中接收和处理返回结果，此时该回调将只包含音频文件url，pText无效
 * 输入参数: const char *text 需要做TTS转化的文本
 * 返回值: 0(调用成功),-1(未初始化，或者初始化失败)
 *
 */
int RooboTTSTextPut(const char *text);

/*
 * 函数名称: RooboReportLoc
 * 函数功能: 上报wifi列表用于定位
            有些语义服务需要基于地理位置信息(如问“今天天气怎么样”，服务器根据设备城市返回该城市天气)
            建议设备开机，WIFI连接上之后，执行完RooboSessionBegin后调用一次
 * 输入参数: rb_wifi_info_t *wifiInfo WIFI列表信息
            int count WIFI列表长度
 *
 */
void RooboReportLoc(rb_wifi_info_t *wifiInfo, int count);

/*
 * 函数名称: RooboSetAIParam
 * 函数功能: 设置AI相关的信息，目前只支持clientId和deviceToken
 * 输入参数: const char *paramName 参数名称
            const char *paramValue 参数对应的值
 * 返回值: 0(调用成功),-1(调用失败)
 *
 */
int RooboSetAIParam(const char *paramName, const char *paramValue);

/*
 * 函数名称: RooboGetAIParam
 * 函数功能: 获取AI相关的信息，目前只支持clientId和deviceToken
 * 输入参数: const char *paramName 参数名称
            char *out 保存输出的值
            size_t *len 输入时是out的最大长度（至少为目标长度+1），输出时是目标值字符串strlen结果
 *
 */
int RooboGetAIParam(const char *paramName, char *out, size_t *len);

/*
 * 函数名称: RooboTextQuery
 * 函数功能: 通过文本获取AI结果
            在ai_answer_callback回调中接收和处理返回结果
 * 输入参数: const char *text 文本
 * 返回值: 0(调用成功),-1(未初始化，或者初始化失败)，-2（传入参数为空）
 *
 */
int RooboTextQuery(const char *text);

/*
 * 函数名称: RooboResetAISence
 * 函数功能: 重置服务器AI场景状态。
            实时翻译等场景状态由服务器端维护，故可能存在客户端掉电，或者关机后，
            服务器未能正常退出场景，故需要调用此接口通知服务器退出当前场景状态。
 * 返回值 : 0(调用成功),-1(未初始化，或者初始化失败)，-2（deviceID或者token为空）
 *
 */
int RooboResetAISence();

/*
 * 函数名称: post_event_callback
 * 函数功能: post_event_async回调函数
 * 输入参数: void
 * 输出参数: int status_code 获取本次调用返回的状态，对应状态参考文档
            const char *result_data 请求云端回调数据结构参考文档
            void *user_data 用户自定义数据结构或数据
 * 返回值: void
 *
 */
typedef void (*post_event_callback)(int status_code, const char *result_data, void *user_data);

/*
 * 函数名称: RooboPostEventAsync
 * 函数功能: RooboPostEventAsync异步事件请求函数
 * 输入参数: int event_type 事件类型
            const char *content 事件数据结构参考文档
            post_event_callback callback 接收数据回调方法
            void *user_data 用户自定义数据此数据在回调post_event_callback方法中可以通过void *user_data参数中获取
 * 输出参数: void
 * 返回值: 0(调用成功)
 * NOTE: this api is not thread-safe
 *
 */
int RooboPostEventAsync(int event_type, const char *content, post_event_callback callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
