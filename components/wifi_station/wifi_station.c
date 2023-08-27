#include <string.h>
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/event_groups.h"
#include "../../sys_conf.h"
#include "wifi_station.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT	BIT1

static bool						_init;
static wifi_sta_conf_t			_cfg;
static wifi_config_t 			_wifi_config;
static EventGroupHandle_t 		_wifi_events;
static unsigned 				_retry_count;

static void event_handler(void*, esp_event_base_t, int32_t, void*);

bool	wifi_sta_init(const wifi_sta_conf_t *cfg) {
	if(!cfg) {
		printf("ERR %s: Invalid arg %p\n", __FUNCTION__, cfg);
		return false;
	}
	_cfg = *cfg;
	if(strlen(_cfg.ssid) <= 0 || strlen(_cfg.pass) <= 0) {
		printf("ERR(%s): Invalid ssid or password\n", __FUNCTION__);
		return false;
	}
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	wifi_init_config_t wifi_init_cfg  = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));
	strncpy((char*)_wifi_config.sta.ssid, _cfg.ssid, strlen(_cfg.ssid));
	strncpy((char*)_wifi_config.sta.password, _cfg.pass, strlen(_cfg.pass));
	if(strlen((char*) _wifi_config.sta.password)) {
		_wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	}
	printf("INFO: Wifi STA Init complete...\n");
	_init = true;
	return true;
}

bool	wifi_sta_connect(void) {	
	bool ret = false;
	if(!_init) {
		printf("ERR %s(): WiFi STA not initialized\n", __FUNCTION__);
		return ret;
	}
	_wifi_events = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &_wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	EventBits_t bits = xEventGroupWaitBits(_wifi_events,
			WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
			pdFALSE,
			pdFALSE,
			portMAX_DELAY);
	if(bits & WIFI_CONNECTED_BIT) { // WiFi Connected
		printf("WiFi STA Connected to SSID %s\n", _cfg.ssid);
		ret = true;
	} else if(bits & WIFI_FAIL_BIT) {
		printf("WiFi STA Failed to Connect to SSID %s\n", _cfg.ssid);
		ret = false;
	} else {
		printf("WiFi STA Unexpected event\n");
		ret = false;
	}
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
	vEventGroupDelete(_wifi_events);
	return ret;
}

bool 	wifi_sta_disconnect(void) {
	// NOP	
	return true;
}

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if(_retry_count < _cfg.retry) {
			esp_wifi_connect();
			_retry_count++;
			printf("INFO: WiFi STA trying to reconnect (SSID: %s) ...\n", _cfg.ssid);
		} else {
		xEventGroupSetBits(_wifi_events, WIFI_FAIL_BIT);
		}
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		printf("INFO: WiFi STA got IP (%s)\n", ipaddr_ntoa(&event->ip_info.ip));
		_retry_count = 0;
		xEventGroupSetBits(_wifi_events, WIFI_CONNECTED_BIT);
	}
	printf("ERR: Connecting to WiFi (SSID: %s)\n", _cfg.ssid);
}
