#include "sys_manager.h"
#include "esp_event_loop.h"
#include "../../sys_conf.h"
#include "../../secret.h"
#include "wifi_station.h"
#include "sntp_client.h"
#include "wifi_ota.h"
#include "mqtt_client.h"
#include "valvecontrol.h"
#include "flowmeter.h"
#include "led_ctrl.h"

EventGroupHandle_t _sys_events = NULL;

namespace System {

Manager* Manager::_inst = NULL;

Manager* Manager::GetInstance(void) {
	if(_inst == NULL)
		_inst = new IrrigationManager();
	return _inst;
}


void IrrigationManager :: Init(void) {
	DPRINTF("%s() Initializing...\n", __FUNCTION__);
	
	// Create Event loop for consequent register
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	_sys_events = xEventGroupCreate();

	// Configure Solenoid Valve
	valve_conf_t v_cfg;
	v_cfg.pin = VALVE_PIN;
	v_cfg.logic = VALVE_ACTIVE_HIGH;
	if(!valve_init(&v_cfg)) {
		printf("ERR: Initializing Valve failed\n");
	}

	// Configure Flow Meter
	flow_meter_conf_t f_cfg;
	f_cfg.pin = FLOW_METER_PIN; 
	f_cfg.ppl = 1000; // Pulse per litre
	if(!flow_meter_init(&f_cfg)) {
		printf("ERR: Initializing Flow Meter failed\n");
	}
	flow_meter_start();

	// Led Control
	led_conf_t l_cfg;
	l_cfg.red = RED_LED_PIN;
	l_cfg.yellow = YELLOW_LED_PIN;
	l_cfg.green = GREEN_LED_PIN;
	if(!led_ctrl_init(&l_cfg)) {
		printf("ERR: Initializing LED failed\n");
	}
	led_ctrl_start();

	// Connect to WiFi
	wifi_sta_conf_t w_cfg;
	w_cfg.ssid = CONFIG_WIFI_SSID;
	w_cfg.pass = CONFIG_WIFI_PASS;
	w_cfg.retry = 10;
	if(!wifi_sta_init(&w_cfg)) {
		printf("ERR: Initializing Wifi STA SSID(%s)\n", CONFIG_WIFI_SSID);
	}
	if(wifi_sta_connect()) {
		sntp_update_localtime();
		// TODO(Brinth): Fix FOTA update breaking normal operation before May30?
		//wifi_ota_start();
		//mqtt_client_start(MQTT_BROKER_IP, MQTT_BROKER_PORT);
	} else {
		printf("ERR: WiFI STA Not Connected\n");
	}
}

void IrrigationManager :: Service(void) {
	//xEventGroupWaitBits(_sys_events, EVENT_BIT(OTA_UPGRADE_START), pdTRUE, pdFALSE, portMAX_DELAY);
	struct tm ctime;
	if(wifi_sta_status() == STA_CONNECTED) {
		sntp_get_localtime(&ctime);
		//if(cur_time_info->tm_hour == 6 || cur_time_info->tm_hour == 18) {
		if(ctime.tm_sec == 0 || ctime.tm_sec == 30) { // Just for Debug
			valve_open();
			sys_delay(IRRIGATION_TIME_SEC);
			valve_close();
		}
	} else printf("ERR: WiFi STA Not Connected\n");
}

} //namespace System

