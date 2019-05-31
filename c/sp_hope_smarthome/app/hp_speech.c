#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "hp_speech.h"
#include "cJSON.h"
#include "tcp_ser_cli.h"
#include "cjson_list.h"
#include "sair_api.h"

static int diapatch_speech_villa(char *jstr)
{
    print_speech("enter speech dispatch!");
    char body[1024] = "";
    print_speech("speech input %lld", get_now_us());
    lua_call_fun("/tmp/lua/speech.lua", "speech_to_villa", "s>s", jstr, body);
    print_speech("speech input %lld", get_now_us());
    print_speech("get body %s", body);
    return 0;
}

static char *delete_ids_add_id(char *input, char *num)
{
    char *out = NULL;
    cJSON *root = REQ_JSON_PARSE(input, root, Exit);
    cJSON *data = REQ_JSON_OBJ(root, data, Exit);
    cJSON *payload = cJSON_GetArrayItem(data, 0);
    if(payload == NULL) {
        goto Exit;
    }
    cJSON_DeleteItemFromObject(payload, "ids");
    if(num != NULL) {
        cJSON_AddNumberToObject(payload, "id", atoi(num));
    }
    out = cJSON_PrintUnformatted(root);
    print_speech("out is %s", out);
    Exit:{
        if(root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        return out;
    }
}

static int diapatch_speech_halo(int cmd, char *jstr)
{
    print_speech("enter speech dispatch!");
    char body[1024] = "";
    lua_call_fun("/tmp/lua/speech.lua", "speech_to_halo", "is>s", cmd, jstr, body);
    print_speech("get body %s", body);
    int len = strlen(body);
    int tmp = 0;
    FOREACH_LINE_OF_BUF(body, len, tmp){
        print_ser_cli("body now is %s", body);
        if(check_json_string(body)){
            msg_to_ser_cli(0, body);
        }
    }
    return 0;
}

int parse_speech_cmd(char *content)
{
    int cmd = 0;
    print_speech("enter speech dispatch!");
    char body[1024] = "";
    char tts[1024] = "";
    lua_call_fun("/tmp/lua/speech.lua", "speech_dispatch", "s>iss", content, &cmd, body, tts);
    print_speech("get body %d, %s\n %s", cmd, body, tts);
    if(*tts != '\0') {
        splayer_set_loop_mode(SPLAYER_LOOP_ONLY_ONE);
        sair_ai_tts_call(tts, strlen(tts));
    }

    diapatch_speech_villa(body);
    diapatch_speech_halo(cmd, body);
    return 0;
}
