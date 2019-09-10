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

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <string.h>

#include "device.h"
#include "qcloud_iot_export.h"


static DeviceInfo   sg_device_info;
static bool         sg_devinfo_initialized;

int iot_device_info_init() {
	if (sg_devinfo_initialized) {
		Log_e("device info has been initialized.");
		return 0;
	}

	memset(&sg_device_info, 0x0, sizeof(DeviceInfo));
	sg_devinfo_initialized = true;
	
	return QCLOUD_ERR_SUCCESS;
}

int iot_device_info_set(const char *product_id, const char *device_name) {	

	memset(&sg_device_info, 0x0, sizeof(DeviceInfo));
	if ((MAX_SIZE_OF_PRODUCT_ID) < strlen(product_id))
	{
		Log_e("product name(%s) length:(%lu) exceeding limitation", product_id, strlen(product_id));
		return QCLOUD_ERR_FAILURE;
	}
	if ((MAX_SIZE_OF_DEVICE_NAME) < strlen(device_name))
	{
		Log_e("device name(%s) length:(%lu) exceeding limitation", device_name, strlen(device_name));
		return QCLOUD_ERR_FAILURE;
	}

	strncpy(sg_device_info.product_id, product_id, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(sg_device_info.device_name, device_name, MAX_SIZE_OF_DEVICE_NAME);

	/* construct device-id(@product_id+@device_name) */
	memset(sg_device_info.client_id, 0x0, MAX_SIZE_OF_CLIENT_ID);
    int ret = HAL_Snprintf(sg_device_info.client_id, MAX_SIZE_OF_CLIENT_ID, "%s%s", product_id, device_name);
    if ((ret < 0) || (ret >= MAX_SIZE_OF_CLIENT_ID)) {
        Log_e("set device info failed");
        return QCLOUD_ERR_FAILURE;
    }

    Log_i("SDK_Ver: %s, Product_ID: %s, Device_Name: %s", QCLOUD_IOT_DEVICE_SDK_VERSION, product_id, device_name);
	return QCLOUD_ERR_SUCCESS;
}

DeviceInfo* iot_device_info_get(void)
{
    return &sg_device_info;
}

#define DEBUG_DEV_INFO_USED
#ifndef DEBUG_DEV_INFO_USED
#include "lite-utils.h"
#include "utils_param_check.h"

#define MAX_DEV_INFO_FILE_LEN		1024
#define MAX_CONFIG_FILE_NAME		256
#define MAX_HOST_LEN			    64

#define DEV_INFO_JSON_FILE_PATH 	"./device_info.json"
#define KEY_AUTH_MODE				"auth_mode"
#define KEY_PRODUCT_ID				"productId"
#define KEY_PRODUCT_SECRET			"productSecret"
#define KEY_DEV_NAME				"deviceName"
#define KEY_DEV_SECRET				"key_deviceinfo.deviceSecret"
#define KEY_DEV_CERT				"cert_deviceinfo.devCertFile"
#define KEY_DEV_PRIVATE_KEY			"cert_deviceinfo.devPrivateKeyFile"
#define KEY_MQTT_HOST				"mqtt_host"
#define KEY_COAP_HOST				"coap_host"
#define KEY_HTTP_HOST				"log_http_host"
#define KEY_HTTP_PORT				"log_http_port"
#define KEY_SUBDEV_PRODUCT_ID		"subDev.sub_productId"
#define KEY_SUBDEV_NAME				"subDev.sub_devName"

#define STR_DEV_INFO				"key_deviceinfo"
#define STR_DEV_SECRET				"deviceSecret"
#define STR_DEV_CERT				"cert_deviceinfo"
#define STR_DEV_CERT_FILE			"devCertFile"
#define STR_DEV_KEY_FILE			"devPrivateKeyFile"

static char sg_configfile[MAX_CONFIG_FILE_NAME] = {'\0'};
static char sg_mqtthost[MAX_HOST_LEN] = {'\0'};
static char sg_coaphost[MAX_HOST_LEN] = {'\0'};
static char sg_httphost[MAX_HOST_LEN] = {'\0'};
static char sg_httpport[MAX_HOST_LEN] = {'\0'};

int iot_set_config_file(const char * configfile)
{
	int ret = QCLOUD_ERR_FAILURE;
	
	if(strlen(configfile) > 0){
		memset(sg_configfile, 0, MAX_CONFIG_FILE_NAME);
		strncpy(sg_configfile, configfile, MAX_CONFIG_FILE_NAME);
		ret = QCLOUD_ERR_SUCCESS;
	}

	return ret;
}

char * iot_get_config_file(void)
{
	return sg_configfile;
}

static int iot_set_host(eHostType type, const char * host)
{
	int ret = QCLOUD_ERR_FAILURE;

	if((NULL != host) && strlen(host) > 0){
		switch(type){
			case eHOST_MQTT:
				memset(sg_mqtthost, 0, MAX_HOST_LEN);
				strncpy(sg_mqtthost, host, MAX_HOST_LEN);
				ret = QCLOUD_ERR_SUCCESS;
				break;
				
			case eHOST_COAP:
				memset(sg_coaphost, 0, MAX_HOST_LEN);
				strncpy(sg_coaphost, host, MAX_HOST_LEN);
				ret = QCLOUD_ERR_SUCCESS;
				break;
				
			case eHOST_LOG_HTTP:
				memset(sg_httphost, 0, MAX_HOST_LEN);
				strncpy(sg_httphost, host, MAX_HOST_LEN);
				ret = QCLOUD_ERR_SUCCESS;
				break;
				
			case eHOST_LOG_PORT:
				memset(sg_httpport, 0, MAX_HOST_LEN);
				strncpy(sg_httpport, host, MAX_HOST_LEN);
				ret = QCLOUD_ERR_SUCCESS;
				break;
				
			default:
				Log_e("no support type");
				break;
		}		
	}

	return ret;
}

char *iot_get_host(eHostType type)
{
	char *phost = NULL;

	switch(type){
		case eHOST_MQTT:
			phost = sg_mqtthost;
			break;
		
		case eHOST_COAP:
			phost = sg_coaphost;
			break;
		
		case eHOST_LOG_HTTP:
			phost = sg_httphost;
			break;
			
		case eHOST_LOG_PORT:
			phost = sg_httpport;
			break;
			
		default:
			Log_e("no support type");
			break;
	}		

	return phost;
}

static int iot_parse_devinfo_from_json_file(DeviceInfo *pDevInfo)
{
	FILE *fp;
	uint32_t len;
	uint32_t rlen;
	int ret;
	char *config_filepath;

	if(strlen(iot_get_config_file()) == 0){
		iot_set_config_file(DEV_INFO_JSON_FILE_PATH);
	}	
	config_filepath = iot_get_config_file();
	fp = fopen(config_filepath, "r");	
    if(NULL == fp) {
        Log_e("open file %s failed", config_filepath);
        ret =  QCLOUD_ERR_FAILURE;
		goto exit;
    }

	
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
	if(len > MAX_DEV_INFO_FILE_LEN){
		Log_e("%s is too big, pls check", config_filepath);
		ret =  QCLOUD_ERR_FAILURE;
		goto exit;
	}

	
	char *JsonDoc = (char *)HAL_Malloc(len + 10);
	if(NULL == JsonDoc){
		Log_e("malloc buffer for json file read fail"); 
		ret =  QCLOUD_ERR_FAILURE;
		goto exit;
	}
	
	rewind(fp);
	rlen = fread(JsonDoc, 1, len, fp);
	
	if(len != rlen){
		Log_e("read data len (%d) less than needed (%d), %s", rlen, len, JsonDoc);
	}

	/*Get device info*/
	char* authMode = LITE_json_value_of(KEY_AUTH_MODE, JsonDoc);
	char* productId = LITE_json_value_of(KEY_PRODUCT_ID, JsonDoc);		
	char* devName = LITE_json_value_of(KEY_DEV_NAME, JsonDoc);
	
	strncpy(pDevInfo->product_id, productId, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(pDevInfo->device_name, devName, MAX_SIZE_OF_DEVICE_NAME);

#ifdef DEV_DYN_REG_ENABLED	
	char* productSecret = LITE_json_value_of(KEY_PRODUCT_SECRET, JsonDoc);
	strncpy(pDevInfo->product_key, productSecret, MAX_SIZE_OF_PRODUCT_KEY);
#endif	
	
#ifdef 	AUTH_MODE_CERT
	char* devCertFileName = LITE_json_value_of(KEY_DEV_CERT, JsonDoc);
	char* devKeyFileName = LITE_json_value_of(KEY_DEV_PRIVATE_KEY, JsonDoc);		
	strncpy(pDevInfo->dev_cert_file_name, devCertFileName, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
	strncpy(pDevInfo->dev_key_file_name, devKeyFileName, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);
	Log_d("mode:%s, pid:%s, devName:%s, devCertFileName:%s, devKeyFileName:%s", authMode,\
			productId, devName,  devCertFileName, devKeyFileName);
#else
	char* devSecret = LITE_json_value_of(KEY_DEV_SECRET, JsonDoc);
	strncpy(pDevInfo->device_secret, devSecret, MAX_SIZE_OF_DEVICE_SECRET);
	Log_d("mode:%s, pid:%s, devName:%s, devSerect:%s", authMode, productId, devName,  devSecret);
#endif

	/*Get host config*/
	char* mqtt_host = LITE_json_value_of(KEY_MQTT_HOST, JsonDoc);
	char* coap_host = LITE_json_value_of(KEY_COAP_HOST, JsonDoc);		
	char* log_http_host = LITE_json_value_of(KEY_HTTP_HOST, JsonDoc);
	char* log_http_port = LITE_json_value_of(KEY_HTTP_PORT, JsonDoc);
	
	ret = iot_set_host(eHOST_MQTT, mqtt_host);
	ret |= iot_set_host(eHOST_COAP, coap_host);
	ret |= iot_set_host(eHOST_LOG_HTTP, log_http_host);
	ret |= iot_set_host(eHOST_LOG_PORT, log_http_port);

	Log_d("mqtt_host:%s, coap_host:%s, log_http_host:%s, log_http_port:%s", mqtt_host, coap_host, log_http_host, log_http_port);	
	
	if(authMode){
		HAL_Free(authMode);
	}
	
	if(productId){
		HAL_Free(productId);
	}
	
	if(devName){
		HAL_Free(devName);
	}
	
#ifdef DEV_DYN_REG_ENABLED		
	if(productSecret){
		HAL_Free(productSecret);
	}
#endif

#ifdef 	AUTH_MODE_CERT
	if(devCertFileName){
		HAL_Free(devCertFileName);
	}

	if(devKeyFileName){
		HAL_Free(devKeyFileName);
	}
#else
	if(devSecret){
		HAL_Free(devSecret);
	}
#endif

	if(mqtt_host){
		HAL_Free(mqtt_host);
	}
	
	if(coap_host){
		HAL_Free(coap_host);
	}
	
	if(log_http_host){
		HAL_Free(log_http_host);
	}
	
	if(log_http_port){
		HAL_Free(log_http_port);
	}

exit:
	
	if(NULL != fp){
		fclose(fp);
	}

	return ret;
}

static int iot_parse_subdevinfo_from_json_file(DeviceInfo *pDevInfo)
{
	FILE *fp;
	uint32_t len;
	uint32_t rlen;
	int ret = QCLOUD_ERR_SUCCESS;
	char *config_filepath;

	if(strlen(iot_get_config_file()) == 0){
		iot_set_config_file(DEV_INFO_JSON_FILE_PATH);
	}	
	config_filepath = iot_get_config_file();
	fp = fopen(config_filepath, "r");	
    if(NULL == fp) {
        Log_e("open file %s failed", config_filepath);
        ret =  QCLOUD_ERR_FAILURE;
		goto exit;
    }
	
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
	if(len > MAX_DEV_INFO_FILE_LEN){
		Log_e("%s is too big, pls check", config_filepath);
		ret =  QCLOUD_ERR_FAILURE;
		goto exit;
	}

	
	char *JsonDoc = (char *)HAL_Malloc(len + 10);
	if(NULL == JsonDoc){
		Log_e("malloc buffer for json file read fail"); 
		ret =  QCLOUD_ERR_FAILURE;
		goto exit;
	}
	
	rewind(fp);
	rlen = fread(JsonDoc, 1, len, fp);
	
	if(len != rlen){
		Log_e("read data len (%d) less than needed (%d), %s", rlen, len, JsonDoc);
	}

	/*Get sub device info*/
	char* productId = LITE_json_value_of(KEY_SUBDEV_PRODUCT_ID, JsonDoc);		
	char* devName = LITE_json_value_of(KEY_SUBDEV_NAME, JsonDoc);
	
	strncpy(pDevInfo->product_id, productId, MAX_SIZE_OF_PRODUCT_ID);
	strncpy(pDevInfo->device_name, devName, MAX_SIZE_OF_DEVICE_NAME);
	Log_d("pid:%s, devName:%s", productId, devName);
	
	if(productId){
		HAL_Free(productId);
	}
	
	if(devName){
		HAL_Free(devName);
	}
	
exit:
	
	if(NULL != fp){
		fclose(fp);
	}

	return ret;
}


int iot_save_devinfo_to_json_file(DeviceInfo *pDevInfo)
{
	char JsonDoc[MAX_DEV_INFO_FILE_LEN] = {0};
	int32_t remain_size = MAX_DEV_INFO_FILE_LEN;
    int32_t rc_of_snprintf = 0;
	int32_t wlen;
	FILE *fp;
	

	rc_of_snprintf = HAL_Snprintf(JsonDoc, remain_size, 
									"{\"%s\":\"%s\", \"%s\":\"%s\", ", \
									KEY_PRODUCT_ID, pDevInfo->product_id,\
									KEY_DEV_NAME, pDevInfo->device_name);
	remain_size -= rc_of_snprintf;
	
#ifdef DEV_DYN_REG_ENABLED
	rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\"%s\":\"%s\", ", KEY_PRODUCT_SECRET, pDevInfo->product_key);
	remain_size -= rc_of_snprintf;
#endif
	
#ifdef 	AUTH_MODE_CERT
	rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\"%s\":{", STR_DEV_CERT); 
	remain_size -= rc_of_snprintf;
	rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size,  \
									"\"%s\":\"%s\", \"%s\":\"%s\"}, \"%s\":\"%s\"}",\
			  						STR_DEV_CERT_FILE, pDevInfo->dev_cert_file_name,\
				  					STR_DEV_KEY_FILE, pDevInfo->dev_key_file_name, \
				  					KEY_AUTH_MODE, "CERT");
	remain_size -= rc_of_snprintf;
#else

	rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\"%s\":{", STR_DEV_INFO); 
	remain_size -= rc_of_snprintf;
	rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size,  \
									"\"%s\":\"%s\"}, \"%s\":\"%s\"}",\
			  						STR_DEV_SECRET, pDevInfo->device_secret,\
				  					KEY_AUTH_MODE, "KEY");
	remain_size -= rc_of_snprintf;
#endif

	Log_d("JsonDoc(%d):%s", MAX_DEV_INFO_FILE_LEN - remain_size, JsonDoc);

	fp = fopen(DEV_INFO_JSON_FILE_PATH, "w");	
    if(NULL == fp) {
        Log_e("open file %s failed", DEV_INFO_JSON_FILE_PATH);
        return  QCLOUD_ERR_FAILURE;
    }

	wlen = fwrite(JsonDoc, 1, strlen(JsonDoc), fp);
	if(wlen < (MAX_DEV_INFO_FILE_LEN - remain_size)){
		Log_e("write len less than needed");
	} 

	fclose(fp);

	return QCLOUD_ERR_SUCCESS;
}

int HAL_SetDevInfo(void *pdevInfo)
{
	POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;

	ret = iot_save_devinfo_to_json_file(devInfo);

	return ret;
}


int HAL_GetDevInfo(void *pdevInfo)
{
	POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;
		
	memset((char *)devInfo, '\0', sizeof(DeviceInfo));	
	ret = iot_parse_devinfo_from_json_file(devInfo);

	return ret;
}

int HAL_GetSubDevInfo(void *pdevInfo)
{
	POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
	int ret;
	DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;
		
	memset((char *)devInfo, '\0', sizeof(DeviceInfo));	
	ret = iot_parse_subdevinfo_from_json_file(devInfo);

	return ret;
}

#endif

#ifdef __cplusplus
}
#endif
