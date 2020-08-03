/*
 * wifi.h
 *
 *  Created on: 3 Aug 2020
 *      Author: Bas
 */

#ifndef MAIN_JBVLIB_WIFI_H_
#define MAIN_JBVLIB_WIFI_H_

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "string.h"
#include "ESP_log.h"


class Wifi
{
	static esp_err_t event_handler(void *ctx, system_event_t *event)
	{
	    return ESP_OK;
	}

public:

	static void Connect()
	{
	    tcpip_adapter_init();
		ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
		ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
		ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

		wifi_config_t sta_config = {};

		memcpy(sta_config.sta.ssid, "vasBassum", 10);
		memcpy(sta_config.sta.password, "pedaalemmerzak", 15);


		sta_config.sta.bssid_set = false;

		ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
		ESP_ERROR_CHECK( esp_wifi_start() );
		ESP_ERROR_CHECK( esp_wifi_connect() );
	}

};








#endif /* MAIN_JBVLIB_WIFI_H_ */
