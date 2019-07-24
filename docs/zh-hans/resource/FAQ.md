
### 常见问题解答

1. 设备中途断开网络后，SDK没有调用hwl_wf_station_connect接口重连
    sdk启动后只调用一次，wpa_supplicant后台服务会自动不断的检测重连，所以linux上我们去掉了内部的检测机制。

2. 
