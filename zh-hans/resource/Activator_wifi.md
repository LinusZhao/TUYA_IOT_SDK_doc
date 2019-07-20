### WiFi设备配网

#### 描述

WiFi设备配网主要有EZ模式和AP模式两种, EZ配网模式需要设备wifi网卡支持sniffer模式。

#### 配网模式设置

SDK初始化时调用tuya_iot_wf_soc_dev_init接口start_mode参数设置

- start_mode参数说明：
WF_START_AP_ONLY: 只支持ap配网模式
WF_START_SMART_ONLY: 只支持EZ配网模式
WF_START_AP_FIRST: 同时支持ap和ez两种模式，默认为ap模式
WF_START_SMART_FIRST: 同时支持ap和ez两种模式，默认为ez模式

```c
/*
* @param cfg wifi工作模式选择GWCM_OLD
* @param start_mode 配网模式选择
* @param product_key 涂鸦云上的产品KEY，需登陆iot.tuya.com创建产品分配唯一key
* @param wf_sw_ver 设备固件版本
*/
    tuya_iot_wf_soc_dev_init(cfg, start_mode,cbs,product_key,wf_sw_ver)
```

#### AP配网模式说明

SDK进入ap配网模式后接口调用流程图

```sequence
Title: 

participant APP
participant Device
participant SDK
participant 涂鸦云

SDK->Device: hwl_wf_wk_mode_set(WWM_SOFTAP)
SDK->Device: hwl_wf_ap_start
APP-->SDK: 连接设备热点，广播{ssid,passwd,token}
SDK->Device: hwl_wf_wk_mode_set(WWM_STATION)
SDK->Device: hwl_wf_station_connect(ssid,passwd)
Device-->SDK: return OPRT_OK
SDK->Device: hwl_wf_station_stat_get
Note over SDK: 每隔1s查询一次网络状态
Device-->SDK: WSS_GOT_IP
SDK -> 涂鸦云: 请求设备激活
涂鸦云 --> SDK: 设备激活成功
SDK->Device : __soc_dev_net_status_cb(STAT_CLOUD_CONN)\n通知设备配网成功
```




#### 设备重置

```c
/*
 * 设备重置接口，设备需要主动调用以重置设备
*/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_unactive(VOID);
```

配网模式设置为WF_START_AP_FIRST时，首次调用重置接口时，设备进入ap配网模式；再次调用进入SMART模式，再次调用进入AP模式，如此循环。
配网模式设置为WF_START_SMART_FIRST时，首次调用重置接口时，设备进入SMART配网模式；再次调用进入AP模式，再次调用进入SMART模式，如此循环。

- 1. 重置设备流程

```sequence
Title: 

participant Device
participant SDK

Device->SDK: call tuya_iot_wf_gw_unactive
SDK->Device: callback __soc_dev_reset_req_cb
Note over Device: 设备执行重启sdk后，进入配网模式

```

#### Smart配网模式说明

Smart配网需要网卡支持进入sniffer模式，捕获空气中的所有无线包，根据

```sequence
Title: Smart配网模式，tuya_sdk <-> 设备应用层数据链路

participant 手机APP
participant tuya_sdk
participant Device
participant Service

Note over APP: 连上路由器
Note over Device: Wifi灯快闪

APP->SDK: 获取token
SDK->Service: 获取token
Service-->SDK: 返回token
SDK-->APP: 返回token

APP->SDK: 开始配网 ssid/pwd/token
Note over SDK: 通过广播、组播循环发送ssid/pwd/token
Device->Device: 捕捉到ssid/password/token

Device->Service: 去激活设备
Service-->Device: 激活成功

Device-->SDK: 激活成功
SDK-->APP: 激活成功

```

```java
mTuyaActivator = TuyaHomeSdk.getActivatorInstance().newMultiActivator(new ActivatorBuilder()
.setSsid(ssid)
.setContext(context)
.setPassword(password)
.setActivatorModel(ActivatorModelEnum.TY_EZ)
.setTimeOut(CONFIG_TIME_OUT)
.setToken(token)
.setListener(this));
```

#### AP模式配网

```sequence
Title: AP 配网

participant APP
participant SDK
participant Device
participant Service

Note over Device: Wifi灯慢闪
APP->SDK: 获取token
SDK->Service: 获取token
Service-->SDK: 返回token
SDK-->APP: 返回token

Note over APP: 连上设备的热点

APP->SDK: 开始配网 ssid/pwd/token
SDK->Device: 发送配置信息 ssid/pwd/token
Note over Device: 自动关闭热点

Note over Device: 连上路由器WiFi

Device->Service: 去激活设备
Service-->Device: 激活成功

Device-->SDK: 激活成功
SDK-->APP: 激活成功

```

```java
mTuyaActivator = TuyaHomeSdk.getActivatorInstance().newActivator(new ActivatorBuilder()
.setSsid(ssid)
.setContext(context)
.setPassword(password)
.setActivatorModel(ActivatorModelEnum.TY_AP)
.setTimeOut(CONFIG_TIME_OUT)
.setToken(token)
.setListener(this));
```

#### 配网方法调用

```java
mTuyaActivator.start(); //开始配网

mTuyaActivator.stop(); //停止配网

mTuyaActivator.onDestroy(); //退出页面销毁一些缓存和监听
```

#### 配网结果回调

```
ITuyaSmartActivatorListener listener 配网回调接口，配网业务类需要实现该接口
```

```java
* @method onError(String errorCode,String errorMsg);
@param errorCode:
1001        网络错误
1002        配网设备激活接口调用失败，接口调用不成功
1003        配网设备激活失败，设备找不到。
1004        token 获取失败
1005        设备没有上线
1006        配网超时

@param errorMsg:
暂时没有数据，请参考errorCode。

* @method onActiveSuccess(DeviceBean deviceBean);
设备配网成功,且设备上线（手机可以直接控制），可以通过

* @method onStep(String step, Object o);
|@param step         |@param o
|device_find         |devId (String)
|device_bind_success |dev (DeviceBean)
【备注】
device_find 发现设备
device_bind_success 设备绑定成功，但还未上线，此时设备处于离线状态，无法控制设备。
```

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
