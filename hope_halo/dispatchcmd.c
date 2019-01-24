#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

#include "applib.h"
#include "act_debug.h"

#include "cloud_client.h"
#include "cJSON.h"
#include "command.h"
#include "common.h"
#include "http_regist.h"
#include "cloud_client.h"

#include "dispatchcmd.h"
#include "smart_player_api.h"

int dis_halo_ctrl(char *body){
    int ret = -1;
    cJSON *params = cJSON_Parse(body);
    if(NULL ==params){
        return -1;
    }
    cJSON *effect_item = cJSON_GetObjectItem(params, "effect");
    if(effect_item){
        print_zyf("effect %d %s", effect_item->type, effect_item->valuestring);
    }
    cJSON *source_item = cJSON_GetObjectItem(params, "source");
    if(source_item){
        print_zyf("source %d %s", source_item->type, source_item->valuestring);
    }
    cJSON *volume_item = cJSON_GetObjectItem(params, "volume");
    if(volume_item){
        print_zyf("volume %d %s", volume_item->type, volume_item->valuestring);
    }
    cJSON *mode_item = cJSON_GetObjectItem(params, "model");
    if(mode_item){
        print_zyf("model %d %s");
    }
    cJSON *playstate_item = cJSON_GetObjectItem(params, "playstate");
    if(playstate_item){
        print_zyf("playstate %d %s", playstate_item->type, playstate_item->valuestring);
        if(strstr(playstate_item->valuestring, "pause")){
            splayer_pause();
        }else if(strstr(playstate_item->valuestring, "play")){
            splayer_resume();
        }
    }
    cJSON *progress_item = cJSON_GetObjectItem(params, "progress");
    if(progress_item){
        print_zyf("progress %d %s", progress_item->type, progress_item->valuestring);
    }
    cJSON *songid_item = cJSON_GetObjectItem(params, "songid");
    if(songid_item){
        print_zyf("song %d %s", songid_item->type, songid_item->valuestring);
    }

Exit:
    cJSON_Delete(params) ;
    return 0;
}

int dis_cloud_ctrl(char *body, char *ref_id){
    int ret = -1;
    cJSON *root = cJSON_Parse(body);
    if(!root){
        return -1;
    }
    cJSON *devID_item = cJSON_GetObjectItem(root, "devId");
    if(!devID_item){
        goto Exit;
    }
    cJSON *profile_item = cJSON_GetObjectItem(root, "profile");
    if(!profile_item){
        goto Exit;
    }
    cJSON *status_item = cJSON_GetObjectItem(profile_item, "status");
    cJSON *play_itme = cJSON_GetObjectItem(profile_item, "play");
    cJSON *control_item = cJSON_GetObjectItem(profile_item, "control");
    cJSON *skip_item = cJSON_GetObjectItem(profile_item, "skip");
    cJSON *idvol_itme = cJSON_GetObjectItem(profile_item, "idvole");
    cJSON *mute_item = cJSON_GetObjectItem(profile_item, "mute");
    cJSON *setvol_item = cJSON_GetObjectItem(profile_item, "setvol");
    cJSON *source_item = cJSON_GetObjectItem(profile_item, "source");
    cJSON *effect_item = cJSON_GetObjectItem(profile_item, "effect");
    cJSON *model_item = cJSON_GetObjectItem(profile_item, "model");
    cJSON *local_item = cJSON_GetObjectItem(profile_item, "local");

    // ctrl(.......);

Exit:
    cJSON_Delete(root);
    return ret;
}