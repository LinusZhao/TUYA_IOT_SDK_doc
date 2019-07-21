## WiFi设备配网

### wifi配网支持模式选择

WiFi设备配网主要有Smart模式和AP模式两种, Smart配网模式需要设备wifi网卡支持sniffer模式。

用户通过设置 tuya_iot_sdk/demo_soc_dev_wifi/user_cfg.h 文件中的宏WF_CFG_MODE_SELECT来选择：

WF_CFG_MODE_SELECT可选参数说明:

* WF_START_AP_ONLY: 只支持AP配网模式
* WF_START_SMART_ONLY: 只支持Smart配网模式
* WF_START_AP_FIRST: 同时支持AP和Smart两种模式，配网成功后重置进入AP模式
* WF_START_SMART_FIRST: 同时支持AP和Smart两种模式，配网成功后重置进入Smart模式

### 设备重置

#### tuya_iot_wf_gw_unactive

```c
/***********************************************************
*  Function: tuya_iot_wf_gw_unactive
*  Return:   OPRT_OK: success  Other: fail
*  Desc:     Remove device from tuya cloud
***********************************************************/
OPERATE_RET tuya_iot_wf_gw_unactive(VOID);
```
In the following cases, the user should call tuya_iot_wf_gw_unactive:
1. The device is not registered to tuya cloud, you need to switch to the distribution network mode AP or Smart.
2. The device has been registered to tuya cloud, you need to remove and re-registered to another user.

```c
/***********************************************************
*  Function: tuya_iot_wf_fast_get_nc_type
*  Input:    nc_type
*  Desc:     get wifi-config fast status from tuya-sdk
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_wf_fast_get_nc_type(GW_WF_NWC_FAST_STAT_T *nc_type);
//Using the instance
GW_WF_NWC_FAST_STAT_T wf_nwc_fast_stat;
OPERATE_RET op_ret = tuya_iot_wf_fast_get_nc_type(&wf_nwc_fast_stat);
if(OPRT_OK != op_ret) {
    PR_ERR("tuya_iot_wf_fast_get_nc_type op_ret:%d",op_ret);
}
if(wf_nwc_fast_stat == GWNS_FAST_UNCFG_SMC){
    // tuya-sdk is in Smart distribution mode
}
else if(wf_nwc_fast_stat == GWNS_FAST_UNCFG_AP){
    // tuya-sdk is in AP distribution mode
}
```

In the following cases, the user should call tuya_iot_wf_fast_get_nc_type:

1. 做配网LED闪烁，以表明设备处于AP配网模式还是Smart配网模式；

   Do the distribution network LED flashing to indicate whether the device is in AP or Smart mode.

配网模式切换说明：

1. If macro WF_CFG_MODE_SELECT is equal to WF_START_AP_ONLY, user call tuya_iot_wf_gw_unactive, tuya-sdk enter AP distribution mode.
2. If macro WF_CFG_MODE_SELECT is equal to WF_START_SMART_ONLY, user call tuya_iot_wf_gw_unactive, tuya-sdk enter Smart distribution mode.
3. If macro WF_CFG_MODE_SELECT is equal to WF_START_AP_FIRST, When the device is registered to tuya-cloud, the user first calls tuya_iot_wf_gw_unactive,  tuya-sdk enters AP distribution mode; the second time call it,  tuya-sdk enter Smart mode;such cyclic switching……
4. If macro WF_CFG_MODE_SELECT is equal to WF_START_SMART_FIRST, When the device is registered to tuya-cloud, the user first calls tuya_iot_wf_gw_unactive,  tuya-sdk enters Smart distribution mode; the second time call it,  tuya-sdk enter AP mode;such cyclic switching……

* 设备进入配网模式流程

```sequence
Title: 

participant Device
participant tuya_sdk

Device->tuya_sdk: call tuya_iot_wf_gw_unactive
tuya_sdk-->Device: return OPRT_OK
tuya_sdk->Device: callback __soc_dev_reset_req_cb
Device->tuya_sdk: restarts tuya_sdk process
tuya_sdk-->Device: enters the distribution mode.
Device->tuya_sdk: call tuya_iot_wf_fast_get_nc_type
tuya_sdk-->Device: return OPRT_OK
Device-->Device: 配网LED闪烁,可以添加设备
```

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

#### 配网结果回调



#### 配网问题汇总

配网超时，此时设备一直处于连不上网络的状态。有以下几种原因。

- 获取WiFi Ssid 错误，导致配网失败
安卓系统API里面获取到ssid，通常前后会有“”。
建议使用Tuya Sdk里面自带的WiFiUtil.getCurrentSSID()去获取
- WiFi密码包含空格
用户在输入密码的时候，由于输入法联想的功能很容易在密码中输入空格。建议密码输入的时候直接显示出来，另外在判断密码含有空格的时候，弹窗提醒用户。
- 用户不输入WiFi密码
用户在首次使用智能设备产品的过程中，很容易不输入密码就进行后续操作
建议判断密码输入为空且WiFi加密类型不为NONE时，弹窗提醒用户。
- 用户在AP配网时选择了设备的热点名称，用户首次使用智能产品的过程中，很容易出现此问题。
建议在判别AP配网时用户选择了设备的热点名称，弹窗提醒给用户。
- 获取WiFi的Ssid为"0x","\<unknown ssid\>"
目前发现在一些国产手机会出现此问题。并不是用户选择的WiFi名称。这是由于定位权限没开启导致的，建议用户可以手动输入WiFi的Ssid，或者给出弹窗提醒，让用户开启相应权限。

配网超时，此时设备已经激活成功。可能原因有：

- APP没有连接到正常的网络，导致无法获取设备的状态。