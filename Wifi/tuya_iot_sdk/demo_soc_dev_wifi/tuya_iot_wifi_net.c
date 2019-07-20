#include "wifi_hwl.h"
#include "uni_log.h"

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COMPLIER_CHECK 
// WIFI设备的名称
//#define WLAN_DEV    "wlp3s0"
#define WLAN_DEV    "ens33"

// WIFI芯片是否是MTK7601
//#define WIFI_CHIP_7601

WF_WK_MD_E WkMode = WWM_SOFTAP;
WF_STATION_STAT_E StationStat = WSS_CONN_SUCCESS;

// 使用指定SSID和PASSWD连接WIFI
OPERATE_RET hwl_wf_station_connect(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd)
{
    PR_DEBUG("STA Con AP ssid:%s passwd:%s", ssid, passwd);

    //最多阻塞10s
    sleep(2);
    // UserTODO
    StationStat = WSS_GOT_IP;
    return OPRT_OK;
}


static void exec_cmd(char *pCmd)
{
    PR_DEBUG("Exec Cmd:%s", pCmd);
    system(pCmd);
}

/***********************************************************
*  Function: 搜索附近无线网络的信息，
/***********************************************************
*  Function: hwl_wf_all_ap_scan
*  Desc:     scan current environment and obtain all the ap
*            infos in current environment
*  Output:   ap_ary: current ap info array
*  Output:   num   : the num of ar_ary
*  Return:   OPRT_OK: success  Other: fail
typedef struct
{
    BYTE_T channel;                 // AP channel
    SCHAR_T rssi;                   // AP rssi
    BYTE_T bssid[6];                // AP bssid
    BYTE_T ssid[WIFI_SSID_LEN+1];   // AP ssid array
    BYTE_T s_len;                   // AP ssid len
}AP_IF_S;
*  Return: OPERATE_RET
***********************************************************/
#define MAX_AP_SEARCH 30
OPERATE_RET hwl_wf_all_ap_scan(OUT AP_IF_S **ap_ary,OUT UINT_T *num)
{
    static AP_IF_S s_aps[MAX_AP_SEARCH];
    memset(s_aps, 0, sizeof(s_aps));
    *ap_ary = s_aps;
    *num = 0;

    FILE *pp = popen("iwlist "WLAN_DEV" scan", "r");
    if(pp == NULL)
        return OPRT_COM_ERROR;

    char tmp[256] = {0};
    memset(tmp, 0, sizeof(tmp));

    PR_DEBUG("WIFI Scan AP Begin");
    int recordIdx = -1;
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        /* 首先找BSSID	  以此为基准 */
        char *pBSSIDStart = strstr(tmp, " - Address: ");
        if(pBSSIDStart != NULL)
        {
            recordIdx++;
            if(recordIdx >= MAX_AP_SEARCH)
            {
                printf(" Reach Max Record \r\n");
                recordIdx--;
                break;
            }

            BYTE_T *pTmp = s_aps[recordIdx].bssid;
            int x1,x2,x3,x4,x5,x6;

            sscanf(pBSSIDStart + strlen(" - Address: "), "%x:%x:%x:%x:%x:%x",&x1,&x2,&x3,&x4,&x5,&x6);
            pTmp[0] = x1 & 0xFF;
            pTmp[1] = x2 & 0xFF;
            pTmp[2] = x3 & 0xFF;
            pTmp[3] = x4 & 0xFF;
            pTmp[4] = x5 & 0xFF;
            pTmp[5] = x6 & 0xFF;

            goto ReadNext;
        }else
        {
            if(recordIdx < 0)
            {/* 只有找到第一个BSSID 才能继续读下去 */
                goto ReadNext;
            }
        }

        {
            /* 查找signal  */
            char *pSIGNALStart = strstr(tmp, "Quality=");
            if(pSIGNALStart != NULL)
            {
                int x = 0;
                int y = 0;
                sscanf(pSIGNALStart + strlen("Quality=") , "%d/%d ",&x,&y);
                s_aps[recordIdx].rssi = x * 100/ (y+1);
                goto ReadNext;
            }
        }

        {
            /* 查找channel	*/
            char *pCHANNELStart = strstr(tmp, "(Channel ");
            if(pCHANNELStart != NULL)
            {
                int x = 0;
                sscanf(pCHANNELStart + strlen("(Channel "), "%d)", &x);
                s_aps[recordIdx].channel = x;
                goto ReadNext;
            }
        }

        {
            /* 查找ssid  */
            char *pSSIDStart = strstr(tmp, "ESSID:\"");
            if(pSSIDStart != NULL)
            {
                sscanf(pSSIDStart + strlen("ESSID:\""), "%s", s_aps[recordIdx].ssid);
                s_aps[recordIdx].s_len = strlen(s_aps[recordIdx].ssid);
                if(s_aps[recordIdx].s_len != 0)
                {
                    s_aps[recordIdx].ssid[s_aps[recordIdx].s_len - 1] = 0;
                    s_aps[recordIdx].s_len--;
                }
                goto ReadNext;
            }
        }

 ReadNext:
        memset(tmp, 0, sizeof(tmp));
    }

    pclose(pp);
    *num = recordIdx + 1;
    #if 1
        *num = 2;
        BYTE_T *pTmp = s_aps[0].bssid;
        pTmp[0] = 0xC8;
        pTmp[1] = 0x94;
        pTmp[2] = 0xBB;
        pTmp[3] = 0x77;
        pTmp[4] = 0x90;
        pTmp[5] = 0x8C;
        sprintf(s_aps[0].ssid,"%s","Doraemon3");
        s_aps[0].channel = 1;
        s_aps[0].rssi = -30;
        s_aps[0].s_len = strlen(s_aps[0].ssid);

        pTmp = s_aps[1].bssid;
        pTmp[0] = 0xF4;
        pTmp[1] = 0xEC;
        pTmp[2] = 0x38;
        pTmp[3] = 0x4F;
        pTmp[4] = 0xD7;
        pTmp[5] = 0x04;
        sprintf(s_aps[0].ssid,"%s","OpenWrt-45");
        s_aps[1].channel = 11;
        s_aps[1].rssi = -80;
        s_aps[1].s_len = strlen(s_aps[1].ssid);
    #endif
    int index = 0;
    for(index = 0; index < *num; index++)
    {

        PR_DEBUG("index:%d channel:%d bssid:%02X-%02X-%02X-%02X-%02X-%02X RSSI:%d SSID:%s",
               index, s_aps[index].channel, s_aps[index].bssid[0],  s_aps[index].bssid[1],  s_aps[index].bssid[2],  s_aps[index].bssid[3],
                s_aps[index].bssid[4],  s_aps[index].bssid[5], s_aps[index].rssi, s_aps[index].ssid);
    }
    PR_DEBUG("WIFI Scan AP End");

    return OPRT_OK;
}

// 获取特定SSID的信息
OPERATE_RET hwl_wf_assign_ap_scan(IN CONST CHAR_T *ssid,OUT AP_IF_S **ap)
{
    /* 这里简单处理，扫描所有ap后进行查找 */
    PR_TRACE_ENTER();
    PR_DEBUG("ssid:%s",ssid);
    AP_IF_S *pTotalAp = NULL;
    UINT_T tatalNum = 0;
    int index = 0;
    hwl_wf_all_ap_scan(&pTotalAp, &tatalNum);
    *ap = NULL;
    for(index = 0; index < tatalNum; index++)
    {
        if(memcmp(pTotalAp[index].ssid, ssid, pTotalAp[index].s_len) == 0)
        {
            *ap = pTotalAp + index;
            break;
        }
    }
    PR_TRACE_LEAVE();
    return OPRT_OK;
}

// 释放内存
OPERATE_RET hwl_wf_release_ap(IN AP_IF_S *ap)
{// s_aps为静态变量，无需释放
    return OPRT_OK;
}

static int s_curr_channel = 1;
/***********************************************************
*  Function: 设置WIFI的工作信道
*  Desc:     set wifi interface work channel
*  Input:    chan: the channel to set
*  Return:   OPRT_OK: success  Other: fail
*  Note:    Smart配网模式时，SDK获取周围wifi信道信息后调用，循环切换信道
***********************************************************/
OPERATE_RET hwl_wf_set_cur_channel(IN CONST BYTE_T chan)
{
    PR_DEBUG("WIFI Set Channel:%d", chan);
    // UserTODO Start
    {
        // #error "please check hwl_wf_set_cur_channel"
        // char tmpCmd[100] = {0};
        // snprintf(tmpCmd, 100, "iwconfig %s channel %d", WLAN_DEV, chan);
        // exec_cmd(tmpCmd);
    }
    // UserTODO End
    s_curr_channel = chan;

#ifdef WIFI_CHIP_7601
    tuya_linux_wf_wk_mode_set(WWM_SNIFFER);
#endif
    return OPRT_OK;
}
/***********************************************************
*  Function: 获取WIFI的工作信道
*  Desc:     get wifi interface work channel
*  Input:    chan: the channel wifi works
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET hwl_wf_get_cur_channel(OUT BYTE_T *chan)
{
    // UserTODO Start
    {
        // #error "please check hwl_wf_get_cur_channel"
    // 如下为参考实现，请根据设备系统检查～
        FILE *pp = popen("iwlist "WLAN_DEV" channel", "r");

        if(pp == NULL)
            return OPRT_COM_ERROR;

        char tmp[128] = {0};
        memset(tmp, 0, sizeof(tmp));
        while (fgets(tmp, sizeof(tmp), pp) != NULL)
        {
            char *pIPStart = strstr(tmp, " (Channel ");
            if(pIPStart != NULL)
                break;
        }

        /* 查找channel	*/
        char *pCHANNELStart = strstr(tmp, " (Channel ");
        if(pCHANNELStart != NULL)
        {
            int x = 0;
            sscanf(pCHANNELStart + strlen(" (Channel "), "%d", &x);
            *chan = x;
        }else
        {
            *chan = s_curr_channel;
        }
        pclose(pp);
    }
    // UserTODO End

    PR_DEBUG("WIFI Get Curr Channel:%d", *chan);
    return OPRT_OK;
}

/*
 *  Function: func_Sniffer 抓包线程实现
 *  Note:     Smart配网模式时，tuya_sdk自动调用，无需应用层调用
 *  Return:   NULL
 */
#pragma pack(1)
/* http://www.radiotap.org/  */
typedef struct {
    BYTE_T it_version;
    BYTE_T it_pad;
    USHORT_T it_len;
    UINT_T it_present;
}ieee80211_radiotap_header;
#pragma pack()

static volatile SNIFFER_CALLBACK s_pSnifferCall = NULL;
static volatile int s_enable_sniffer = 0;
static void * func_Sniffer(void *pReserved)
{
    PR_DEBUG("Sniffer Thread Create");

    int sock = socket(PF_PACKET, SOCK_RAW, htons(0x03));//ETH_P_ALL
    if(sock < 0)
    {
        printf("Sniffer Socket Alloc Fails %d \r\n", sock);
        perror("Sniffer Socket Alloc");
        return (void *)0;
    }

    {/* 强制绑定到WLAN_DEV 上。后续可以考虑去掉 */
        struct ifreq ifr;
        memset(&ifr, 0x00, sizeof(ifr));
        strncpy(ifr.ifr_name, WLAN_DEV , strlen(WLAN_DEV) + 1);
        setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    }

    #define MAX_REV_BUFFER 512
    BYTE_T rev_buffer[MAX_REV_BUFFER];

    int skipLen = 26;/* radiotap 默认长度为26 */

    while((s_pSnifferCall != NULL) && (s_enable_sniffer == 1))
    {
        int rev_num = recvfrom(sock, rev_buffer, MAX_REV_BUFFER, 0, NULL, NULL);
        ieee80211_radiotap_header *pHeader = (ieee80211_radiotap_header *)rev_buffer;
        skipLen = pHeader->it_len;

#ifdef WIFI_CHIP_7601
        skipLen = 144;
#endif
        if(skipLen >= MAX_REV_BUFFER)
        {/* 有出现过header全ff的情况，这里直接丢掉这个包 */
            continue;
        }
#if 0
        {
            printf("skipLen:%d ", skipLen);
            int index = 0;
            for(index = 0; index < 180; index++)
            {
                printf("%02X-", rev_buffer[index]);
            }
            printf("\r\n");
        }
#endif
        /* wifi recv from packages from the air, and send these 
         packages to tuya-sdk with hwl_wf_sniffer_set of callback <cb> */
        if(rev_num > skipLen)
        {
            s_pSnifferCall(rev_buffer + skipLen, rev_num - skipLen);
        }
        PR_DEBUG("s_enable_sniffer");
    }

     s_pSnifferCall = NULL;

    close(sock);

    PR_DEBUG("Sniffer Proc Finish");
    return (void *)0;
}

/*
 *  Function: hwl_wf_sniffer_set 设置WIFI的sniffer抓包状态
 *  Param:    en,开启或结束抓包线程
 *  Param:    cb,if wifi sniffer mode is enabled, wifi recv from
 *            packages from the air, and user shoud send these
 *            packages to tuya-sdk with callback <cb>
 *  Note:     Smart配网模式时，tuya_sdk自动调用，应用层不可调用
 *  Return:   OPERATE_RET
 */
static pthread_t sniffer_thId; // 抓包线程ID
OPERATE_RET hwl_wf_sniffer_set(IN CONST BOOL_T en,IN CONST SNIFFER_CALLBACK cb)
{
    PR_DEBUG("cb:%s",cb);
    if(en == TRUE)
    {
        PR_DEBUG("Enable Sniffer");
        hwl_wf_wk_mode_set(WWM_SNIFFER);
        s_pSnifferCall = cb;
        s_enable_sniffer = 1;
        pthread_create(&sniffer_thId, NULL, func_Sniffer, NULL);
    }else
    {
        PR_DEBUG("Disable Sniffer");
        s_enable_sniffer = 0;
        pthread_join(sniffer_thId, NULL);
        hwl_wf_wk_mode_set(WWM_STATION);
    }
    return OPRT_OK;
}

/*  
 *  Function: hwl_wf_get_ip 获取WIFI的IP地址
 *  Param:    wf,网卡工作模式，station or ap
 *  Param:    ip,user should send ip to tuya_sdk by this param
 *  Note:     
 *  Return:   OPERATE_RET
 */ 
OPERATE_RET hwl_wf_get_ip(IN CONST WF_IF_E wf,OUT NW_IP_S *ip)
{
    FILE *pp = popen("ifconfig "WLAN_DEV, "r");
    if(pp == NULL){
        PR_ERR("hwl_wf_get_ip");
        return OPRT_COM_ERROR;
    }
        
    char tmp[256];
    memset(tmp, 0, sizeof(tmp));
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        char *pIPStart = strstr(tmp, "inet ");
        if(pIPStart != NULL)
        {/* 目前都是一行里面包含ip gw mask 直接跳出  */
            break;
        }
    }
    /* 查找ip  */
    char *pIPStart = strstr(tmp, "inet ");
    if(pIPStart != NULL)
        sscanf(pIPStart + strlen("inet "), "%s", ip->ip);
    /* 查找gw  */
    char *pGWStart = strstr(tmp, "broadcast ");
    if(pGWStart != NULL)
        sscanf(pGWStart + strlen("broadcast "), "%s", ip->gw);
    /* 查找mask */
    char *pMASKStart = strstr(tmp, "netmask ");
    if(pMASKStart != NULL)
        sscanf(pMASKStart + strlen("netmask "), "%s", ip->mask);

    pclose(pp);

    PR_DEBUG("WIFI Get IP:%s", ip->ip);
    return OPRT_OK;
}
 
/*  
 *  Function: hwl_wf_get_mac 获取WIFI的MAC地址
 *  Param:    wf,网卡工作模式，station or ap
 *  Param:    mac,user should send mac to tuya_sdk by this param
 *  Note:     此mac地址不是涂鸦智能手机app上设备信息显示的mac
 *  Return:   OPERATE_RET
 */
OPERATE_RET hwl_wf_get_mac(IN CONST WF_IF_E wf,INOUT NW_MAC_S *mac)
{
    FILE *pp = popen("ifconfig "WLAN_DEV, "r");
    if(pp == NULL)
       return OPRT_COM_ERROR;

    char tmp[256];
    memset(tmp, 0, sizeof(tmp));
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        char *pMACStart = strstr(tmp, "ether ");
        if(pMACStart != NULL)
        {
            int x1,x2,x3,x4,x5,x6;
            sscanf(pMACStart + strlen("ether "), "%x:%x:%x:%x:%x:%x",&x1,&x2,&x3,&x4,&x5,&x6);
            mac->mac[0] = x1 & 0xFF;
            mac->mac[1] = x2 & 0xFF;
            mac->mac[2] = x3 & 0xFF;
            mac->mac[3] = x4 & 0xFF;
            mac->mac[4] = x5 & 0xFF;
            mac->mac[5] = x6 & 0xFF;

            break;
        }
    }
    pclose(pp);

    PR_DEBUG("WIFI Get MAC %02X-%02X-%02X-%02X-%02X-%02X", mac->mac[0],mac->mac[1],mac->mac[2],mac->mac[3],mac->mac[4],mac->mac[5]);
    return OPRT_OK;
}

// 当前无需实现
OPERATE_RET hwl_wf_set_mac(IN CONST WF_IF_E wf,IN CONST NW_MAC_S *mac)
{
    return OPRT_OK;
}

/*  
 *  Function: hwl_wf_wk_mode_set 设置当前WIFI工作模式
 *  Param:    mode,WIFI工作模式，
 *  Note:     配网过程需要切换工作模式时，tuya_sdk调用
 *  Return:   OPERATE_RET
 */
OPERATE_RET hwl_wf_wk_mode_set(IN CONST WF_WK_MD_E mode)
{
    WkMode = mode;
    PR_DEBUG("WIFI Set Mode:%d", mode);
    return OPRT_OK;
}

/*  
 *  Function: hwl_wf_wk_mode_get 获取当前WIFI工作模式
 *  Param:    mode,WIFI工作模式，user should send wifi work mode to tuya_sdk by this param
 *  Note:     需要获取WIFI工作模式时，tuya_sdk调用
 *  Return:   OPERATE_RET
 */
OPERATE_RET hwl_wf_wk_mode_get(OUT WF_WK_MD_E *mode)
{
    *mode = WkMode;
    PR_DEBUG("WIFI Get Mode:%d", *mode);
    return OPRT_OK;
}

// 断开当前WIFI网络的连接
OPERATE_RET hwl_wf_station_disconnect(VOID)
{
    PR_DEBUG("Disconnect WIFI Conn");
    // UserTODO
    return OPRT_OK;
}

// 获取当前WIFI联网的RSSI
OPERATE_RET hwl_wf_station_get_conn_ap_rssi(OUT SCHAR_T *rssi)
{
    *rssi = 99;

    PR_DEBUG("Get Conn AP RSSI:%d", *rssi);
    return OPRT_OK;
}

// 获取当前WIFI联网状态
OPERATE_RET hwl_wf_station_stat_get(OUT WF_STATION_STAT_E *stat)
{
    *stat = StationStat;
    // UserTODO

    // PR_DEBUG("Curr WIFI Stat:%d", *stat);
    return OPRT_OK;
}

// AP配网模式下开启热点
OPERATE_RET hwl_wf_ap_start(IN CONST WF_AP_CFG_IF_S *cfg)
{
    PR_DEBUG("Start AP SSID:%s", cfg->ssid);
    // UserTODO
    return OPRT_OK;
}

// AP配网模式下停止热点
OPERATE_RET hwl_wf_ap_stop(VOID)
{
    PR_DEBUG("Stop Ap Mode");
    // UserTODO
    return OPRT_OK;
}

OPERATE_RET hwl_wf_set_country_code(IN CONST CHAR_T *p_country_code)
{
    PR_DEBUG("Set Country Code:%s", p_country_code);

    return OPRT_OK;
}


OPERATE_RET hwl_lowpower_enable(VOID)
{
    PR_DEBUG("Enable Low Power ..");

    return OPRT_OK;
}

OPERATE_RET hwl_lowpower_disable(VOID)
{
    PR_DEBUG("Disble Low Power ..");
    return OPRT_OK;
}

