##  qcloud-iot-c-sdk-porting-examples 

qcloud-iot-c-sdk-porting-examples 是[腾讯云设备端C-SDK](https://github.com/tencentyun/qcloud-iot-explorer-sdk-embedded-c.git)的移植示例，详细信息请参考示例下的Documentation中的说明文档。

## 目录结构
| 名称            | 说明 |
| ----            | ---- |
| Tencent_icube_based_Nucleol476_FreeRTOS           | 基于STM32+esp8266+FreeRTOS的移植示例 |
| Tencent_icube_based_Nucleol476_nonOS          	| 基于STM32+esp8266+无RTOS的移植示例 |
| README.md       |使用说明 |

>! I-Cube工程部分路径嵌套较深，建议放在盘符路径的根目录，否则Keil会有编译错误提示。