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

int main(int argc, char* argv[])
{

	char hex[40];
	bzero(hex, 40);
	int i;
	char *len = "149";
	char *ver = "1.0";
	char *cid = "881256532942016512";
	char *sid = "750837261197414400";
	char *key = "0CF9DE03AF1C46F9A970AB27120A91D2";
	

		//	unsigned char encrypt[] = "515306BE98143724C6481E5F2BB7F6951D57508322433334353927508372611974144001.0";
	// unsigned char encrypt[] = "454306BE98143724C6481E5F2BB7F6951D57508322433334353927508372611974144001.0";
		unsigned char encrypt[1024];
		bzero(encrypt, 1024);
		sprintf(encrypt, "%s%s%s%s%s", len, key, cid, sid, ver);
	unsigned char decrypt[16];

	MD5_CTX md5;

	MD5Init(&md5);
	MD5Update(&md5, encrypt, strlen((char *)encrypt));
	MD5Final(&md5, decrypt);

	//Md5加密后的32位结果
	printf("加密前:%s\n加密后16位:", encrypt);
	for (i = 4; i<12; i++)
	{
		printf("%02x", decrypt[i]);  
	}

	//Md5加密后的32位结果
	printf("\n加密前:%s\n加密后32位:", encrypt);
	for (i = 0; i<16; i++)
	{
		sprintf(hex, "%s%02X", hex, decrypt[i]);
		printf("%02x", decrypt[i]);  
	}
	
	printf("\n");
	printf("%s\n", hex);

	char hex_2;

	hex_2 = hex[0];
	for(i = 1; i < 32; i++){
		hex_2 = hex_2 ^ hex[i];
	}
	printf("%02X\n", hex_2);

	return 0;
}
