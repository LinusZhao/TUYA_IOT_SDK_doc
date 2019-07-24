### AP配网模式

#### 

#### AP配网数据链路图

```sequence
Title: 

participant 手机APP
participant Device
participant tuya_sdk
participant 涂鸦云

tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_SOFTAP)
tuya_sdk->Device: hwl_wf_ap_start
手机APP-->tuya_sdk: 连接设备热点，广播{ssid,passwd,token}
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