#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"

#include "tuya_cloud_wifi_defs.h"
#include "tuya_iot_wifi_api.h"
#include "wifi_hwl.h"
#include "user_cfg.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

// UserTODO
// SOC固件版本，用于OTA管理，格式必须为"XX.XX.XX"，其中XX必须为数字
#ifndef USER_SW_VER
#define USER_SW_VER         "1.2.3"
#endif

GW_WIFI_NW_STAT_E gw_wifi_nw_stat = STAT_STA_CONN;

STATIC VOID __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw);//SOC设备升级入口
STATIC VOID __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status);//SOC设备云端状态变更回调
STATIC VOID __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry);//SOC设备特定数据查询入口
STATIC VOID __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp);//SOC设备格式化指令数据下发入口
STATIC VOID __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp);//SOC设备透传指令数据下发入口
STATIC VOID __soc_dev_reset_req_cb(GW_RESET_TYPE_E type);//SOC设备进程重启请求入口
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat);//SOC外网状态变动回调

unsigned char buffer[] = {"eyJzaWQiOjIyMSwic2V0VmVyc2lvbiI6MSwidm9pY2VVcmwiOiJodHRwczovL3R1eWEtbWlkZGxld2FyZS1kYXRhLm9zcy1jbi1oYW5nemhvdS5hbGl5dW5jcy5jb20vc2lnbWF4LW9ubGluZS8yMS5wbmc/T1NTQWNjZXNzS2V5SWQ9TFRBSU43cDRhc3JNYXcyMSZFeHBpcmVzPTE1NjY1MjY0NTcmU2lnbmF0dXJlPUpidHVaQ2V6WVFwdDZINmtJMVVTN1Z6SWFmZyUzRCIsIm1kNSI6InVpdWlpdXVpIiwic2l6ZSI6NTQyNzJ9IA=="};

void *test_thread_fn(void *arg)
{

}
int main(void)
{
    printf("IOT SDK Version: %s \r\n", tuya_iot_get_sdk_info());
    printf("PRODUCT_KEY: %s \r\n", PRODUCT_KEY);
    printf("USER_SW_VER: %s \r\n", USER_SW_VER);
    printf("CFG_STORAGE_PATH: %s \r\n", CFG_STORAGE_PATH);
    printf("UUID: %s \r\n", UUID);
    printf("AUTHKEY: %s \r\n", AUTHKEY);

    OPERATE_RET op_ret = tuya_iot_init(CFG_STORAGE_PATH);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_init err:%d PATH:%s", op_ret, CFG_STORAGE_PATH);
        return -1;
    }
    PR_NOTICE("tuya_iot_init success");

    // 设置日志输出等级
    op_ret = SetLogManageAttr(LOG_LEVEL_SET);
    if(OPRT_OK != op_ret){
        PR_ERR("SetLogManageAttr op_ret is %d\n",op_ret);
        return -2;
    }
    PR_NOTICE("SetLogManageAttr success");

    WF_GW_PROD_INFO_S prod_info = {UUID, AUTHKEY, AP_NAME, AP_PASSWD};
    op_ret = tuya_iot_set_wf_gw_prod_info(&prod_info);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_set_wf_gw_prod_info err:%d", op_ret);
        return -3;
    }
    PR_NOTICE("tuya_iot_set_wf_gw_prod_info success");

    TY_IOT_CBS_S iot_cbs = {
        __soc_dev_status_changed_cb,
        __soc_dev_rev_upgrade_info_cb,
        __soc_dev_reset_req_cb,
        __soc_dev_obj_dp_cmd_cb,
        __soc_dev_raw_dp_cmd_cb,
        __soc_dev_dp_query_cb,
        NULL,
    };
    op_ret = tuya_iot_wf_soc_dev_init(GWCM_OLD, WF_CFG_MODE_SELECT, &iot_cbs, PRODUCT_KEY, USER_SW_VER);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init err:%d",op_ret);
        return -4;
    }
    PR_NOTICE("tuya_iot_wf_soc_dev_init success");

    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(__soc_dev_net_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb err:%d",op_ret);
        return -5;
    }
    PR_NOTICE("tuya_iot_reg_get_wf_nw_stat_cb success");

    while (1)
    {
        // for debug
        char buf[128] = {0};
        UINT8_T readLen = 0;
        if((readLen = read(STDIN_FILENO,buf,64)) < 0){
            PR_ERR("read %d",readLen);
            return -5;
        }
        PCHAR_T command = NULL;
        PCHAR_T p = buf;
        command = strsep(&p," ");
        if (!strcmp(command,"unactive")){
            op_ret = tuya_iot_wf_gw_unactive();
            if(OPRT_OK != op_ret) {
                PR_ERR("tuya_iot_wf_gw_unactive err:%d", op_ret);
                return -6;
            }
            PR_DEBUG("tuya_iot_wf_gw_unactive success");
        }
        else if(!strcmp(command,"token")){
            PCHAR_T token = strsep(&p," ");
            op_ret = tuya_iot_gw_wf_user_cfg("Tuya","12345678",token);
            if(OPRT_OK != op_ret) {
                PR_ERR("tuya_iot_gw_wf_user_cfg err:%d", op_ret);
                return -7;
            }
            PR_DEBUG("tuya_iot_gw_wf_user_cfg success");
        }
        // else if(!strcmp(command,"station_stat")){
        //     PCHAR_T stat = strsep(&p," ");
        //     StationStat = atoi(stat);
        //     PR_DEBUG("set StationStat is %d",StationStat);
        // }

        // test
        // if(gw_wifi_nw_stat == STAT_CLOUD_CONN){
        //     op_ret = tuya_iot_upload_route_buffer(11,buffer,sizeof(buffer));
        //     PR_ERR("tuya_iot_upload_route_buffer op_ret:%d", op_ret);
        // }
        // sleep(1);
    }

    return 0;
}

//SOC设备升级相关代码开始
STATIC VOID __upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fclose(p_upgrade_fd);

    if(download_result == 0) {
        PR_DEBUG("SOC Upgrade File Download Success");
        //User TODO

    }else {
        PR_ERR("SOC Upgrade File Download Fail.ret = %d", download_result);
    }
}

STATIC OPERATE_RET __get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                      IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
//    PR_DEBUG("Rev File Data");
//    PR_DEBUG("Total_len:%u", total_len);
//    PR_DEBUG("Offset:%u Len:%u", offset, len);
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fwrite(data, 1, len, p_upgrade_fd);
    *remain_len = 0;

    return OPRT_OK;
}

//SOC设备升级入口
VOID __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("SOC Rev Upgrade Info");
    PR_DEBUG("fw->tp:%d", fw->tp);
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->fw_hmac:%s", fw->fw_hmac);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%u", fw->file_size);

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
    op_ret = tuya_iot_upgrade_gw(fw, __get_file_data_cb, __upgrade_notify_cb, p_upgrade_fd);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_upgrade_gw err:%d",op_ret);
    }else
    {
        PR_NOTICE("tuya_iot_upgrade_gw start.");
    }
    
}
//升级相关代码结束

//SOC设备云端状态变更回调
VOID __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_DEBUG("SOC TUYA-Cloud Status:%d", status);
}

//SOC设备特定数据查询入口
VOID __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_DEBUG("SOC Rev DP Query Cmd");
    if(dp_qry->cid != NULL) PR_ERR("soc not have cid.%s", dp_qry->cid);

    if(dp_qry->cnt == 0) {
        PR_DEBUG("soc rev all dp query");
        //User TODO
    }else {
        PR_DEBUG("soc rev dp query cnt:%d", dp_qry->cnt);
        UINT_T index = 0;
        for(index = 0; index < dp_qry->cnt; index++) {
            PR_DEBUG("rev dp query:%d", dp_qry->dpid[index]);
            //User TODO
        }
    }
}

//SOC设备格式化指令数据下发入口
VOID __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_NOTICE("SOC Rev DP Obj Cmd t1:%d t2:%d CNT:%u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);
    if(dp->cid != NULL) PR_ERR("soc not have cid.%s", dp->cid);

    UINT_T index = 0;
    for(index = 0; index < dp->dps_cnt; index++)
    {
        CONST TY_OBJ_DP_S *p_dp_obj = dp->dps + index;
        PR_DEBUG("idx:%d dpid:%d type:%d ts:%u", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp);
        switch (p_dp_obj->type) {
        case PROP_BOOL:     { PR_DEBUG("bool value:%d", p_dp_obj->value.dp_bool); break;}
        case PROP_VALUE:    { PR_DEBUG("INT value:%d", p_dp_obj->value.dp_value); break;}
        case PROP_STR:      { PR_DEBUG("str value:%s", p_dp_obj->value.dp_str); break;}
        case PROP_ENUM:     { PR_DEBUG("enum value:%u", p_dp_obj->value.dp_enum); break;}
        case PROP_BITMAP:   { PR_DEBUG("bits value:0x%X", p_dp_obj->value.dp_bitmap); break;}
        default:            { PR_ERR("idx:%d dpid:%d type:%d ts:%u is invalid", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp); break;}
        }//end of switch
    }
    //User TODO

    //用户处理完成之后需要主动上报最新状态，这里简单起见，直接返回收到的数据，认为处理全部成功。
    OPERATE_RET op_ret = dev_report_dp_json_async(dp->cid,dp->dps,dp->dps_cnt);
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async op_ret:%d",op_ret);
    }
}

//SOC设备透传指令数据下发入口
VOID __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_NOTICE("SOC Rev DP Raw Cmd t1:%d t2:%d dpid:%d len:%u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);
    if(dp->cid != NULL) PR_ERR("soc not have cid.%s", dp->cid);

    //User TODO

    //用户处理完成之后需要主动上报最新状态，这里简单起见，直接返回收到的数据，认为处理全部成功。
    OPERATE_RET op_ret = dev_report_dp_raw_sync(dp->cid,dp->dpid,dp->data,dp->len,0);
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async op_ret:%d",op_ret);
    }
}

//SOC设备进程重启请求入口
VOID __soc_dev_reset_req_cb(GW_RESET_TYPE_E type)
{
    PR_DEBUG("SOC Rev Reset Req %d", type);
    GW_WF_NWC_FAST_STAT_T wf_nwc_fast_stat;
    OPERATE_RET op_ret = tuya_iot_wf_fast_get_nc_type(&wf_nwc_fast_stat);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_fast_get_nc_type op_ret:%d",op_ret);
    }
    if(wf_nwc_fast_stat == GWNS_FAST_UNCFG_SMC){
        PR_DEBUG("tuya-sdk is in Smart distribution mode");
    }
    else if(wf_nwc_fast_stat == GWNS_FAST_UNCFG_AP){
        PR_DEBUG("tuya-sdk is in AP distribution mode");
    }
    
    //User TODO
}

// SOC外网状态变动回调
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("network status:%d", stat);
    gw_wifi_nw_stat = stat;
}

