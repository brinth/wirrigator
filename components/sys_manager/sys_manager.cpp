#include "sys_manager.h"
#include "../../sys_conf.h"
#include "../../secret.h"
#include "wifi_station.h"
#include "sntp_client.h"
#include "wifi_ota.h"
#include "mqtt_client.h"
#include "valvecontrol.h"
#include "flowmeter.h"
#include "led_ctrl.h"

namespace System {

Manager* Manager::_inst = NULL;

Manager* Manager::GetInstance(void) {
	if(_inst == NULL)
		_inst = new IrrigationManager();
	return _inst;
}


void IrrigationManager :: Init(void) {
	DPRINTF("%s() Initializing...\n", __FUNCTION__);
	
	// Configure Solenoid Valve
	valve_conf_t v_cfg;
	v_cfg.pin = 2; // ESP8266 GPIO02 is connected to D4 pin of Wemos D1 mini
	v_cfg.logic = VALVE_ACTIVE_HIGH;
	if(!valve_init(&v_cfg)) {
		printf("ERR: Initializing Valve failed\n");
	}

	// Configure Flow Meter
	flow_meter_conf_t f_cfg;
	f_cfg.pin = 10; // ESP8266 pin
	f_cfg.ppl = 1000; // Pulse per litre
	if(!flow_meter_init(&f_cfg)) {
		printf("ERR: Initializing Flow Meter failed\n");
	}
	flow_meter_start();

	// Led Control
	led_conf_t l_cfg;
	l_cfg.red = 0;
	l_cfg.yellow = 1;
	l_cfg.green = 2;
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
		wifi_ota_start();
		sntp_update_localtime();
		mqtt_client_start(MQTT_BROKER_IP, MQTT_BROKER_PORT);
	} else {
		printf("ERR: WiFI STA Not Connected\n");
		// Send LED Warning
	}
	vTaskStartScheduler();
}

void IrrigationManager :: Service(void) {
	DPRINTF("%s() Servicing...\n", __FUNCTION__);
	bool valve_control_req = true;
	time_t cur_time;
	struct tm cur_time_info;
	time(&cur_time);
	localtime_r(&cur_time, &cur_time_info);
	//if(cur_time_info->tm_hour == 6 || cur_time_info->tm_hour == 18) {
	if(cur_time_info.tm_sec == 0 || cur_time_info.tm_sec == 30) { // Just for Debug
		valve_open();
		// Send LED Alert when Flow is not detected after opening valve
		sys_delay(IRRIGATION_TIME_SEC);
		valve_close();
		// Send LED Alert when Flow is detected even after closing valve
	}
}

} //namespace System

