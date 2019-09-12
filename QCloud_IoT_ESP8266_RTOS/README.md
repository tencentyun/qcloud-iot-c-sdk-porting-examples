# 文档说明
本文档介绍如何将腾讯云物联 **IoT C-SDK** 移植到乐鑫 **ESP8266 RTOS** 平台，并提供可运行的demo

本项目适用于**ESP8266 Launcher**开发板，如果使用其他板子，需要修改main/board_ops.h里面的GPIO配置

### 1. 获取 ESP8266_RTOS_SDK 以及编译器
在当前目录下获取ESP8266 RTOS SDK 3.1
```
git clone --single-branch -b release/v3.1 https://github.com/espressif/ESP8266_RTOS_SDK.git
```

编译toolchain请参考ESP8266_RTOS_SDK/README.md，推荐使用
* [Linux(64) GCC 5.2.0](https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz)

在Linux安装toolchain之后，需要将toolchain的bin目录添加到PATH环境变量中

### 2.从腾讯云物联 C-SDK 中抽取相关代码
下载腾讯云物联 C-SDK 3.1.0版本，在根目录的 CMakeLists.txt 中配置为freertos平台，并开启代码抽取功能：
```
set(BUILD_TYPE                  "release")
set(PLATFORM 	                "freertos")
set(EXTRACT_SRC ON)
set(FEATURE_AT_TCP_ENABLED OFF)
```
Linux环境运行以下命令
```
mkdir build
cd build
cmake ..
```
将output/qcloud_iot_c_sdk 文件夹拷贝到当前目录的components/qcloud_iot/目录下

### 3.工程目录结构
在下载了ESP8266 RTOS SDK以及拷贝qcloud_iot_c_sdk之后，应该具有以下目录结构
```
QCloud_IoT_ESP8266_RTOS/
├── components
│   └── qcloud_iot
│       ├── port
│       │   └── include
│       │       └── netinet
│       └── qcloud_iot_c_sdk
│           ├── include
│           ├── platform
│           └── sdk_src
│               └── internal_inc
├── ESP8266_RTOS_SDK
│   ├── components
│   ├── docs
│   ├── examples
│   ├── make
│   └── tools
└── main
```

### 4.修改WiFi热点信息和腾讯云设备信息
到main/qcloud_iot_demo.h里面修改WiFi热点信息以及在腾讯云物联网平台注册的设备信息：

```
/* WiFi router SSID  */
#define TEST_WIFI_SSID                 CONFIG_DEMO_WIFI_SSID
/* WiFi router password */
#define TEST_WIFI_PASSWORD             CONFIG_DEMO_WIFI_PASSWORD

/* Product Id */
#define QCLOUD_IOT_PRODUCT_ID          CONFIG_QCLOUD_IOT_PRODUCT_ID
/* Device Name */
#define QCLOUD_IOT_DEVICE_NAME         CONFIG_QCLOUD_IOT_DEVICE_NAME
/* Device Secret */
#define QCLOUD_IOT_DEVICE_SECRET       CONFIG_QCLOUD_IOT_DEVICE_SECRET
```

### 5.编译及烧写
执行make menuconfig可进行功能配置，顶层菜单里面有对本示例的配置（QCloud IoT demo Configuration）
```
    [*] To demo IoT Explorer (y) or IoT Hub (n)   
    [*] To use smartconfig or not                 
    (YOUR_WIFI) WiFi SSID                         
    (12345678) WiFi PASSWORD                      
    (PRODUCT_ID) PRODUCT ID from QCloud IoT platform
    (device1) DEVICE NAME from QCloud IoT platform  
    (qwertyuiopasdfghh) DEVICE SECRET from QCloud IoT platform
```
第一项可选择演示IoT Explorer示例或者IoT Hub示例
第二项可选择在找不到目标WiFi路由器的时候是否进入smartconfig配网模式
后面几项也可以用于配置WiFi热点信息以及在腾讯云物联网平台注册的设备信息

也可以直接退出，然后使用根目录下面的默认配置文件
```
cp sdkconfig.qcloud_at sdkconfig
```
再执行make就可以在build目录下面生成镜像。

烧写镜像可以在Linux下面执行make flash命令，或者使用乐鑫在Windows下面的FLASH_DOWNLOAD_TOOLS工具
