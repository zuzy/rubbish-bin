#include <string.h>
#include "applib.h"
#include "net/wifi_manager.h"
#include "net/config.h"
#include "act_debug.h"
#include "common.h"
#ifndef off64_t
typedef unsigned long long off64_t;
#endif
#include "ghttp/ghttp.h" 
#include "cJSON.h"




int get_mac_string(char *mac){
    wifi_info_t wifi_info;
    wifi_get_wifi_info(&wifi_info);
    print_zyf("mac is %s", wifi_info.mac_address);
    strcpy(mac, wifi_info.mac_address);
    // get_mac(mac, MAX_MACHINE_NAME);
    // sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", wifi_info.mac_address[0], wifi_info.mac_address[1], wifi_info.mac_address[2], wifi_info.mac_address[3], wifi_info.mac_address[4], wifi_info.mac_address[5]);
    // print_zyf("mac now is %s", mac);
    return 0;
}

int get_server_port(){
    return PORT;
}

int get_server_ip(char *ip){
    wifi_info_t wifi_info;
    wifi_get_wifi_info(&wifi_info);
    strcpy(ip, wifi_info.ip_address);
    return 0;
}

int get_dev_info(pub_dev_info_t *devinfo){
    devinfo->version_code = VERSION_CODE;
    strcpy(devinfo->version_string, VERSION_NAME);
    devinfo->halo_version = HALO_VERSION;
    strcpy(devinfo->model, MODEL);
    get_mac_string(devinfo->hope_id);
    get_server_ip(devinfo->hope_ip);
    devinfo->hope_port = get_server_port();
    devinfo->player_type = PLAYER_TYPE;
}

static int _http_interrupt(void *param){
    return 0;
}

int get_location(char *province, char *city, char *longitude, char *latitude, char *area){
    ghttp_request *req = NULL;
    ghttp_status status;
    int status_code;
    req = ghttp_request_new(_http_interrupt, NULL);
    ghttp_set_uri(req, "http://freegeoip.net/json/");
    ghttp_set_type(req, ghttp_type_get);
    ghttp_prepare(req);
    status = ghttp_connect(req, 0ll, 0ll);
    if((status != ghttp_done) && (status != ghttp_error_no_support_range)){
        print_zyf("get failed");
        status_code = -1;
        goto EXIT;
    }
    status_code = ghttp_status_code(req);
    if(status_code != 200){
        print_zyf("get location error %d", status_code);
        goto EXIT;
    }
    char *body = (char*)malloc(1024);
    int ret = ghttp_read(req, body, 1024);
    print_zyf("<get> %d ret: %d\r\n body: %s", ret, status_code, body);
    cJSON *root = cJSON_Parse(body);
    if(!root){
        goto Exit_body;
    }
    cJSON *province_item = cJSON_GetObjectItem(root, "region_name");
    if(province_item){
        print_zyf("province %s", province_item->valuestring);
        strcpy(province, province_item->valuestring);
    }
    cJSON *city_item = cJSON_GetObjectItem(root, "city");
    if(city_item){
        print_zyf("city %s", city_item->valuestring);
        strcpy(city, city_item->valuestring);
    }
    cJSON *long_item = cJSON_GetObjectItem(root, "longitude");
    if(long_item){
        print_zyf("longitude %f", long_item->valuedouble);
        sprintf(longitude, "%f", long_item->valuedouble);
    }
    cJSON *lati_item = cJSON_GetObjectItem(root, "latitude");
    if(lati_item){
        print_zyf("latitude %f", lati_item->valuedouble);
        sprintf(latitude, "%f", lati_item->valuedouble);
    }
    cJSON *area_item = cJSON_GetObjectItem(root, "zip_code");
    if(area_item){
        print_zyf("zip code %s", area_item->valuestring);
        strcpy(area, area_item->valuestring);
    }
Exit_body:
    if(root)cJSON_Delete(root);
    if(body)free(body);
EXIT:    
    ghttp_request_destroy(req);
    return status_code;
}