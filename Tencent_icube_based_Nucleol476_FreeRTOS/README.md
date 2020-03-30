## Tencent_icube_based_Nucleol476_FreeRTOS 

Tencent_icube_based_Nucleol476_FreeRTOS 是[腾讯云设备端C-SDK](https://github.com/tencentyun/qcloud-iot-explorer-sdk-embedded-c.git)在Nucleol476+esp8266+iks01A3硬件环境，带FreeRTOS的移植示例，其中传感器板子iks01A3是可选项。联网方式是wifi模组esp8266，mcu通过串口AT方式与模组通信，基于SDK的at_socket方式实现联网。新增通信模组，在目录 Middlewares\Third_Party\qcloud-iot-sdk-embedded-c\platform\at_device 下参照at_device_esp8266.c添加对应驱动即可。

## 目录结构
| 名称            | 说明 |
| ----            | ---- |
| Documentation            | 文档目录 |
| Drivers         | STM32L476 mcu驱动及传感器单板IKS01A3的BSP |
| Projects           | 工程目录 |
| Middlewares/Third_Party         | 第三方软件包 |
|  ├─FreeRTOS         | FreeRTOS 10.01软件包 |
|  ├─qcloud-iot-sdk-embedded-c         | 腾讯云C-SDK |
| Utilities         | 相关工具 |
| README.md       |使用说明 |


## 使用指导
参考Documentation目录的 [I-CUBE-Tencent_UserManual.docx](https://git.code.oa.com/iotcloud_teamIII/qcloud-iot-c-sdk-porting-examples/blob/master/Tencent_icube_based_Nucleol476_FreeRTOS/Documentation/I-CUBE-Tencent_UserManual.docx)