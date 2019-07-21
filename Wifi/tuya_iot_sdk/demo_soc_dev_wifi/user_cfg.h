/*
    user_cfg.h
    Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef _USER_CFG_H
#define _USER_CFG_H

#include "tuya_cloud_wifi_defs.h"
#include "uni_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  Please check the relevant interface WIFI to be compatible with your equipment system, 
    tuya-sdk does not do any restrictions on its implementation.*/
#define COMPILE_CHECK   // 接口实现检查

// 涂鸦云上的产品KEY，需登陆tuya.com创建产品分配唯一key
#define PRODUCT_KEY         "2y8fs1j3lk6wddio"
// SD本地配置存储路径，该路径必须对应一个可读写文件系统分区
#define CFG_STORAGE_PATH    "./"
// UUID和AUTHKEY用于涂鸦云设备的安全认证，每个设备所用key均为唯一
#define UUID                "tuya985d5f5255e0342b"
#define AUTHKEY             "xONNZA9yFvLZeEDQRh0CpaE2wAAKAZJT"
// OTA文件本地保存绝对路径，SDK会自动进行OTA管理
#define SOC_OTA_FILE        "/tmp/soc_upgrade.ota"
// wifi配网支持模式选择
#define WF_CFG_MODE_SELECT  (WF_START_AP_ONLY)   // 仅支持AP配网模式
#define LOG_LEVEL_SET       (LOG_LEVEL_DEBUG)
  
/*
    1. AP配网模式下，设备热点名称设置
    2. if AP_NAME is null ,then the default ssid is Smartlife_xxxx
    3. strlen(AP_NAME) <= 16
 */
#define AP_NAME             NULL   // 
/*
    1. AP配网模式下，设备热点密码设置
    2. if AP_NAME is null ,
    3. strlen(AP_PASSWD) <= 16
 */
#define AP_PASSWD           NULL   // 无密码

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif