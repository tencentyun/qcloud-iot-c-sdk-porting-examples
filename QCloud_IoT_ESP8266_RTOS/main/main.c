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
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"

#include "esp_event_loop.h"
#include "esp_log.h"

#include "qcloud_iot_export.h"
#include "qcloud_iot_demo.h"
#include "board_ops.h"


static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;

static EventGroupHandle_t wifi_event_group;

#if CONFIG_DEMO_WIFI_SMARTCONFIG
static void _smartconfig_event_cb(smartconfig_status_t status, void *pdata)
{
    switch (status) {
        case SC_STATUS_WAIT:
            Log_i("SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            Log_i("SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            Log_i("SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            if (pdata) {
                wifi_config_t *wifi_config = pdata;
                Log_i( "SC_STATUS_LINK SSID:%s PSD: %s", wifi_config->sta.ssid, wifi_config->sta.password);

                int ret = esp_wifi_disconnect();
                if( ESP_OK != ret ) {
                    Log_e("esp_wifi_disconnect failed: %d", ret);
                }
                
                ret = esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config);
                if( ESP_OK != ret ) {
                    Log_e("esp_wifi_set_config failed: %d", ret);
                }
                
                ret = esp_wifi_connect();
                if( ESP_OK != ret ) {
                    Log_e("esp_wifi_connect failed: %d", ret);
                }
            }else {
                Log_e("invalid smart config link data");
            }
            break;
        case SC_STATUS_LINK_OVER:
            Log_w( "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = { 0 };
                memcpy(phone_ip, (uint8_t* )pdata, 4);
                Log_i( "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            
            xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
            break;
        default:
            break;
    }
}
#endif

bool wait_for_wifi_ready(int event_bits, uint32_t wait_cnt, uint32_t BlinkTime)
{
    EventBits_t uxBits;
    uint32_t cnt = 0;
    uint8_t blueValue = 0;

    while (cnt++ < wait_cnt) {
        uxBits = xEventGroupWaitBits(wifi_event_group, event_bits, true, false, BlinkTime/portTICK_RATE_MS);

        if(uxBits & CONNECTED_BIT) {            
            Log_d("WiFi Connected to AP");
            return true;
        }
        
        if(uxBits & ESPTOUCH_DONE_BIT) {
            /* recv smartconfig stop event before connected */
            Log_w("smartconfig over");            
            esp_smartconfig_stop();
            break;
        }

        blueValue = (~blueValue)&0x01;
        set_wifi_led_state(blueValue);
    }
    
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT);
    
    return false;
}


static void wifi_connection(void)
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = TEST_WIFI_SSID,
            .password = TEST_WIFI_PASSWORD,
        },
    };
    Log_i("Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    esp_wifi_connect();
}

static esp_err_t _esp_event_handler(void* ctx, system_event_t* event)
{
    Log_i("event = %d", event->event_id);

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            Log_i("SYSTEM_EVENT_STA_START");            
            wifi_connection();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            Log_i("Got IPv4[%s]", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            Log_i("SYSTEM_EVENT_STA_DISCONNECTED");
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            esp_wifi_connect();
            break;

        default:
            break;
    }

    return ESP_OK;
}

static void esp_wifi_initialise(void)
{
    tcpip_adapter_init();

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(_esp_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void qcloud_demo_task(void* parm)
{    
    bool wifi_connected = false;
    Log_i("qcloud_demo_task start");
    
    /* waiting for wifi connection with expected BSS */
    /* 20 * 1000ms */
    wifi_connected = wait_for_wifi_ready(CONNECTED_BIT, 20, 1000);

#if CONFIG_DEMO_WIFI_SMARTCONFIG
    if (!wifi_connected) {        
        /* WiFi not connected, start smartconfig */
        int ret = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
        if( ESP_OK != ret ) {
            Log_e("esp_smartconfig_set_type failed: %d", ret);
            goto TASK_QUIT;
        }
        
        ret = esp_smartconfig_start(_smartconfig_event_cb);
        if( ESP_OK != ret ) {
            Log_e("esp_smartconfig_start failed: %d", ret);
            goto TASK_QUIT;
        }

        /* 100 * 500ms */
        wifi_connected = wait_for_wifi_ready(CONNECTED_BIT, 100, 500);

        esp_smartconfig_stop();
    }
#endif    
    
    if (wifi_connected) {
        Log_i("WiFi is ready, to do Qcloud IoT demo");
#ifdef CONFIG_QCLOUD_IOT_EXPLORER_ENABLED
        qcloud_iot_explorer_demo();
#else
        qcloud_iot_hub_demo();
#endif
    } else {
        Log_e("WiFi is not ready, please check configuration");
    }           

TASK_QUIT:
    Log_w("qcloud_demo_task quit");
    vTaskDelete(NULL);
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    //init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    board_init();

    esp_wifi_initialise();

    xTaskCreate(qcloud_demo_task, "qcloud_demo_task", 16384, NULL, 3, NULL);
}
