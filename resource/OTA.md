#### 固件升级

##### 【描述】

固件升级主要用于修复设备bug和增加设备新功能。固件升级主要分两种，第一种是设备升级，第二种是MCU升级。

#### 设置设备固件信息

用户需要登录涂鸦开发者平台账号，上传固件包并配置，设备才会相应收到升级消息推送

操作说明：https://docs.tuya.com/cn/product/ota.html  

网关和子设备的固件类型均选择SDK固件

#### 子设备OTA说明

1. 流程交互图

```sequence
Title: 

participant 子设备
participant 应用层
participant GW_SDK
participant 涂鸦云
participant APP

涂鸦云->GW_SDK: 通知固件升级
GW_SDK->应用层: callback __dev_ug_inform_cb
应用层-->应用层: call tuya_iot_upgrade_dev_notify
Note over 应用层: 启动升级线程，关闭下发通道
GW_SDK->涂鸦云: 更新设备升级状态为升级中
涂鸦云-->GW_SDK: success
GW_SDK->涂鸦云: 下载固件包
涂鸦云-->GW_SDK:
GW_SDK->应用层: __get_file_data_cb
应用层-->GW_SDK: return OK
Note over GW_SDK: 继续下载...
GW_SDK->应用层: __upgrade_notify_cb下载完成
Note over GW_SDK: 删除升级线程，开启下发通道
应用层->子设备: 发送固件包
子设备-->应用层: return OK
应用层->GW_SDK:上报进度tuya_iot_dev_upgd_progress_rept
GW_SDK->涂鸦云: 推送mq进度
涂鸦云-->GW_SDK:ACK
GW_SDK-->应用层:return OK
涂鸦云->APP:
APP-->APP:显示进度
应用层-->应用层: 继续发送固件包
应用层->GW_SDK:发送完成tuya_iot_gw_subdevice_update
GW_SDK->涂鸦云:更新子设备固件版本号
涂鸦云-->涂鸦云: 更新设备升级状态为升级成功
涂鸦云->APP:推送子设备升级成功消息
```
2. 接口说明

- __dev_ug_inform_cb
```c
/*
 * @fcution 子设备升级入口，tuya_iot_gw_init中注册
 * @param dev_id, 子设备的dev_id
 * @param fw,固件包信息
 * @note, 应用层不能主动调用，sdk收到升级消息时，会回调
*/
VOID __dev_ug_inform_cb(IN CONST CHAR_T *dev_id,IN CONST FW_UG_S *fw)
{
    // 启动升级线程，注册 __get_file_data_cb & __upgrade_notify_cb回调
#if 0  // GW_SDK 上报进度
    OPERATE_RET op_ret = tuya_iot_upgrade_dev(dev_id, fw, __get_file_data_cb, __upgrade_notify_cb, user_param);
#else  // 关闭进度上报，用应用层上报
    OPERATE_RET op_ret = tuya_iot_upgrade_dev_notify(dev_id, fw, __get_file_data_cb, __upgrade_notify_cb, user_param,FALSE,0);
#endif 
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_upgrade_gw err:%d",op_ret);
    }
}
```

```c
/*
 * @fcution 上报升级进度
 * @param percent, 升级进度，范围(0,100)
 * @param devid, 子设备时，传入子设备的devid; 网关时，传入NULL
 * @note 子设备过程中，应用层主动调用
 * @note 网关和子设备升级共用此接口，通过devid参数区分
*/
OPERATE_RET tuya_iot_dev_upgd_progress_rept(IN CONST UINT_T percent, IN CONST CHAR_T *devid, IN CONST DEV_TYPE_T tp);
```

- tuya_iot_gw_subdevice_update
```c
/*
 * @fcution 更新子设备版本号
 * @param id, 子设备的dev_id
 * @param ver, 子设备固件版本号，支持格式 XX.XX.XX
 * @note 子设备升级完成后，主动调用
*/
#define tuya_iot_gw_subdevice_update(id, ver)
```

#### FAQ

1. 固件还未下载完成时，网关断网，GW_SDK处理的机制是怎么样的

GW_SDK会尝试重连https服务器，重连超时30s, 累积重连8次还是失败时，GW_SDK删除升级线程，
回调__upgrade_notify_cb通知应用层固件下载失败;

2. 网关下一个子设备正在升级，另一个共享家庭用户点击另一个子设备进行升级 

无法进行这种操作～
同一时间下网关只能进行一个子设备的升级，这种操作APP端会弹出提醒 "网关已有设备升级中，请稍后再试～"