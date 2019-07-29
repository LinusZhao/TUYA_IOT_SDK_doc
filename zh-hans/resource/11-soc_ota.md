## 网关固件升级

### 数据交互图

说明：
1. 下图中上报升级进度为tuya_sdk上报，也可通过tuya_iot_upgrade_gw_notify配置为应用层上报
```uml
@startuml
title device ota simple
participant TuyaApp
participant device
participant tuya_sdk
participant TuyaCloud

Note over TuyaCloud:用户上传配置固件包
Note over TuyaApp:用户启动升级
TuyaApp->tuya_sdk:通知升级
tuya_sdk->TuyaCloud:获取设备升级信息
TuyaCloud-->tuya_sdk:有升级任务
tuya_sdk->device:__soc_dev_rev_upgrade_info_cb
device->tuya_sdk: tuya_iot_upgrade_gw_notify，注册回调
Note over tuya_sdk:创建升级线程,dp下发通道关闭
tuya_sdk->TuyaCloud:更新设备升级状态为升级中
TuyaCloud-->tuya_sdk:success
loop Downloading
tuya_sdk->TuyaCloud:Download
TuyaCloud-->tuya_sdk:
tuya_sdk->device:__get_file_data_cb
device-->tuya_sdk:return OPRT_OK
tuya_sdk->TuyaApp:上报升级进度
end
Note over tuya_sdk:退出升级线程,dp下发通道打开
tuya_sdk->device:__upgrade_notify_cb通知下载结果
Note over device:开始安装固件
device->tuya_sdk:重启设备
tuya_sdk->TuyaCloud:更新固件版本号
Note over TuyaCloud:校验版本号
TuyaCloud-->TuyaApp:版本号匹配成功，更新设备升级状态为升级完成，推送升级成功消息
Note over TuyaApp:升级成功
@enduml
```

### 相关接口说明

#### __soc_dev_rev_upgrade_info_cb
```c
/***********************************************************
 * @Function:__soc_dev_rev_upgrade_info_cb
 * @Desc:   wifi设备升级入口
 * @Param:  fw,固件信息结构体
***********************************************************/
VOID __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("SOC Rev Upgrade Info");

    OPERATE_RET op_ret;

    // 如果不满足升级条件
#if 0
    op_ret = tuya_iot_refuse_upgrade(fw,tuya_iot_get_gw_id());
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_iot_refuse_upgrade op_ret:%d",op_ret);
        return ;
    }
    else{
        PR_DEBUG("tuya_iot_refuse_upgrade is ok");
        return;
    }
#endif

    FILE *p_upgrade_fd = fopen(SOC_OTA_FILE, "w+b");
    if(NULL == p_upgrade_fd){
        PR_ERR("open upgrade file fail. upgrade fail %s", SOC_OTA_FILE);
        return;
    }

#if 1  // tuya_sdk上报进度
    op_ret = tuya_iot_upgrade_gw_notify(fw, __get_file_data_cb, __upgrade_notify_cb, p_upgrade_fd,TRUE,0);
#else  // 关闭tuya_sdk进度上报，由应用层上报
    op_ret = tuya_iot_upgrade_gw_notify(fw, __get_file_data_cb, __upgrade_notify_cb, p_upgrade_fd,FALSE,0);
#endif    
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_upgrade_gw err:%d",op_ret);
        return;
    }
}
```

#### __get_file_data_cb
```c
// 固件下载中，tuya_sdk通知应用层处理下载包
STATIC OPERATE_RET __get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                      IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    FILE *p_upgrade_fd = (FILE *)pri_data;

    fwrite(data, 1, len, p_upgrade_fd);
    *remain_len = 0;

    return OPRT_OK;
}
```
#### __upgrade_notify_cb
```c
// 固件下载完成，tuya_sdk通知下载结果
STATIC VOID __upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fclose(p_upgrade_fd);

    if(download_result == 0) {
        PR_DEBUG("Upgrade File Download Success");
        // UserTODO
    }else {
        PR_ERR("Upgrade File Download Fail.ret = %d", download_result);
        // UserTODO
    }
}
```

#### tuya_iot_upgrade_gw_notify
```c
/***********************************************************
 * @Function:tuya_iot_upgrade_gw
 * @Desc:    注册回调，启动网关固件升级线程
 * @Param:   fw, 固件信息
 * @Param:   get_file_cb, 接收固件包回调
 * @Param:   upgrd_nofity_cb, 固件包下载结果通知回调
 * @Param:   pri_data, private param of get_file_cb && upgrd_nofity_cb
 * @Param:   notify,是否由tuya_sdk上报升级进度
 * @Param:   download_buf_size, 下载最大缓存，单位字节
 * @Return:  TRUE: success  FALSE: fail，
***********************************************************/
#define tuya_iot_upgrade_gw(fw, get_file_cb, upgrd_nofity_cb, pri_data) \
    tuya_iot_upgrade_gw_notify(fw, get_file_cb, upgrd_nofity_cb, pri_data, TRUE, 0)
OPERATE_RET tuya_iot_upgrade_gw_notify(IN CONST FW_UG_S *fw,
                                       IN CONST GET_FILE_DATA_CB get_file_cb,\
                                       IN CONST UPGRADE_NOTIFY_CB upgrd_nofity_cb,\
                                       IN CONST PVOID_T pri_data,\
                                       BOOL_T notify, UINT_T download_buf_size);

```

### 应用层上报升级进度

当需要应用层控制升级进度上报时，请先用tuya_iot_upgrade_gw_notify接口关闭tuya_sdk上报升级进度

#### tuya_iot_dev_upgd_progress_rept
```c
/***********************************************************
 * @Function:tuya_iot_dev_upgd_progress_rept
 * @Desc:   上报升级进度
 * @Param percent, 升级进度，范围(0,100)
 * @Param devid, 子设备时，传入子设备的devid; 网关时，传入NULL
 * @Return: TRUE: success  FALSE: fail，
 * @Note 子设备过程中，应用层主动调用
 * @Note 网关和子设备升级共用此接口，通过devid参数区分
***********************************************************/
OPERATE_RET tuya_iot_dev_upgd_progress_rept(IN CONST UINT_T percent, \
                                            IN CONST CHAR_T *devid,  \
                                            IN CONST DEV_TYPE_T tp);
```