### Smart配网模式

Smart配网需要网卡支持进入sniffer模式，捕获空气中的所有无线包以获取手机APP广播的ssid,passwd,token配网信息；

### Smart配网数据链路图

```sequence
Title: 
participant TuyaApp
participant Device
participant tuya_sdk
participant TuyaCloud

tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_STATION)
tuya_sdk->Device: hwl_wf_all_ap_scan
tuya_sdk->Device: hwl_wf_wk_mode_set(WWM_SNIFFER)
tuya_sdk->Device: create func_Sniffer thread
TuyaApp->TuyaApp:用户选择Smart模式添加设备
Note over TuyaApp: 广播发送ssid/pwd/token
Note over Device: 切换信道捕捉报文
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
tuya_sdk->TuyaCloud: 请求设备激活
TuyaCloud-->tuya_sdk: 设备激活成功
tuya_sdk->Device: __soc_dev_net_status_cb(STAT_CLOUD_CONN)\n通知设备配网成功
TuyaApp->TuyaCloud:刷新设备列表
TuyaCloud-->TuyaApp:设备列表新增设备

```

### 用户需实现的接口说明

说明：如下接口涉及wifi网卡操作，请根据设备系统实现，demo里的实现仅供参考，未必适配你的设备；tuya_sdk未对wifi网卡驱动层做限制，比如没有要求必须支持ifconfig命令。
接口位置：tuya_iot_sdk/demo_soc_dev_wifi/tuya_iot_wifi_net.c文件中

- [hwl_wf_wk_mode_set](wifi_api.md#hwlwfwkmodeset)
- [hwl_wf_wk_mode_get](wifi_api.md#hwlwfwkmodeget)
- [hwl_wf_station_connect](wifi_api.md#hwlwfstationconnect)
- [hwl_wf_station_stat_get](wifi_api.md#hwlwfstationstatget)

#### hwl_wf_all_ap_scan
```c
/***********************************************************
 * @Function:hwl_wf_all_ap_scan
 * @Desc:   扫描设备周围热点信息
 * @Return: OPRT_OK: success  Other: fail
 * @Note:   tuya_sdk的demo使用iwlist实现，如果网卡驱动支持，可不用修改
***********************************************************/
OPERATE_RET hwl_wf_all_ap_scan(OUT AP_IF_S **ap_ary,OUT UINT_T *num)
{
    // UserTODO
    return OPRT_OK;
}
```

#### hwl_wf_set_cur_channel
```c
/***********************************************************
 * @Function:hwl_wf_set_cur_channel
 * @Desc:   设置wifi工作信道
 * @Return: OPRT_OK: success  Other: fail
 * @Note:   
***********************************************************/
OPERATE_RET hwl_wf_set_cur_channel(IN CONST BYTE_T chan)
{
    PR_DEBUG("WIFI Set Channel:%d", chan);
    // UserTODO
    return OPRT_OK;
}

#### hwl_wf_get_cur_channel
```c
/***********************************************************
 * @Function:hwl_wf_get_cur_channel
 * @Desc:   获取wifi工作信道
 * @Return: OPRT_OK: success  Other: fail
 * @Note:   
***********************************************************/
OPERATE_RET hwl_wf_get_cur_channel(OUT BYTE_T *chan)
{
    // UserTODO
    *chan = ？；
    PR_DEBUG("WIFI Get Curr Channel:%d", *chan);
    return OPRT_OK;
}
```