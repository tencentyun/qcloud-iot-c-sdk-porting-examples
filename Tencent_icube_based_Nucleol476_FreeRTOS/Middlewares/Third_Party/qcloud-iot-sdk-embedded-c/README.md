#  腾讯物联网设备端 C-SDK编译指南

## 一、编译环境

重构后C-SDK主要使用cmake作为跨平台的编译工具，支持在Linux和Windows下的编译。

1. Linux编译环境

| 环境相关 | 版本        |
| -------- | ----------- |
| ubuntu   | 18.04.2 LTS |
| gcc      | 7.4.0       |
| cmake    | 3.10.2      |

2. Windows编译环境

| 环境相关                        | 版本                      |
| ------------------------------- | ------------------------- |
| windows 10                      | 1803 17134.829            |
| visual studio professional 2017 | 15.8.9（其中cmake为3.11） |

## 二、SDK源码结构

| 名称               | 说明                                                         |
| ------------------ | ------------------------------------------------------------ |
| CMakeLists.txt     | cmake编译描述文件                                            |
| CMakeSettings.json | visual studio下的配置文件                                    |
| build.sh           | Linux下使用GCC编译工具编译脚本                               |
| config.h.in        | cmake下生成config.h的模板                                    |
| device_info.json   | 设备信息文件，当不使用DEBUG_DEV_INFO_USED，将从该文件解析出设备信息 |
| docs               | 文档目录，SDK在不同平台下使用说明文档。                      |
| external_libs      | 第三方软件包组件                                             |
| samples            | 应用示例                                                     |
| include            | 提供外部调用的头文件                                         |
| platform           | 平台相关的源码文件，目前提供了包括AT模组、linux和windows下的实现。 |
| sdk_src            | SDK核心通信协议及服务                                        |
| tools              | SDK配套的脚本工具                                            |

详情如下：

```
├── CMakeLists.txt
├── CMakeSettings.json
├── LICENSE
├── README.md
├── build.sh
├── certs
│   ├── README.md
│   ├── support_cert.crt
│   └── support_private.key
├── config.h.in
├── device_info.json
├── docs
│   ├── 物联网开发平台.md
│   └── 物联网通信平台.md
├── external_libs
│   └── mbedtls
│       ├── CMakeLists.txt
│       ├── configs
│       │   ├── README.txt
│       │   ├── config-ccm-psk-tls1_2.h
│       │   ├── config-mini-tls1_1.h
│       │   ├── config-no-entropy.h
│       │   ├── config-picocoin.h
│       │   ├── config-suite-b.h
│       │   └── config-thread.h
│       ├── include
│       │   └── mbedtls
│       │       ├── aes.h
│       │       ├── aesni.h
│       │       ├── arc4.h
│       │       ├── asn1.h
│       │       ├── asn1write.h
│       │       ├── base64.h
│       │       ├── bignum.h
│       │       ├── blowfish.h
│       │       ├── bn_mul.h
│       │       ├── camellia.h
│       │       ├── ccm.h
│       │       ├── certs.h
│       │       ├── check_config.h
│       │       ├── cipher.h
│       │       ├── cipher_internal.h
│       │       ├── cmac.h
│       │       ├── compat-1.3.h
│       │       ├── config.h
│       │       ├── ctr_drbg.h
│       │       ├── debug.h
│       │       ├── des.h
│       │       ├── dhm.h
│       │       ├── ecdh.h
│       │       ├── ecdsa.h
│       │       ├── ecjpake.h
│       │       ├── ecp.h
│       │       ├── entropy.h
│       │       ├── entropy_poll.h
│       │       ├── error.h
│       │       ├── gcm.h
│       │       ├── havege.h
│       │       ├── hmac_drbg.h
│       │       ├── md.h
│       │       ├── md2.h
│       │       ├── md4.h
│       │       ├── md5.h
│       │       ├── md_internal.h
│       │       ├── memory_buffer_alloc.h
│       │       ├── net.h
│       │       ├── net_sockets.h
│       │       ├── oid.h
│       │       ├── padlock.h
│       │       ├── pem.h
│       │       ├── pk.h
│       │       ├── pk_internal.h
│       │       ├── pkcs11.h
│       │       ├── pkcs12.h
│       │       ├── pkcs5.h
│       │       ├── platform.h
│       │       ├── platform_time.h
│       │       ├── ripemd160.h
│       │       ├── rsa.h
│       │       ├── sha1.h
│       │       ├── sha256.h
│       │       ├── sha512.h
│       │       ├── ssl.h
│       │       ├── ssl_cache.h
│       │       ├── ssl_ciphersuites.h
│       │       ├── ssl_cookie.h
│       │       ├── ssl_internal.h
│       │       ├── ssl_ticket.h
│       │       ├── threading.h
│       │       ├── timing.h
│       │       ├── version.h
│       │       ├── x509.h
│       │       ├── x509_crl.h
│       │       ├── x509_crt.h
│       │       ├── x509_csr.h
│       │       └── xtea.h
│       └── library
│           ├── aes.c
│           ├── aesni.c
│           ├── arc4.c
│           ├── asn1parse.c
│           ├── asn1write.c
│           ├── base64.c
│           ├── bignum.c
│           ├── blowfish.c
│           ├── camellia.c
│           ├── ccm.c
│           ├── certs.c
│           ├── cipher.c
│           ├── cipher_wrap.c
│           ├── cmac.c
│           ├── ctr_drbg.c
│           ├── debug.c
│           ├── des.c
│           ├── dhm.c
│           ├── ecdh.c
│           ├── ecdsa.c
│           ├── ecjpake.c
│           ├── ecp.c
│           ├── ecp_curves.c
│           ├── entropy.c
│           ├── entropy_poll.c
│           ├── error.c
│           ├── gcm.c
│           ├── havege.c
│           ├── hmac_drbg.c
│           ├── md.c
│           ├── md2.c
│           ├── md4.c
│           ├── md5.c
│           ├── md_wrap.c
│           ├── memory_buffer_alloc.c
│           ├── net_sockets.c
│           ├── oid.c
│           ├── padlock.c
│           ├── pem.c
│           ├── pk.c
│           ├── pk_wrap.c
│           ├── pkcs11.c
│           ├── pkcs12.c
│           ├── pkcs5.c
│           ├── pkparse.c
│           ├── pkwrite.c
│           ├── platform.c
│           ├── ripemd160.c
│           ├── rsa.c
│           ├── sha1.c
│           ├── sha256.c
│           ├── sha512.c
│           ├── ssl_cache.c
│           ├── ssl_ciphersuites.c
│           ├── ssl_cli.c
│           ├── ssl_cookie.c
│           ├── ssl_srv.c
│           ├── ssl_ticket.c
│           ├── ssl_tls.c
│           ├── threading.c
│           ├── timing.c
│           ├── version.c
│           ├── version_features.c
│           ├── x509.c
│           ├── x509_create.c
│           ├── x509_crl.c
│           ├── x509_crt.c
│           ├── x509_csr.c
│           ├── x509write_crt.c
│           ├── x509write_csr.c
│           └── xtea.c
├── include
│   ├── config.h
│   ├── exports
│   │   ├── qcloud_iot_export_coap.h
│   │   ├── qcloud_iot_export_dynreg.h
│   │   ├── qcloud_iot_export_error.h
│   │   ├── qcloud_iot_export_event.h
│   │   ├── qcloud_iot_export_gateway.h
│   │   ├── qcloud_iot_export_log.h
│   │   ├── qcloud_iot_export_mqtt.h
│   │   ├── qcloud_iot_export_ota.h
│   │   ├── qcloud_iot_export_shadow.h
│   │   └── qcloud_iot_export_system.h
│   ├── lite-list.h
│   ├── lite-utils.h
│   ├── platform.h
│   ├── qcloud_iot_export.h
│   └── qcloud_iot_import.h
├── platform
│   ├── CMakeLists.txt
│   ├── at_device
│   │   └── esp8266
│   │       ├── at_device_esp8266.c
│   │       └── at_device_esp8266.h
│   ├── os
│   │   ├── linux
│   │   │   ├── HAL_Device_linux.c
│   │   │   ├── HAL_OS_linux.c
│   │   │   ├── HAL_TCP_linux.c
│   │   │   ├── HAL_Timer_linux.c
│   │   │   ├── HAL_UART_linux.c
│   │   │   └── HAL_UDP_linux.c
│   │   └── windows
│   │       ├── HAL_Device_win.c
│   │       ├── HAL_OS_win.c
│   │       ├── HAL_TCP_win.c
│   │       ├── HAL_Timer_win.c
│   │       ├── HAL_UART_win.c
│   │       └── HAL_UDP_win.c
│   └── tls
│       └── mbedtls
│           ├── HAL_DTLS_mbedtls.c
│           └── HAL_TLS_mbedtls.c
├── samples
│   ├── CMakeLists.txt
│   ├── coap
│   │   └── coap_sample.c
│   ├── data_template
│   │   ├── data_config.c
│   │   ├── data_template_sample.c
│   │   └── events_config.c
│   ├── dynamic_reg_dev
│   │   └── dynamic_reg_dev_sample.c
│   ├── event
│   │   ├── event_sample.c
│   │   └── events_config.c
│   ├── gateway
│   │   └── gateway_sample.c
│   ├── mqtt
│   │   ├── mqtt_sample.c
│   │   └── multi_thread_mqtt_sample.c
│   ├── ota
│   │   ├── ota_coap_sample.c
│   │   └── ota_mqtt_sample.c
│   ├── scenarized
│   │   ├── aircond_shadow_sample.c
│   │   ├── aircond_shadow_sample_v2.c
│   │   ├── door_coap_sample.c
│   │   ├── door_mqtt_sample.c
│   │   └── light_data_template_sample.c
│   └── shadow
│       └── shadow_sample.c
├── sdk_src
│   ├── CMakeLists.txt
│   ├── internal_inc
│   │   ├── at_client.h
│   │   ├── at_ringbuff.h
│   │   ├── at_socket_inf.h
│   │   ├── at_uart_hal.h
│   │   ├── at_utils.h
│   │   ├── ca.h
│   │   ├── coap_client.h
│   │   ├── coap_client_net.h
│   │   ├── device.h
│   │   ├── gateway_common.h
│   │   ├── json_parser.h
│   │   ├── log_upload.h
│   │   ├── mqtt_client.h
│   │   ├── mqtt_client_net.h
│   │   ├── network_interface.h
│   │   ├── ota_client.h
│   │   ├── ota_fetch.h
│   │   ├── ota_lib.h
│   │   ├── shadow_client.h
│   │   ├── shadow_client_common.h
│   │   ├── shadow_client_json.h
│   │   ├── utils_aes.h
│   │   ├── utils_base64.h
│   │   ├── utils_hmac.h
│   │   ├── utils_httpc.h
│   │   ├── utils_list.h
│   │   ├── utils_md5.h
│   │   ├── utils_param_check.h
│   │   ├── utils_sha1.h
│   │   └── utils_timer.h
│   ├── network
│   │   ├── at_socket
│   │   │   ├── at_client.c
│   │   │   ├── at_ringbuff.c
│   │   │   ├── at_socket_inf.c
│   │   │   ├── at_utils.c
│   │   │   └── network_at_tcp.c
│   │   ├── network_interface.c
│   │   ├── socket
│   │   │   └── network_socket.c
│   │   └── tls
│   │       └── network_tls.c
│   ├── protocol
│   │   ├── coap
│   │   │   ├── coap_client.c
│   │   │   ├── coap_client_auth.c
│   │   │   ├── coap_client_common.c
│   │   │   ├── coap_client_deserialize.c
│   │   │   ├── coap_client_message.c
│   │   │   ├── coap_client_net.c
│   │   │   └── coap_client_serialize.c
│   │   ├── http
│   │   │   └── utils_httpc.c
│   │   └── mqtt
│   │       ├── mqtt_client.c
│   │       ├── mqtt_client_common.c
│   │       ├── mqtt_client_connect.c
│   │       ├── mqtt_client_net.c
│   │       ├── mqtt_client_publish.c
│   │       ├── mqtt_client_subscribe.c
│   │       ├── mqtt_client_unsubscribe.c
│   │       └── mqtt_client_yield.c
│   ├── services
│   │   ├── dynreg
│   │   │   └── dynreg.c
│   │   ├── event
│   │   │   └── qcloud_iot_event.c
│   │   ├── gateway
│   │   │   ├── gateway_api.c
│   │   │   └── gateway_common.c
│   │   ├── log
│   │   │   ├── log_mqtt.c
│   │   │   └── log_upload.c
│   │   ├── ota
│   │   │   ├── ota_client.c
│   │   │   ├── ota_coap.c
│   │   │   ├── ota_fetch.c
│   │   │   ├── ota_lib.c
│   │   │   └── ota_mqtt.c
│   │   ├── shadow
│   │   │   ├── shadow_client.c
│   │   │   ├── shadow_client_common.c
│   │   │   ├── shadow_client_json.c
│   │   │   └── shadow_client_manager.c
│   │   └── system
│   │       └── system_mqtt.c
│   └── utils
│       ├── ca.c
│       ├── device.c
│       ├── json_parser.c
│       ├── json_token.c
│       ├── qcloud_iot_log.c
│       ├── string_utils.c
│       ├── utils_aes.c
│       ├── utils_base64.c
│       ├── utils_hmac.c
│       ├── utils_list.c
│       ├── utils_md5.c
│       ├── utils_sha1.c
│       └── utils_timer.c
└── tools
    └── convert.sh
```

## 三、SDK编译

### 3.1 添加设备信息

如果选择使用DEBUG_DEV_INFO_USED，则在`platform/linux/HAL_Device_linux.c`中修改设备信息；反之，则在`device_info.json`文件中修改，此种方式下更改设备信息不需重新编译sdk。

### 3.2 修改配置选项

| 名称                             | 有效值        | 说明                                                         |
| :------------------------------- | ------------- | ------------------------------------------------------------ |
| BUILD_TYPE                       | release/debug | release：不启用IOT_DEBUG信息，根据配置将源码文件抽离到release目录下 debug：启用IOT_DEBUG信息 |
| EXTRACT_SRC                      | ON/OFF        | 代码抽离                                                     |
| COMPILE_TOOLS                    | gcc/MSVC      | 目前支持gcc和msvc，其中msvc依赖于visual studio               |
| PLATFORM                         | linux/windows | linux下使用gcc widows下使用msvc-x86                          |
| FEATURE_MQTT_COMM_ENABLED        | ON/OFF        | MQTT通道总开关                                               |
| FEATURE_MQTT_DEVICE_SHADOW       | ON/OFF        | 设备影子总开关                                               |
| FEATURE_COAP_COMM_ENABLED        | ON/OFF        | CoAP通道总开关                                               |
| FEATURE_GATEWAY_ENABLED          | ON/OFF        | 网关功能总开关                                               |
| FEATURE_OTA_COMM_ENABLED         | ON/OFF        | OTA固件升级总开关                                            |
| FEATURE_OTA_SIGNAL_CHANNEL       | MQTT/COAP     | OTA信令通道类型                                              |
| FEATURE_AUTH_MODE                | KEY/CERT      | 接入认证方式                                                 |
| FEATURE_AUTH_WITH_NOTLS          | ON/OFF        | TLS使能                                                      |
| FEATURE_DEV_DYN_REG_ENABLED      | ON/OFF        | 设备动态注册开关                                             |
| FEATURE_LOG_UPLOAD_ENABLED       | ON/OFF        | 日志上报开关                                                 |
| FEATURE_EVENT_POST_ENABLED       | ON/OFF        | 事件上报开关                                                 |
| FEATURE_DEBUG_DEV_INFO_USED      | ON/OFF        | 设备信息获取开关                                             |
| FEATURE_SYSTEM_COMM_ENABLED      | ON/OFF        | 获取后台时间开关                                             |
| FEATURE_AT_TCP_ENABLED           | ON/OFF        | AT模组TCP功能开关                                            |
| FEATURE_AT_UART_RECV_IRQ         | ON/OFF        | AT模组中断接受功能开关                                       |
| FEATURE_AT_OS_USED               | ON/OFF        | AT模组多线程功能开关                                         |
| FEATURE_AT_DEBUG                 | ON/OFF        | AT模组调试功能开关                                           |
| FEATURE_MULTITHREAD_TEST_ENABLED | ON/OFF        | 是否编译Linux多线程测试例程                                  |

```cmake
# 编译类型: release/debug
set(BUILD_TYPE "release")

# 代码抽离，ON表示根据配置抽取源码到ouput目录
set(EXTRACT_SRC ON)

# 编译工具链
#set(COMPILE_TOOLS "gcc") 
#set(PLATFORM 	  "linux")
 
set(COMPILE_TOOLS "MSVC") 
set(PLATFORM 	  "windows") 

# 是否打开MQTT通道的总开关
set(FEATURE_MQTT_COMM_ENABLED ON)

# 是否打开设备影子的总开关
set(FEATURE_MQTT_DEVICE_SHADOW ON)

# 是否打开CoAP通道的总开关
set(FEATURE_COAP_COMM_ENABLED ON)

# 是否打开MQTT通道网关功能
set(FEATURE_GATEWAY_ENABLED ON)

# 是否打开OTA固件升级总开关
set(FEATURE_OTA_COMM_ENABLED ON)

# OTA信令通道类型：MQTT/COAP
set(FEATURE_OTA_SIGNAL_CHANNEL "MQTT")

# MQTT/CoAP接入认证方式，使用证书认证：CERT；使用密钥认证：KEY
set(FEATURE_AUTH_MODE "KEY")

# 接入认证是否不使用TLS，证书方式必须选择使用TLS，密钥认证可选择不使用TLS
set(FEATURE_AUTH_WITH_NOTLS ON)

# 是否使能设备动态注册
set(FEATURE_DEV_DYN_REG_ENABLED ON)

# 是否打开日志上报云端功能
set(FEATURE_LOG_UPLOAD_ENABLED ON)

# 是否打开事件上报功能
set(FEATURE_EVENT_POST_ENABLED ON)

# 是否打开代码中获取设备信息功能，OFF时将从device_info.json中读取设备信息
set(FEATURE_DEBUG_DEV_INFO_USED ON)

# 是否打开获取iot后台时间功能
set(FEATURE_SYSTEM_COMM_ENABLED ON)

# 是否打开AT模组TCP功能
set(FEATURE_AT_TCP_ENABLED OFF)

# 是否打开AT模组中断接收功能
set(FEATURE_AT_UART_RECV_IRQ ON)

# 是否打开AT模组多线程功能
set(FEATURE_AT_OS_USED ON)

# 是否打开AT模组调试功能
set(FEATURE_AT_DEBUG ON)

# 是否编译Linux多线程测试例程
set(FEATURE_MULTITHREAD_TEST_ENABLED ON)
```

配置选项之间存在依赖关系，当依赖选项的值为有效值时，部分配置选项才有效，主要如下：

| 名称                             | 依赖选项                                                | 有效值       |
| :------------------------------- | ------------------------------------------------------- | ------------ |
| FEATURE_MQTT_DEVICE_SHADOW       | FEATURE_MQTT_COMM_ENABLED                               | ON           |
| FEATURE_GATEWAY_ENABLED          | FEATURE_MQTT_COMM_ENABLED                               |              |
| FEATURE_OTA_SIGNAL_CHANNEL(MQTT) | FEATURE_OTA_COMM_ENABLED<br />FEATURE_MQTT_COMM_ENABLED | ON<br />ON   |
| FEATURE_OTA_SIGNAL_CHANNEL(COAP) | FEATURE_OTA_COMM_ENABLED<br />FEATURE_COAP_COMM_ENABLED | ON<br />ON   |
| FEATURE_AUTH_WITH_NOTLS          | FEATURE_AUTH_MODE                                       | KEY必须为OFF |
| FEATURE_AT_UART_RECV_IRQ         | FEATURE_AT_TCP_ENABLED                                  | ON           |
| FEATURE_AT_OS_USED               | FEATURE_AT_TCP_ENABLED                                  | ON           |
| FEATURE_AT_DEBUG                 | FEATURE_AT_TCP_ENABLED                                  | ON           |

### 3.3 编译实例与运行

#### 3.3.1 linux

1. cmake下载与安装

   下载：https://cmake.org/download/

   安装：https://gitlab.kitware.com/cmake/cmake

2. 配置修改

   ```cmake
   set(COMPILE_TOOLS "gcc") 
   set(PLATFORM 	  "linux")
   ```

3. 执行脚本编译
   编译库和示例
   ```sh
   ./cmake_build.sh 
   ```
   只编译示例
   ```sh
   ./cmake_build.sh sample
   ```

4. 运行示例

   示例位于`output/bin`文件夹中，比如运行mqtt_sample示例，输入`./output/bin/mqtt_sample`即可。

4. 抽取源码

   将EXTRACT_SRC设置为ON，可根据配置抽取需要的源文件到`output/src`文件夹中。

#### 3.3.2 windows

1. visual studio下载与安装

   visual studio下载与安装：https://visualstudio.microsoft.com/zh-hans/vs/

   cmake功能由visual studio提供支持，务必勾选CMake tools for Windows。

   ![](https://main.qcloudimg.com/raw/d03ec40177d954e0725e1e08b6506b9c.png)

2. 配置修改

   ```cmake
   set(COMPILE_TOOLS "MSVC") 
   set(PLATFORM 	  "windows") 
   ```

3. 运行visual studio打开项目所在文件夹

   ![](https://main.qcloudimg.com/raw/7e81b6d12f37fa45969ec3713ff047e2.jpg)

4. 双击根目录的CMakeLsits.txt，在红框处选择当前文档，运行编译

   ![](https://main.qcloudimg.com/raw/1a19a6417c17873e463ab068ae8c9e64.jpg)

5. 运行示例

   示例位于`output\bin`文件夹中，可在CMD下输入相应的可执行文件运行。

6. 抽取源码

   将EXTRACT_SRC设置为ON，可根据配置抽取需要的源文件到output文件夹中。

### 3.4 注意事项

1. 代码抽取目前只抽取sdk_src和platform中的源文件，头文件默认都加入。
2. 由于Windows和Linux上文件格式问题，默认发布为windows格式，在linux下的build.sh中会调用tools文件夹中的convert.sh用作格式转换，转换一次后可以手动在build.sh中注释conver.sh的调用以加快编译速度。
3. Windows平台上AT底层串口驱动尚未完成，即AT模组目前应用于Linux平台下。
4. 目前Linux上AT的IRQ使用线程方式模拟，做示例时补充。
5. Windows上多线程示例尚未完成。
