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
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"
#include "utils_timer.h"
#include "qcloud_iot_demo.h"
#include "board_ops.h"


static MQTTEventType sg_subscribe_event_result = MQTT_EVENT_UNDEF;
static bool sg_delta_arrived = false;


static char sg_data_report_buffer[2048];
size_t sg_data_report_buffersize = sizeof(sg_data_report_buffer) / sizeof(sg_data_report_buffer[0]);

/*-----------------data config start  -------------------*/ 
#define TOTAL_PROPERTY_COUNT 3
#define MAX_STR_NAME_LEN	(64)

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

typedef enum{
	eCOLOR_RED = 0,
	eCOLOR_GREEN = 1,
	eCOLOR_BLUE = 2,
}eColor;

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_BOOL m_light_switch; 
    TYPE_DEF_TEMPLATE_INT m_brightness;
    TYPE_DEF_TEMPLATE_STRING m_name[MAX_STR_NAME_LEN+1];
} ProductDataDefine;

static   ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    memset((void *) & sg_ProductData, 0, sizeof(ProductDataDefine));
	
	sg_ProductData.m_light_switch = 0;
    sg_DataTemplate[0].data_property.key  = "power_switch";
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_light_switch;
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;   
    
	sg_ProductData.m_brightness = 0;
    sg_DataTemplate[1].data_property.key  = "brightness";
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_brightness;
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;

	strncpy(sg_ProductData.m_name, QCLOUD_IOT_DEVICE_NAME, MAX_STR_NAME_LEN);
	sg_ProductData.m_name[strlen(QCLOUD_IOT_DEVICE_NAME)] = '\0';
    sg_DataTemplate[2].data_property.key  = "name";
    sg_DataTemplate[2].data_property.data = sg_ProductData.m_name;
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;

};
/*-----------------data config end  -------------------*/ 

#ifdef EVENT_POST_ENABLED
/*-----------------event config start  -------------------*/ 
#define EVENT_COUNTS     (2)
#define MAX_EVENT_STR_MESSAGE_LEN (64)
#define MAX_EVENT_STR_NAME_LEN (64)


static TYPE_DEF_TEMPLATE_BOOL sg_status;
static TYPE_DEF_TEMPLATE_STRING sg_message[MAX_EVENT_STR_MESSAGE_LEN+1];
static DeviceProperty g_propertyEvent_status_report[] = {

   {.key = "status", .data = &sg_status, .type = TYPE_TEMPLATE_BOOL},
   {.key = "message", .data = sg_message, .type = TYPE_TEMPLATE_STRING},
};

static TYPE_DEF_TEMPLATE_FLOAT sg_voltage;
static DeviceProperty g_propertyEvent_low_voltage[] = {

   {.key = "voltage", .data = &sg_voltage, .type = TYPE_TEMPLATE_FLOAT},
};

static sEvent g_events[]={

    {
     .event_name = "status_report",
     .type = "info",
     .timestamp = 0,
     .eventDataNum = sizeof(g_propertyEvent_status_report)/sizeof(g_propertyEvent_status_report[0]),
     .pEventData = g_propertyEvent_status_report,
    },
    {
     .event_name = "low_voltage",
     .type = "alert",
     .timestamp = 0,
     .eventDataNum = sizeof(g_propertyEvent_low_voltage)/sizeof(g_propertyEvent_low_voltage[0]),
     .pEventData = g_propertyEvent_low_voltage,
    },
};	 
/*-----------------event config end	-------------------*/ 


static void update_events_timestamp(sEvent *pEvents, int count)
{
	int i;
	
	for(i = 0; i < count; i++){
        if (NULL == (&pEvents[i])) { 
	        Log_e("null event pointer"); 
	        return; 
        }
#ifdef EVENT_TIMESTAMP_USED		
		pEvents[i].timestamp = time(NULL);	//should be UTC and accurate
#else
		pEvents[i].timestamp = 0;
#endif
	}
}

static void event_post_cb(void *pClient, MQTTMessage *msg)
{
	Log_d("Reply:%.*s", msg->payload_len, msg->payload);
	clearEventFlag(FLAG_EVENT0);
}

static void eventPostCheck(void *client)
{
	int i;
	int rc;
	uint32_t eflag;
	sEvent *pEventList[EVENT_COUNTS];
	uint8_t eventCont;
	
	eflag = getEventFlag();
	if((EVENT_COUNTS > 0 )&& (eflag > 0)){	
		eventCont = 0;
		for(i = 0; i < EVENT_COUNTS; i++){
			if((eflag&(1<<i))&ALL_EVENTS_MASK){
				 pEventList[eventCont++] = &(g_events[i]);
				 update_events_timestamp(&g_events[i], 1);
				clearEventFlag(1<<i);
			}
		}	
	
		rc = qcloud_iot_post_event(client, sg_data_report_buffer, sg_data_report_buffersize, \
											eventCont, pEventList, event_post_cb);
		if(rc < 0){
			Log_e("events post failed: %d", rc);
		}
	}

}

#endif


static void _event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) 
{	
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
		case MQTT_EVENT_UNDEF:
			Log_i("undefined event occur.");
			break;

		case MQTT_EVENT_DISCONNECT:
			Log_i("MQTT disconnect.");
			break;

		case MQTT_EVENT_RECONNECT:
			Log_i("MQTT reconnect.");
			break;

		case MQTT_EVENT_SUBCRIBE_SUCCESS:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_SUCCESS:
			Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_TIMEOUT:
			Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_NACK:
			Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
			break;
		default:
			Log_i("Should NOT arrive here.");
			break;
	}
}


/**
 * MQTT connect parameters
 */
static int _setup_connect_init_params(ShadowInitParams* initParams)
{
	initParams->device_name = QCLOUD_IOT_DEVICE_NAME;
	initParams->product_id = QCLOUD_IOT_PRODUCT_ID;
	initParams->device_secret = QCLOUD_IOT_DEVICE_SECRET;

    initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->shadow_type = eTEMPLATE;	 
    initParams->event_handle.h_fp = _event_handler;

    return QCLOUD_RET_SUCCESS;
}

/* parse self defined string value */
static int update_self_define_value(const char *pJsonDoc, DeviceProperty *pProperty) 
{
    int rc = QCLOUD_RET_SUCCESS;
		
	if((NULL == pJsonDoc)||(NULL == pProperty)){
		return QCLOUD_ERR_INVAL;
	}
	
	/*convert const char* to char * */
	char *pTemJsonDoc =HAL_Malloc(strlen(pJsonDoc) + 1);
	strcpy(pTemJsonDoc, pJsonDoc);

	char* property_data = LITE_json_value_of(pProperty->key, pTemJsonDoc);	
	
    if(property_data != NULL){
		if(pProperty->type == TYPE_TEMPLATE_STRING){
			/*check the key*/					
			if(0 == strcmp("name", pProperty->key)){
				memset(sg_ProductData.m_name, 0, MAX_STR_NAME_LEN);
				strncpy(sg_ProductData.m_name, pProperty->data, MAX_STR_NAME_LEN);
				sg_ProductData.m_name[MAX_STR_NAME_LEN-1] = '\0';
			}
		}else if(pProperty->type == TYPE_TEMPLATE_JOBJECT){
			Log_d("Json type wait to be deal,%s",property_data);	
		}
		
		HAL_Free(property_data);
    }else{
		
		rc = QCLOUD_ERR_FAILURE;
		Log_d("Property:%s no matched",pProperty->key);	
	}
	
	HAL_Free(pTemJsonDoc);
		
    return rc;
}

/* callback for self defined string value */
static void OnDeltaTemplateCallback(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) 
{
    int i = 0;

    for (i = 0; i < TOTAL_PROPERTY_COUNT; i++) {
		/* handle self defined string/json here. Other properties are dealed in _handle_delta()*/
        if (strcmp(sg_DataTemplate[i].data_property.key, pProperty->key) == 0) {
            sg_DataTemplate[i].state = eCHANGED;
			if((sg_DataTemplate[i].data_property.type == TYPE_TEMPLATE_STRING)
				||(sg_DataTemplate[i].data_property.type == TYPE_TEMPLATE_JOBJECT)){

				update_self_define_value(pJsonValueBuffer, &(sg_DataTemplate[i].data_property));
			}
		
            Log_i("Property=%s changed", pProperty->key);
            sg_delta_arrived = true;
            return;
        }
    }

    Log_e("Property=%s changed no match", pProperty->key);
}


static void OnShadowUpdateCallback(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument, void *pUserdata) {
	Log_i("recv shadow update response, response ack: %d", requestAck);	
}


static int _register_data_template_property(void *pshadow_client)
{
	int i,rc;
	
    for (i = 0; i < TOTAL_PROPERTY_COUNT; i++) {
	    rc = IOT_Shadow_Register_Property(pshadow_client, &sg_DataTemplate[i].data_property, OnDeltaTemplateCallback);
	    if (rc != QCLOUD_RET_SUCCESS) {
	        rc = IOT_Shadow_Destroy(pshadow_client);
	        Log_e("register device data template property failed, err: %d", rc);
	        return rc;
	    } else {
	        Log_i("data template property=%s registered.", sg_DataTemplate[i].data_property.key);
	    }
    }

	return QCLOUD_RET_SUCCESS;
}


static void deal_down_stream_user_logic(ProductDataDefine *light)
{
	int i;
    char brightness_bar[]  = "||||||||||||||||||||";
    int brightness_bar_len = strlen(brightness_bar);


	/* using console log to show brightness change */		    
    brightness_bar_len = (light->m_brightness >= 100)?brightness_bar_len:(int)((light->m_brightness * brightness_bar_len)/100);
    for (i = brightness_bar_len; i < strlen(brightness_bar); i++) {
        brightness_bar[i] = '-';
    }

	if(light->m_light_switch){        
		HAL_Printf( "\n[  light is on  ]|[brightness:%s]|[%s]\n\n", brightness_bar, light->m_name);
	}else{
		HAL_Printf( "\n[  light is off ]|[brightness:%s]|[%s]\n\n", brightness_bar, light->m_name);	
	}

    /** hardware control **/
	if(light->m_light_switch) {
       set_relay_led_state(RELAY_LED_ON);
	} else {
	   set_relay_led_state(RELAY_LED_OFF);
	}

#ifdef EVENT_POST_ENABLED
	if(eCHANGED == sg_DataTemplate[0].state){
		if(light->m_light_switch){	
			memset(sg_message, 0, MAX_EVENT_STR_MESSAGE_LEN);
			strcpy(sg_message,"light off");
			sg_status = 1;
		}else{
			memset(sg_message, 0, MAX_EVENT_STR_MESSAGE_LEN);
			strcpy(sg_message,"light on");
			sg_status = 0;			
		}
		setEventFlag(FLAG_EVENT0);
	}
#endif
	
}


/*just to simulate a value change */
int cycle_report(DeviceProperty *pReportDataList[], Timer *reportTimer)
{
	int i;

	if(expired(reportTimer)){
	    for (i = 0; i < TOTAL_PROPERTY_COUNT; i++) {       
			pReportDataList[i] = &(sg_DataTemplate[i].data_property);
			/* simulate brightness changes */
			if (0 == strcmp(pReportDataList[i]->key, "brightness")) {
			    int *brightness = (int *)pReportDataList[i]->data;
			    *brightness += 5;
			    if (*brightness > 100)
			        *brightness = 0;
			}
	    }
	    countdown_ms(reportTimer, 3000);
		return QCLOUD_RET_SUCCESS;
	}else{
		return QCLOUD_ERR_INVAL;
	}		
}

int qcloud_iot_explorer_demo(void)
{   
	DeviceProperty *pReportDataList[TOTAL_PROPERTY_COUNT];
	Timer reportTimer;
	int rc;    

    //init connection
    ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
    if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("init params err,rc=%d", rc);
		return rc;
	}

    void *client = IOT_Shadow_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    //init data template
    _init_data_template();

#ifdef EVENT_POST_ENABLED
	rc = event_init(client);
	if (rc < 0) 
	{
		Log_e("event init failed: %d", rc);
		return rc;
	}
#endif

    //register data template propertys here
    rc = _register_data_template_property(client);
    if (rc == QCLOUD_RET_SUCCESS) {
        Log_i("Register data template propertys Success");
    } else {
        Log_e("Register data template propertys Failed: %d", rc);
        return rc;
    }

	//shadow sync
	rc = IOT_Shadow_Get_Sync(client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("get device shadow failed, err: %d", rc);
		return rc;
	}

	InitTimer(&reportTimer);
	
    while (IOT_Shadow_IsConnected(client) || rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT 
		|| rc == QCLOUD_RET_MQTT_RECONNECTED || QCLOUD_RET_SUCCESS == rc) {

        rc = IOT_Shadow_Yield(client, 500);

        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            sleep(1);
            continue;
        }
		else if (rc != QCLOUD_RET_SUCCESS) {
			Log_e("Exit loop caused of errCode: %d", rc);
		}


		if (sg_delta_arrived) {													

			deal_down_stream_user_logic(&sg_ProductData);

            int rc = IOT_Shadow_JSON_ConstructDesireAllNull(client, sg_data_report_buffer, sg_data_report_buffersize);
            if (rc == QCLOUD_RET_SUCCESS) {
                rc = IOT_Shadow_Update_Sync(client, sg_data_report_buffer, sg_data_report_buffersize, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
                sg_delta_arrived = false;
                if (rc == QCLOUD_RET_SUCCESS) {
                    Log_i("shadow update(desired) success");
                } else {
                    Log_e("shadow update(desired) failed, err: %d", rc);
                }

            } else {
                Log_e("construct desire failed, err: %d", rc);
            }
		}			
		
		if (QCLOUD_RET_SUCCESS == cycle_report(pReportDataList, &reportTimer)){				
			rc = IOT_Shadow_JSON_ConstructReportArray(client, sg_data_report_buffer, sg_data_report_buffersize, TOTAL_PROPERTY_COUNT, pReportDataList);
	        if (rc == QCLOUD_RET_SUCCESS) {
				Log_i("cycle report: %s",sg_data_report_buffer);
	            rc = IOT_Shadow_Update(client, sg_data_report_buffer, sg_data_report_buffersize, 
	                    OnShadowUpdateCallback, NULL, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
	            if (rc == QCLOUD_RET_SUCCESS) {
	                Log_d("shadow update(reported) success");
	            } else {
	                Log_e("shadow update(reported) failed, err: %d", rc);
	            }
	        } else {
	            Log_e("construct reported failed, err: %d", rc);
	        }

		}
		
#ifdef EVENT_POST_ENABLED	
		eventPostCheck(client);
#endif

        HAL_SleepMs(500);
    }

    rc = IOT_Shadow_Destroy(client);

    return rc;
}
