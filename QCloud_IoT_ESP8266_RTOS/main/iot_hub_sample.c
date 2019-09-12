/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS chips only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_demo.h"


#define MAX_SIZE_OF_TOPIC_CONTENT 100

static int sg_count = 0;
static int sg_sub_packet_id = -1;

// log self callback
bool log_handler(const char* message)
{
	return false;
}

static void _mqtt_event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) {
	MQTTMessage* mqtt_messge = (MQTTMessage*)msg->msg;
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

		case MQTT_EVENT_PUBLISH_RECVEIVED:
			Log_i("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
					  mqtt_messge->topic_len,
					  mqtt_messge->ptopic,
					  mqtt_messge->payload_len,
					  mqtt_messge->payload);
			break;
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
			Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
			Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_NACK:
			Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
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


static void on_message_callback(void *pClient, MQTTMessage *message, void *userData) 
{
	if (message == NULL) {
        Log_e("msg null");
        return;
    }
    
    if (message->topic_len == 0 && message->payload_len == 0) {
        Log_e("length zero");
        return;
    }

    Log_d("recv msg topic: %s", message->ptopic);

    uint32_t msg_topic_len = message->payload_len + 4;
    char *buf = (char *)HAL_Malloc(msg_topic_len);
    if (buf == NULL) {
        Log_e("malloc %u bytes failed, topic: %s", msg_topic_len, message->ptopic);
        return;
    }

    memset(buf, 0, msg_topic_len);    
    memcpy(buf, message->payload, message->payload_len);

    printf("  msg payload: %s\n", buf);

    HAL_Free(buf);
}


static int _setup_connect_init_params(MQTTInitParams* initParams)
{
	initParams->device_name = QCLOUD_IOT_DEVICE_NAME;
	initParams->product_id = QCLOUD_IOT_PRODUCT_ID;
	initParams->device_secret = QCLOUD_IOT_DEVICE_SECRET;

	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->event_handle.h_fp = _mqtt_event_handler;
	initParams->event_handle.context = NULL;

    return QCLOUD_RET_SUCCESS;
}


static int _publish_msg(void *client, int qos)
{
    char topicName[128] = {0};
    sprintf(topicName,"%s/%s/%s", QCLOUD_IOT_PRODUCT_ID, QCLOUD_IOT_DEVICE_NAME, "data");

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos = qos;

    char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

	int size = HAL_Snprintf(topic_content, sizeof(topic_content), "{\"action\": \"publish_test\", \"count\": \"%d\"}", sg_count++);
	if (size < 0 || size > sizeof(topic_content) - 1)
	{
		Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
		return -3;
	}

	pub_params.payload = topic_content;
	pub_params.payload_len = strlen(topic_content);

    return IOT_MQTT_Publish(client, topicName, &pub_params);
}

static int _register_subscribe_topics(void *client, char *key_word, int qos)
{
    char topic_name[128] = {0};
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", QCLOUD_IOT_PRODUCT_ID, QCLOUD_IOT_DEVICE_NAME, key_word);
    if (size < 0 || size > sizeof(topic_name) - 1)
    {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.qos = qos;
    sub_params.on_message_handler = on_message_callback;
    return IOT_MQTT_Subscribe(client, topic_name, &sub_params);
}


int qcloud_iot_hub_demo(void) 
{
    //init log level
    IOT_Log_Set_Level(eLOG_DEBUG);
    IOT_Log_Set_Upload_Level(eLOG_DEBUG);
    IOT_Log_Set_MessageHandler(log_handler);
    
    int rc;
    int count = 0;
    int pub_interval = 2; 
    bool loop = true;
    int pub_qos = QOS0, sub_qos = QOS0;
    
    // init connection parameters
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
		return rc;
	}

#ifdef LOG_UPLOAD
    // IOT_Log_Init_Uploader should be done after _setup_connect_init_params
    LogUploadInitParams log_init_params = {.product_id = QCLOUD_IOT_PRODUCT_ID, 
                        .device_name = QCLOUD_IOT_DEVICE_NAME,
                        .read_func = NULL, .sign_key = QCLOUD_IOT_DEVICE_SECRET};
    IOT_Log_Init_Uploader(&log_init_params);
#endif
   
    // connect to MQTT server
    void *client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        IOT_Log_Upload(true);
        return QCLOUD_ERR_FAILURE;
    }

#ifdef SYSTEM_COMM
    long time = 0;

	rc = IOT_Get_SysTime(client, &time);
	if (QCLOUD_RET_SUCCESS == rc){
		Log_i("system time is %ld", time);
	}
	else{
		Log_e("get system time failed!");
	}
#endif

	//register subscribe topics here
    rc = _register_subscribe_topics(client, "data", sub_qos);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        IOT_Log_Upload(true);
        return rc;
    }
    
    rc = _register_subscribe_topics(client, "control", QOS0);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        IOT_Log_Upload(true);
        return rc;
    }

	rc = IOT_MQTT_Yield(client, 200);

    do {
		//wait for sub result
		if (sg_sub_packet_id > 0 && !(count%pub_interval)) {
            count = 0;
			rc = _publish_msg(client, pub_qos);
			if (rc < 0) {
				Log_e("client publish topic failed :%d.", rc);
			}
		}

    	rc = IOT_MQTT_Yield(client, 500);

		if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
			HAL_SleepMs(1000);
			continue;
		}
		else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED){
			Log_e("exit with error: %d", rc);            
			break;
		}

		HAL_SleepMs(1000);
        
        count++;

    } while (loop);


    rc = IOT_MQTT_Destroy(&client);
    
    IOT_Log_Upload(true);

    IOT_Log_Fini_Uploader();
    
    return rc;
}


