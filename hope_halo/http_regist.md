{"areaCode":"鄞州区","bgType":"master","bgVersion":"1.2.3","cityCode":"宁波市","comName":"HOPE","deviceAddress":"中国浙江省宁波市鄞州区扬帆路1301号","deviceCata":"A6","deviceDrive":"rk3188","deviceName":"HOPE-A6","deviceSN":"70368170428025","firmVersion":"rk3188-userdebug 4.4.4 KTU84Q eng.karl.20170323.171738.170401 test-keys-fq","macAddress":"44:2C:15:85:40:B9","osLanguage":"zh","parentId":753396045774098432,"playerType":"release","playerVersion":"1.0","provinceCode":"浙江省","softVersion":"4.4.4","upgradeChannel":"release-HOPE_A7"}

563-24*2

{
	"areaCode":"鄞州区",
	"bgType":"master",
	"bgVersion":"1.2.3",
	"cityCode":"宁波市",
	"comName":"HOPE",
	"deviceAddress":"中国浙江省宁波市鄞州区扬帆路1301号",
	"deviceCata":"A6",
	"deviceDrive":"rk3188",
	"deviceName":"HOPE-A6",
	"deviceSN":"70368170428025",
	"firmVersion":"rk3188-userdebug 4.4.4 KTU84Q eng.karl.20170323.171738.170401 test-keys-fq",
	"macAddress":"44:2C:15:85:40:B9",
	"osLanguage":"zh",
	"parentId":753396045774098432,
	"playerType":"release",
	"playerVersion":"1.0",
	"provinceCode":"浙江省",
	"softVersion":"4.4.4",
	"upgradeChannel":"release-HOPE_A7"
}



上层类别编号对照
本接口parentId表示当前设备的上层设备类别编号，相关对照请参考以下
类别编号	类别名称	类别说明
753396045774098432	影音娱乐	音乐背景，音响及与音乐相关的设备类别
消息体参数说明
参数名称	参数内容	参数类型

comName	设备厂商名称	设备厂商定义的格式（必填最多64个字）

deviceSN	设备SN号码	设备SN号码格式的字符串（必填最多32个字）
deviceName	设备名称	设备名称字符串（必填最多32个字）
deviceCata	设备类型	设备类型表定义的格式（必填最多32个字）

firmVersion	固件版本	固件版本字符串（建议填最多32个字）
softVersion	软件版本	软件版本字符串（建议填最多32个字）

macAddress	MAC地址	MAC地址字符串（建议填最多32个字）

playerType	播放器类型	播放器类型定义的格式（建议填最多32个字）
playerVersion	播放器版本	播放器版本（建议填最多32个字）

bgType	后台类型	后台类型（建议填最多32个字）
bgVersion	后台版本号	后台版本号（建议填最多32个字）

upgradeChannel	升级渠道号	升级渠道号（建议填最多32个字）

deviceDrive	设备驱动	设备驱动（建议填最多32个字）

osLanguage	系统语言	系统语言，zh:中文，en:英文（建议填最多32个字）

deviceLong	经度	经度格式字符串（有GPS模块建议填）
deviceLat	纬度	纬度格式字符串（有GPS模块建议填）
provinceCode	省份	省份名称（建议填最多32个字）
cityCode	城市	城市名称（建议填最多32个字）
areaCode	区县	区县名称（建议填最多32个字）
deviceAddress	详细定位地址	详细定位地址（建议填最多128个字）
返回内容示例
					
成功：
{
	"code": 100000,
	"message": "成功！",
	"object": {
		"authCode": "2B6DFE887320407D9F64B634415E4D01",
		"refrenceId": 754057826917978100
	}
}
失败：
{
	"code": 300004,
	"message": "请求参数签名校验码校验失败！"
}
					

