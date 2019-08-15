## 用户需要实现的接口

说明：如下接口涉及wifi网卡操作，请根据设备系统实现，demo里的实现仅供参考，未必适配你的设备；
tuya_sdk未对wifi网卡驱动层做限制，比如没有要求必须支持ifconfig命令。

接口位置：tuya_iot_sdk/demo_soc_dev_wifi/tuya_iot_wifi_net.c文件中

AP配网和Smart配网tuya_sdk都会调用如下接口；

### hwl_wf_wk_mode_set
```c
/***********************************************************
 * @Function:hwl_wf_wk_mode_set
 * @Desc:   设置设备wifi网卡工作模式
 * @Param:  mode,
 * @Return: OPRT_OK: success  Other: fail
 * @Note:   tuya_sdk会调用，应用层需要根据设备实现
 * @Note:   用户需实现 station/ap/sniffer三种模式切换的实现，
 *          Smart配网模式下，需要支持切换sniffer模式
***********************************************************/
OPERATE_RET hwl_wf_wk_mode_set(IN CONST WF_WK_MD_E mode)
{
    // UserTODO
    switch (mode)
    {
        case WWM_LOWPOWER:
        {
            //关闭配网模式
            break;
        }
        case WWM_SNIFFER:
        {
            break;
        }
        case WWM_STATION:
        {
            break;
        }
        case WWM_SOFTAP:
        {
            break;
        }
        case WWM_STATIONAP:
        {//reserved
            break;
        }
        default:
        {
            break;
        }
    }
    PR_DEBUG("WIFI Set Mode:%d", mode);
    return OPRT_OK;
}
```
### hwl_wf_wk_mode_get
```c
/***********************************************************
 * @Function:hwl_wf_wk_mode_get
 * @Desc:   获取设备wifi网卡工作模式
 * @Param:  mode,
 * @Return: OPRT_OK: success  Other: fail
 * @Note:   tuya_sdk会调用，应用层需要根据设备实现
***********************************************************/
OPERATE_RET hwl_wf_wk_mode_get(OUT WF_WK_MD_E *mode)
{
    // UserTODO
    *mode = ? ;
    PR_DEBUG("WIFI Get Mode:%d", *mode);
    return OPRT_OK;
}
```

### hwl_wf_station_connect
```c
/***********************************************************
*  Function: hwl_wf_station_connect
*  Desc:     connect wifi with ssid and passwd.
*  Input:    ssid && passwd, The router's wifi name and password
*  Return:   OPRT_OK: success  Other: fail
*  Note:     1. tuya-sdk will call when it receives the distribution network of mobile phone to send message
             2. After equipment has distribution network successfully, the tuya-sdk after start-up called only once
***********************************************************/
OPERATE_RET hwl_wf_station_connect(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd)
{
    PR_DEBUG("STA Con AP ssid:%s passwd:%s", ssid, passwd);
    // UserTODO
    
#endif
    return OPRT_OK;
}
```

### hwl_wf_station_stat_get
```c
/***********************************************************
*  Function: hwl_wf_station_stat_get
*  Desc:     get wifi station work status
*  Output:   stat: the wifi station work status
*  Return:   OPRT_OK: success  Other: fail
*  Note:     User must tell tuya-sdk network status of the equipment with stat
***********************************************************/
OPERATE_RET hwl_wf_station_stat_get(OUT WF_STATION_STAT_E *stat)
{
    // UserTODO
    *stat = ?;
    
    return OPRT_OK;
}
```
