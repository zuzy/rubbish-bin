#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "md5.h"

/*
ver	1.0
des	70
cid	750832243333435392
sid	750837261197414400
key	306BE98143724C6481E5F2BB7F6951D5
len	515
dat	{"areaCode":"鄞州区","bgType":"master","bgVersion":"1.2.3","cityCode":"宁波市","comName":"HOPE","deviceAddress":"中国浙江省宁波市鄞州区扬帆路1301号","deviceCata":"A6","deviceDrive":"rk3188","deviceName":"HOPE-A6","deviceSN":"70368170428025","firmVersion":"rk3188-userdebug 4.4.4 KTU84Q eng.karl.20170323.171738.170401 test-keys-fq","macAddress":"44:2C:15:85:40:B9","osLanguage":"zh","parentId":753396045774098432,"playerType":"release","playerVersion":"1.0","provinceCode":"浙江省","softVersion":"4.4.4","upgradeChannel":"release-HOPE_A7"}
*/



/*
ver=1.0
cid=881256532942016512
sid=750837261197414400
key=0CF9DE03AF1C46F9A970AB27120A91D2

{ "comName":"庆科电机",
 "deviceCata":"wifi",
 "deviceName":"wifi",
 "deviceSN":"B0F893332517",
 "macAddress":"B0:F8:93:33:25:17",
 "parentId":"829793393889529856"
}*/

// {"comName":"庆科电机","deviceCata":"wifi","deviceName":"wifi","deviceSN":"B0F893332517","macAddress":"B0:F8:93:33:25:17","parentId":"829793393889529856"}

#define TEST_HOST "http://192.168.2.9:8080"
#define OFFICIAL_HOST "http://open.nbhope.cn"

#define MAC_PATH        "/hopeApi/upgrade/mac"
#define DEV_REGIST_PATH	"/hopeApi/device/register"
#define STATUS_PATH     "/hopeApi/device/status"
#define JOIN_PATH       "/hopeApi/device/editJoin"

#define test_mac TEST_HOST MAC_PATH

static size_t _str_u8_len(char *str){
    char *ptr = str;
    unsigned char tmp = 0;
    size_t len = 0;
    int u8_len = 0;
    int num = 0;
    while(*ptr != '\0'){
        if(*ptr & 0x80){
            ++u8_len;
            #if 0
            num = 0;
            while(*ptr & 0x80){
                ++ptr;
                ++num;
            }
            printf("num is %d\n", num);
            #else
            num = 0;
            tmp = *ptr;
            // tmp <<= 1;
            while(tmp & 0x80){
                ++num;
                tmp <<= 1;
            }
            // printf("num is %d\n", num);
            ptr += num;
            #endif
        }else{
            ++ptr;
        }
        ++len;
    }
    // printf("strlen is %d, u8 is %d, final is %d\n", strlen(str), u8_len, len);
    return len;
}



int main(int argc, char* argv[])
{

	char hex[40] = "";
	int i;
	char *len = "149";
	char *ver = "1.0";
	char *cid = "881256532942016512";
	char *sid = "750837261197414400";
	char *key = "0CF9DE03AF1C46F9A970AB27120A91D2";
	

		//	unsigned char encrypt[] = "515306BE98143724C6481E5F2BB7F6951D57508322433334353927508372611974144001.0";
	// unsigned char encrypt[] = "454306BE98143724C6481E5F2BB7F6951D57508322433334353927508372611974144001.0";
    char buf[1024] = "";
    if(fgets(buf, sizeof(buf), stdin)){
        if(*buf) {
            char *p = strchr(buf, '\n');
            if(p)
                *p = '\0';
            printf("target string is\n%s\n", buf);
            int length = _str_u8_len(buf);

            printf("get utf8 length is %d\n", length);
            unsigned char encrypt[1024];
            bzero(encrypt, 1024);
            sprintf(encrypt, "%d%s%s%s%s", length, key, cid, sid, ver);
            unsigned char decrypt[16];

            MD5_CTX md5;

            MD5Init(&md5);
            MD5Update(&md5, encrypt, strlen((char *)encrypt));
            MD5Final(&md5, decrypt);

            //Md5加密后的16位结果
            // printf("加密前:%s\n加密后16位:", encrypt);
            // for (i = 4; i<12; i++)
            // {
            //     printf("%02x", decrypt[i]);  
            // }
            // printf("\n");
            //Md5加密后的32位结果
            printf("加密前:%s\n加密后32位:", encrypt);
            char tmp[3] = "";
            for (i = 0; i<16; i++)
            {
                snprintf(tmp, 3, "%.02X", decrypt[i]);
                strcat(hex, tmp);
                // printf("%.02x", decrypt[i]);  
            }
            
            // printf("\n");
            printf("%s\n", hex);

            char hex_2;

            hex_2 = hex[0];
            for(i = 1; i < 32; i++){
                hex_2 = hex_2 ^ hex[i];
            }
            printf("%02X\n", hex_2);
        }
    }

    printf(TEST_HOST MAC_PATH);

	return 0;
}
