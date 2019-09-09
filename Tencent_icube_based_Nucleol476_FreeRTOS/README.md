## Tencent_icube_based_Nucleol476_FreeRTOS 

Tencent_icube_based_Nucleol476_FreeRTOS 是[腾讯云设备端C-SDK](https://github.com/tencentyun/qcloud-iot-sdk-embedded-c.git)在Nucleol476+esp8266+iks01A3硬件环境，带FreeRTOS的移植示例，其中传感器板子iks01A3是可选项。联网方式是wifi模组esp8266，mcu通过串口AT方式与模组通信，基于SDK的at_socket方式实现联网。新增通信模组，在目录 Middlewares\Third_Party\qcloud-iot-sdk-embedded-c\platform\at_device 下参照at_device_esp8266.c添加对应驱动即可。

## 目录结构
| 名称            | 说明 |
| ----            | ---- |
| docs            | 文档目录 |
| Drivers         | STM32L476 mcu驱动及传感器单板IKS01A3的BSP |
| inc           | BSP相关头文件 |
| MDK-ARM         | 工程文件 |
| Middlewares/Third_Party         | 第三方软件包 |
|  ├─FreeRTOS         | FreeRTOS 10.01软件包 |
|  ├─qcloud-iot-sdk-embedded-c         | 腾讯云C-SDK |
| Src         | nucleol476板级驱动及示例 |
| README.md       |使用说明 |


## 示例内容
工程集成了四个示例，分别是mqtt_sample、shadow_sample、light_sample、icube_data_template_sample，启动后进入shell交互模式，可以通过shell配置wifi热点信息及选择运行对应示例。

![shell](https://main.qcloudimg.com/raw/60601a8e9e0cdd76ee22252d51c374a2.jpg)


## 快速入门
各示例详细用法参考doc目录的 [tencent_icube_quick_start_guide.pptx](https://git.code.oa.com/iotcloud_teamIII/qcloud-iot-c-sdk-porting-examples/blob/master/Tencent_icube_based_Nucleol476_FreeRTOS/doc/tencent_icube_quick_start_guide.pptx)