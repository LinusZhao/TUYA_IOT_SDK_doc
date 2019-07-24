### Smart配网模式

Smart配网需要网卡支持进入sniffer模式，捕获空气中的所有无线包以获取手机APP广播的ssid,passwd,token配网信息；
需要实现接口：
tuya_iot_sdk/demo_soc_dev_wifi/tuya_iot_wifi_net.c

Smart配网数据链路图

```sequence
Title: 
participant 手机APP
participant Device
participant tuya_sdk
participant 涂鸦云

tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_STATION)
tuya_sdk->Device: hwl_wf_all_ap_scan
tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_SNIFFER)
tuya_sdk->Device: create func_Sniffer thread
手机APP->手机APP:用户选择Smart模式添加设备
Note over 手机APP: 广播发送ssid/pwd/token
Device->Device:捕捉到合法的网络报文
Device-->tuya_sdk:s_pSnifferCall(rev_buf,len)
tuya_sdk->tuya_sdk:解析出ssid/passwd/token
tuya_sdk->Device: exit func_Sniffer thread
tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_STATION) 
tuya_sdk->Device: hwl_wf_station_connect(ssid,passwd)
Device-->tuya_sdk: return OPRT_OK
tuya_sdk->Device: hwl_wf_station_stat_get
Note over tuya_sdk: 每隔1s查询一次网络状态
Device-->tuya_sdk: WSS_GOT_IP
tuya_sdk->涂鸦云: 请求设备激活
涂鸦云-->tuya_sdk: 设备激活成功
tuya_sdk->Device: __soc_dev_net_status_cb(STAT_CLOUD_CONN)\n通知设备配网成功
手机APP->涂鸦云:刷新设备列表
涂鸦云-->手机APP:设备列表新增设备

```