#ifndef __HP_SPEECH_H__
#define __HP_SPEECH_H__

#define HALO_SPEECH_DEBUG 0
#if HALO_SPEECH_DEBUG == 1
    #define print_speech(format, arg...)   do { printf("\033[31m[halo_speech]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_speech(format, arg...)   NULL
#endif

enum
{
    SPEECH_CMD_BEGIN = 0,
    SPEECH_CMD_DEV_INFO = 1,
    SPEECH_CMD_SCE_INFO,
    SPEECH_CMD_RMS_INFO,
    SPEECH_CMD_FLR_INFO,
    SPEECH_CMD_SCE_CTRL,
    SPEECH_CMD_DEV_CTRL,

    SPEECH_CMD_EMD,
};

int parse_speech_cmd(char *content);

#endif