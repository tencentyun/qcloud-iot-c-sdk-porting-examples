Content 

* Drivers.

  ESP-WROOM-02D board flashed with latest AT firmware ESP8266-IDF-AT_V2.0_0
  STM32Cube_FW_L4_V1.15.1
  STMicroelectronics.X-CUBE-MEMS1 7.1.0

* Middlewares.  

  Tencent qcloud-iot-sdk-embedded-c
  FreeRTOS(ST modified.)
  mbedTLS

* Application.  

  1,on cloud side(IoT hub platofrm):
	1) Register an Tencent cloud account at https://cloud.tencent.com
	2) Create a product, get ProductID
	3) Create a device, get DeviceName and DeviceSecret 
	4) Define topics and set the permission as: device is capable of publishing and subscribing
	5) after the device/wifi provisioning, run the device application, you can view messages log on your IoT hub platform console.
   
  2,on device side:
   
	1) The parameters obtained from the cloud(ProductKey，DeviceName和DeviceSecret) must be updated into HAL_Device_freertos.c.
	2) Compile, download and run the application on target board.
	3) Use a terminal console for WiFi provisioning.

* Development Toolchains and Compilers 

  IAR v8.40.2
  Keil v5.24.1
  STM32CubeIDE v1.3.0
 
* Supported Board

  ST NUCLEO-L476RG MB1136 Rev C(search "NUCLEO-L476RG" on http://www.st.com)
  ST X-NUCLEO-IKS01A3(search "X-NUCLEO-IKS01A3" on http://www.st.com)
  ESPRESSIF ESP-WROOM-02D (https://www.espressif.com/zh-hans/products/hardware/esp-wroom-02/resources)
