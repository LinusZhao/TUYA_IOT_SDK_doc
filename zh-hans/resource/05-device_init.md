
## 网关初始化
tuya最新的sdk支持网关产品上定义dp点

网关产品key信息获取，请参考[创建产品](New_product.md)

### tuya_iot_gw_init
```c
#include "tuya_iot_base_api.h"
/***********************************************************
 * @Function:tuya_iot_gw_init
 * @Desc:   有线网关初始化接口
 * @Param:  cbs,    sdk用户回调
 * @Param:  gw_cbs, 子设备管理用户回调
 * @Param:  product_key,网关的产品key信息
 * @Param:  sw_ver,网关的固件版本 格式:xx.xx.xx (0<=x<=9)
 * @Param:  attr,网关属性数组
 * @Param:  attr_num,网关属性长度
 * @Return: OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_gw_init(IN CONST TY_IOT_CBS_S *cbs,IN CONST TY_IOT_GW_CBS_S *gw_cbs,\
                             IN CONST CHAR_T *product_key,IN CONST CHAR_T *sw_ver,\
                             IN CONST GW_ATTACH_ATTR_T *attr,IN CONST UINT_T attr_num);
```
接口使用说明：
```c
#define USER_DEV_IN_GW_SW_VER  "1.0.0"  // 网关内部通讯模块固件版本，用于OTA管理

TY_IOT_CBS_S iot_cbs = {
        __gw_dev_status_changed_cb,  // 网关sdk状态通知
        __gw_dev_rev_upgrade_info_cb,// 参考网关固件升级部分
        __gw_dev_restart_req_cb,     // 参考网关配网部分
        __gw_dev_obj_dp_cmd_cb,      // 参考业务功能说明
        __gw_dev_raw_dp_cmd_cb,      // 参考业务功能说明
        __gw_dev_dp_query_cb,        // 不常用功能
        __dev_ug_inform_cb,          // 参考子设备固件升级部分
        __dev_reset_cb,
#if defined(ENGINEER) && (ENGINEER==1) // 用于工程版sdk使用
        __gw_lan_ug_inform_cb,  
        __gw_set_channel_cb,
        __gw_get_channel_cb,  
        __gw_get_log_cb,
        __gw_sync_config_cb,
#endif
    };

TY_IOT_GW_CBS_S gw_cbs = {
        __gw_permit_add_dev_cb,
        __gw_dev_del_cb,
        __gw_dev_grp_infm_cb,
        __gw_dev_scene_infm_cb,
        __gw_bind_dev_inform_cb,
#if defined(ENGINEER) && (ENGINEER==1)
        __gw_sce_panel_cb,
#endif
    };

// 网关内部通讯模块属性，通讯类型+固件版本
GW_ATTACH_ATTR_T attr[] = {
    {GP_DEV_ZB, USER_DEV_IN_GW_SW_VER},
};

op_ret = tuya_iot_gw_init(&iot_cbs, &gw_cbs, GW_PRODUCT_KEY, USER_SW_VER, attr, \
                            sizeof(attr)/sizeof(GW_ATTACH_ATTR_T));
if(OPRT_OK != op_ret) {
    PR_ERR("tuya_iot_gw_init op_ret:%d",op_ret);
    return op_ret;
}
```

### 子设备管理回调
- __gw_permit_add_dev_cb，__gw_dev_del_cb，__gw_bind_dev_inform_cb
请参考：[子设备配网](sub_active.md)

- __gw_dev_grp_infm_cb 请参考：   [组操作](grp_infm.md)
- __gw_dev_scene_infm_cb 请参考： [场景管理](scene_infm.md)

### tuya_iot_wf_gw_init
```c
#include "tuya_iot_wifi_api.h"
/***********************************************************
 * @Function:tuya_iot_wf_gw_init
 * @Desc:   wifi网关初始化接口
 * @Param:  cfg,默认为GWCM_OLD
 * @Param:  start_mode, wifi网关支持配网模式种类设置，AP或者Smart
 * @Param:  cbs,    sdk用户回调
 * @Param:  gw_cbs, 子设备管理用户回调
 * @Param:  product_key,网关的产品key信息
 * @Param:  wf_sw_ver,网关的固件版本 格式:xx.xx.xx (0<=x<=9)
 * @Param:  attr,网关属性数组
 * @Param:  attr_num,网关属性长度
 * @Return: OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_wf_gw_init(IN CONST GW_WF_CFG_MTHD_SEL cfg, \
                                IN CONST GW_WF_START_MODE start_mode,\
                                IN CONST TY_IOT_CBS_S *cbs, IN CONST TY_IOT_GW_CBS_S *gw_cbs,
                                IN CONST CHAR_T *product_key, IN CONST CHAR_T *wf_sw_ver,
                                IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num);
```