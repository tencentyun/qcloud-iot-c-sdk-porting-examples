/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"


#ifdef AUTH_MODE_CERT
    /* 设备证书文件不存在*/
    #define QCLOUD_IOT_NULL_CERT_FILENAME          "YOUR_DEVICE_NAME_cert.crt"
    /* 设备私钥文件不存在*/
    #define QCLOUD_IOT_NULL_KEY_FILENAME           "YOUR_DEVICE_NAME_private.key"
#else
	 /* 设备密钥不存在*/
    #define QCLOUD_IOT_NULL_DEVICE_SECRET          "YOUR_IOT_PSK"
#endif


int main(int argc, char **argv) {

    //init log level
    IOT_Log_Set_Level(LOG_DEBUG);
   
    int ret;
	DeviceInfo sDevInfo;
	bool infoNullFlag = false;

    // to avoid process crash when writing to a broken socket
   	// signal(SIGPIPE, SIG_IGN);

	memset((char *)&sDevInfo, 0, sizeof(DeviceInfo));

	ret = HAL_GetDevInfo(&sDevInfo);

#ifdef 	AUTH_MODE_CERT
	/*用户需要根据自己的产品情况修改设备信息为空的逻辑，此处仅为示例*/
	if(!strcmp(sDevInfo.dev_cert_file_name, QCLOUD_IOT_NULL_CERT_FILENAME)
		||!strcmp(sDevInfo.dev_key_file_name, QCLOUD_IOT_NULL_KEY_FILENAME)){
		Log_d("dev Cert not exist!");
		infoNullFlag = true;
	}else{
		Log_d("dev Cert exist");
	}
#else
	/*用户需要根据自己的产品情况修改设备信息为空的逻辑，此处仅为示例*/
	if(!strcmp(sDevInfo.device_secret, QCLOUD_IOT_NULL_DEVICE_SECRET)){
		Log_d("dev psk not exist!");
		infoNullFlag = true;
	}else{
		Log_d("dev psk exist");
	}
#endif 

	/*设备信息为空，发起设备注册 注意：成功注册并完成一次连接后则无法再次发起注册，请做好设备信息的保存*/
	if(infoNullFlag){
		if(QCLOUD_ERR_SUCCESS == qcloud_iot_dyn_reg_dev(&sDevInfo)){
			ret = HAL_SetDevInfo(&sDevInfo);
			if(QCLOUD_ERR_SUCCESS != ret){
				Log_e("devices info save fail");
			}else{
#ifdef 	AUTH_MODE_CERT
				Log_d("dynamic register success, productID: %s, devName: %s, CertFile: %s, KeyFile: %s", \
						sDevInfo.product_id, sDevInfo.device_name, sDevInfo.dev_cert_file_name, sDevInfo.dev_key_file_name);
#else
				Log_d("dynamic register success,productID: %s, devName: %s, device_secret: %s", \
					    sDevInfo.product_id, sDevInfo.device_name, sDevInfo.device_secret);
#endif
			}
		}else{
			Log_e("%s dynamic register fail", sDevInfo.device_name);
		}
	}

    return ret;
}
