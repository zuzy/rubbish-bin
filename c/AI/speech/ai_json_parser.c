/*
 * Copyright (C) 2018 Actions
 * Author:lishiyuan
 */
#include "json/cJSON.h"
#include "ai_json_parser.h"

typedef struct
{
    int imgSize;
    char* artistPic_url;
    int id;
    char* img_url;
    char* artist;
    char* album;
    char* lrc_url;
    char* title;
    char* albumPic_url;
    int size;
    char* url;
    int duration;
    int lrcSize;
}song_datails_t;

static long int get_random_seed(void) 
{
    long int at = 0;    
    struct timespec now;    
    clock_gettime(CLOCK_MONOTONIC, &now);    
    at = now.tv_sec * 1000 + now.tv_nsec / 1000000;    
    return at; 
}


int ai_json_parser(char* json_string, abuffer_t* ask_abuffer, abuffer_t* answer_abuffer, abuffer_t* music_abuffer)
{
    int ret = 0;
	
    cJSON*root=cJSON_Parse(json_string); 
    if (root == NULL)
        return -1;
    	
    cJSON*result_item =cJSON_GetObjectItem(root, "result");

    if (result_item == NULL)
    {
        printf("[AI] : check result error, %s\n", json_string);  
        goto Exit;
    }

    cJSON*input_item =cJSON_GetObjectItem(result_item, "input");

    if (input_item == NULL)
    {
        printf("[AI] : check result error, %s\n", json_string);  
        goto Exit;
    }

    if (input_item->type == cJSON_String )
    {
        printf("[AI] input : %s \n",input_item->valuestring);	
        if (ask_abuffer ->size >= strlen(input_item->valuestring))	
        {
            memcpy(ask_abuffer->data, input_item->valuestring,  strlen(input_item->valuestring));  
            ret |=BIT_ASK_ABUFFER;			
        }
    }

    cJSON*sds_item =cJSON_GetObjectItem(result_item, "sds");

    if (sds_item == NULL)
    {
        printf("[AI] check result error, %s\n", json_string);  
        goto Exit;
    }

    cJSON*output_item =cJSON_GetObjectItem(sds_item, "output");

    if (output_item == NULL)
    {
        printf("[AI] check result error, %s\n", json_string);  
        goto Exit;
    }

    if (output_item->type == cJSON_String )
    {
        printf("[AI] output : %s \n",output_item->valuestring);

        if (answer_abuffer->size >= strlen(output_item->valuestring))	
        {
            memcpy(answer_abuffer->data, output_item->valuestring,  strlen(output_item->valuestring)); 
            ret |=BIT_ANSWER_ABUFFER;			
        }
    }


/*****************************************************************************
"data":
{
	"dbdata":
	[
		{
			"imgSize":"23363",
			"artistPic":"http://cdndown.dorylist.com/uploads/images/2013/06/22/111039556.jpg",
			"id":"4707",
			"img":"http://cdndown.dorylist.com/uploads/images/2013/06/22/111039556.jpg",
			"artist":"周杰伦",
			"album":"其 它",
			"lrcUrl":"http://cdndown.dorylist.com/uploads/lrc/4707.lrc",
			"title":"东风破",
			"albumPic":"",
			"size":"1764448",
			"url":"http://cdndown.dorylist.com/uploads/ogg/2016/04/27/022810157.ogg",
			"duration":0,"lrcSize":1919
		}
	]
},
*********************************************************************************/
    cJSON*data_item =cJSON_GetObjectItem(sds_item, "data");

    if (data_item == NULL)
    {
        goto Exit;
    }

    cJSON*dbdata_array =cJSON_GetObjectItem(data_item, "dbdata");

    if (dbdata_array == NULL)
    {
        goto Exit;
    }

    if(dbdata_array !=NULL)  
    {  
        int size=cJSON_GetArraySize(dbdata_array);  

        printf("[AI] allmusic count: %d\n",size);  

        if (size > 0)
        {
            int seed;
            cJSON* object;
            cJSON* item;		   

            seed = get_random_seed()% size;
            object=cJSON_GetArrayItem(dbdata_array,seed);  
  
            item=cJSON_GetObjectItem(object,"url");  
            if(item!=NULL && item->type == cJSON_String )
            {  
                printf("[AI] string is %s, url: %s \n",item->string,item->valuestring);

                if (music_abuffer->size >= strlen(item->valuestring))	
                {
                    memcpy(music_abuffer->data, item->valuestring,  strlen(item->valuestring)); 
                    ret |=BIT_MUSIC_ABUFFER;							
                }
            } 

            item=cJSON_GetObjectItem(object,"title");  
            if(item!=NULL && item->type == cJSON_String )
            {  
                printf("[AI] string is %s, title: %s \n",item->string,item->valuestring);  
	     } 		
        }	
    }

Exit:    
   cJSON_Delete(root);

   return ret;
}


