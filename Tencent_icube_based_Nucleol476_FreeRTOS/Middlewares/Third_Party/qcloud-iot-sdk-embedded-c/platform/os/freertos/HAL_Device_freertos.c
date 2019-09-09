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
#include <stdarg.h>
#include "string.h"

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"
#include "utils_param_check.h"

#define DEBUG_DEV_INFO_USED

#ifdef DEBUG_DEV_INFO_USED
/* 产品ID, 与云端同步设备状态时需要  */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]	 = "PRODUCT_ID";

/* 设备名称, 与云端同步设备状态时需要 */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_DEV_NAME";

#ifdef DEV_DYN_REG_ENABLED
/* 产品密钥, 若使能动态注册功能，控制台生成，必填。若不使能，则不用赋值  */
static char sg_product_secret[MAX_SIZE_OF_PRODUCT_KEY + 1]  = "YOUR_PRODUCT_SECRET";
#endif

#ifdef AUTH_MODE_CERT
/* 客户端证书文件名  非对称加密使用, TLS 证书认证方式*/
static char sg_device_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1]      = "YOUR_DEVICE_NAME_cert.crt";
/* 客户端私钥文件名 非对称加密使用, TLS 证书认证方式*/
static char sg_device_privatekey_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1] = "YOUR_DEVICE_NAME_private.key";
#else
/* 设备密钥, TLS PSK认证方式*/
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1] = "YOUR_IOT_PSK";
#endif

#ifdef GATEWAY_ENABLED
/* 子产品名称, 与云端同步设备状态时需要  */
static char sg_sub_device_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]	 = "PRODUCT_ID";
/* 子设备名称, 与云端同步设备状态时需要 */
static char sg_sub_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_SUB_DEV_NAME";
#endif

static int device_info_copy(void *pdst, void *psrc, uint8_t max_len)
{
	if(strlen(psrc) > max_len){
		return QCLOUD_ERR_FAILURE;
	}
	memset(pdst, '\0', max_len);
	strncpy(pdst, psrc, max_len);
	return QCLOUD_ERR_SUCCESS;
}

#endif

int HAL_SetDevInfo(void *pdevInfo)
{
	POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;

#ifdef DEBUG_DEV_INFO_USED
	ret  = device_info_copy(sg_product_id, devInfo->product_id, MAX_SIZE_OF_PRODUCT_ID);//set product ID
	ret |= device_info_copy(sg_device_name, devInfo->device_name, MAX_SIZE_OF_DEVICE_NAME);//set dev name

#ifdef 	AUTH_MODE_CERT
	ret |= device_info_copy(sg_device_cert_file_name, devInfo->dev_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);//set dev cert file name
	ret |= device_info_copy(sg_device_privatekey_file_name, devInfo->dev_key_file_name, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);//set dev key file name
#else
	ret |= device_info_copy(sg_device_secret, devInfo->device_secret, MAX_SIZE_OF_DEVICE_SECRET);//set dev secret
#endif

#else
	Log_e("HAL_SetDevInfo not implement yet");
	ret = QCLOUD_ERR_DEV_INFO;
#endif

	if(QCLOUD_ERR_SUCCESS != ret) {
		Log_e("Set device info err");
		ret = QCLOUD_ERR_DEV_INFO;
	}
	return ret;
}

int HAL_GetDevInfo(void *pdevInfo)
{
	POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;	
	memset((char *)devInfo, '\0', sizeof(DeviceInfo));	

#ifdef DEBUG_DEV_INFO_USED
	ret  = device_info_copy(devInfo->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);//get product ID
	ret |= device_info_copy(devInfo->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);//get dev name

#ifdef DEV_DYN_REG_ENABLED
	ret |= device_info_copy(devInfo->product_key, sg_product_secret, MAX_SIZE_OF_PRODUCT_KEY );//get product ID
#endif 

#ifdef 	AUTH_MODE_CERT
	ret |= device_info_copy(devInfo->dev_cert_file_name, sg_device_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);//get dev cert file name
	ret |= device_info_copy(devInfo->dev_key_file_name, sg_device_privatekey_file_name, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);//get dev key file name
#else
	ret |= device_info_copy(devInfo->device_secret, sg_device_secret, MAX_SIZE_OF_DEVICE_SECRET);//get dev secret
#endif

#else
	Log_e("HAL_GetDevInfo not implement yet");
	ret = QCLOUD_ERR_DEV_INFO;
#endif

	if(QCLOUD_ERR_SUCCESS != ret){
		Log_e("Get device info err");
		ret = QCLOUD_ERR_DEV_INFO;
	}
	return ret;
}

#ifdef GATEWAY_ENABLED
int HAL_GetGwDevInfo(void *pgwDeviceInfo)
{
	POINTER_SANITY_CHECK(pgwDeviceInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	GatewayDeviceInfo *gwDevInfo = (GatewayDeviceInfo *)pgwDeviceInfo;
	memset((char *)gwDevInfo, 0, sizeof(GatewayDeviceInfo));

#ifdef DEBUG_DEV_INFO_USED
	ret  = HAL_GetDevInfo(&(gwDevInfo->gw_info));//get gw dev info
	//only one sub-device is supported now 
	gwDevInfo->sub_dev_num = 1;
	gwDevInfo->sub_dev_info = (DeviceInfo *)HAL_Malloc(sizeof(DeviceInfo)*(gwDevInfo->sub_dev_num));
	memset((char *)gwDevInfo->sub_dev_info, '\0', sizeof(DeviceInfo));
	//copy sub dev info
	ret = device_info_copy(gwDevInfo->sub_dev_info->product_id, sg_sub_device_product_id, MAX_SIZE_OF_PRODUCT_ID);
	ret |= device_info_copy(gwDevInfo->sub_dev_info->device_name, sg_sub_device_name, MAX_SIZE_OF_DEVICE_NAME);

#else
	Log_e("HAL_GetDevInfo not implement yet");
	ret = QCLOUD_ERR_DEV_INFO;
#endif

	if(QCLOUD_ERR_SUCCESS != ret) {
		Log_e("Get gateway device info err");
		ret = QCLOUD_ERR_DEV_INFO;
	}
	return ret;
}
#endif 
