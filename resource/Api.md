### 接口实现说明

#### hwl_wf_station_connect

```c
/***********************************************************
*  Function: hwl_wf_station_connect
*  Desc:     connect wifi with ssid and passwd.
*  Input:    ssid && passwd, The router's wifi name and password
*  Return:   OPRT_OK: success  Other: fail
*  Note:     1. tuya-sdk will call when it receives the distribution network of mobile phone to send message.
             2. After equipment has distribution network successfully, the tuya-sdk after start-up called only once.
***********************************************************/
OPERATE_RET hwl_wf_station_connect(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd)
{
    PR_DEBUG("STA Con AP ssid:%s passwd:%s", ssid, passwd);
    // UserTODO
    return OPRT_OK;
}
```



#### hwl_wf_ap_start

```c
/***********************************************************
*  Function: hwl_wf_ap_start
*  Desc:     start a soft ap
*  Input:    cfg: the soft ap config
*  Return:   OPRT_OK: success  Other: fail
*  Note:     Used for AP distribution network mode, tuya-sdk will call
***********************************************************/
OPERATE_RET hwl_wf_ap_start(IN CONST WF_AP_CFG_IF_S *cfg)
{
    PR_DEBUG("Start AP SSID:%s", cfg->ssid);
    if(cfg->passwd != NULL){
        PR_DEBUG("PASSWD:%s", cfg->passwd);
    }
    // UserTODO

    return OPRT_OK;
}
```

#### hwl_wf_ap_stop

```c
/***********************************************************
*  Function: hwl_wf_ap_stop
*  Desc:     stop a soft ap
*  Return:   OPRT_OK: success  Other: fail
*  Note:     Used for AP distribution network mode, 
             tuya-sdk will call when it receives the distribution network of mobile phone to send message
***********************************************************/
OPERATE_RET hwl_wf_ap_stop(VOID)
{
    PR_DEBUG("Stop Ap Mode");
    // UserTODO

    return OPRT_OK;
}
```

#### hwl_wf_wk_mode_set
```c
/***********************************************************
*  Function: hwl_wf_wk_mode_set
*  Desc:     set wifi work mode
*  Input:    mode: wifi work mode
*  Return:   OPRT_OK: success  Other: fail
*  Note:     用户需实现 station/ap/sniffer三种模式切换的实现，Smart配网模式下，需要支持切换sniffer模式
*  Note:     配网过程需要切换工作模式时，tuya-sdk调用
***********************************************************/
OPERATE_RET hwl_wf_wk_mode_set(IN CONST WF_WK_MD_E mode)
{
    switch (mode)
    {
        case WWM_LOWPOWER:{
            //linux系统不关心低功耗
            break;
        }
        case WWM_SNIFFER:{
            // UserTODO
            break;
        }
        case WWM_STATION:{
            // UserTODO
            break;
        }
        case WWM_SOFTAP:{
            // UserTODO
            break;
        }
        case WWM_STATIONAP:{
            //reserved
            break;
        }
        default:{
            break;
        }
    }

    PR_DEBUG("WIFI Set Mode:%d", mode);
    return OPRT_OK;
}
```

#### hwl_wf_wk_mode_get
```c
/***********************************************************
*  Function: hwl_wf_wk_mode_get
*  Desc:     get wifi work mode
*  Output:   mode: wifi work mode
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET hwl_wf_wk_mode_get(OUT WF_WK_MD_E *mode)
{
    // UserTODO
    *mode = ?;
    return OPRT_OK;
}
```
#### hwl_wf_station_stat_get
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
    if(1){ // If successful device to connect the network
        *stat = WSS_GOT_IP;
    }
    else{
        *stat = WSS_CONN_FAIL;
    }
    return OPRT_OK;
}
```
#### hwl_wf_station_get_conn_ap_rssi
```c
/***********************************************************
*  Function: hwl_wf_station_get_conn_ap_rssi
*  Desc:     get wifi connect rssi
*  Output:   rssi: the return rssi.
*  Return:   OPRT_OK: success  Other: fail
*  Note:     用于手机app上设备信息界面->信号强度的显示
***********************************************************/
OPERATE_RET hwl_wf_station_get_conn_ap_rssi(OUT SCHAR_T *rssi)
{
    // UserTODO
    *rssi = ?;
    return OPRT_OK;
}
```

#### hwl_wf_station_disconnect
```c
/***********************************************************
*  Function: hwl_wf_station_disconnect
*  Desc:     disconnect wifi from connect ap
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET hwl_wf_station_disconnect(VOID)
{
    PR_DEBUG("Disconnect WIFI Conn");
    // UserTODO

    return OPRT_OK;
}
```

#### hwl_wf_get_cur_channel
```c
/***********************************************************
*  Function: hwl_wf_get_cur_channel
*  Desc:     get wifi interface work channel
*  Input:    chan: the channel wifi works
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET hwl_wf_get_cur_channel(OUT BYTE_T *chan)
{
#if (WF_CFG_MODE_SELECT != WF_START_AP_ONLY)
	// UserTODO
	*chan = ?
    PR_DEBUG("WIFI Get Curr Channel:%d", *chan);
#endif
    return OPRT_OK;
}
```
#### hwl_wf_set_cur_channel
```c
/***********************************************************
*  Function: hwl_wf_set_cur_channel
*  Desc:     set wifi interface work channel
*  Input:    chan: the channel to set
*  Return:   OPRT_OK: success  Other: fail
*  Note:     Smart distribution network mode, the tuya-sdk calls, switching channels to capture the message in the air
***********************************************************/
OPERATE_RET hwl_wf_set_cur_channel(IN CONST BYTE_T chan)
{
    PR_DEBUG("WIFI Set Channel:%d", chan);
#if (WF_CFG_MODE_SELECT != WF_START_AP_ONLY)
	// UserTODO
#endif
    return OPRT_OK;
}
```