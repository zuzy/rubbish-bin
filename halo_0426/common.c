#include "halo_common.h"

#define HALO_VERSION    102
#define VERSION_CODE    1060302
#define VERSION_NAME    "1.6.3.2"
#define MODEL           "HOPE-Q3"
#define PLAYER_TYPE     2

int get_mac_string(char *mac){
#if 0    
    sprintf(max, "%02x:%02x:%02x:%02x:%02x:%02x", )
#else
    strcpy(mac, "12:23:34:45:56:67");
#endif
    return 0;
}

int get_server_port(){
    // port = 1234;
    return 1234;
}

int get_server_ip(char *ip){
    strcpy(ip, "192.168.31.189");
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