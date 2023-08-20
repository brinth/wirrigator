#include <time.h>
#include "lwip/apps/sntp.h"
#include "sys_manager.h"
#include "../../sys_conf.h"
#include "../../secret.h"
#include "wifi_station.h"
#include "valvecontrol.h"
#include "flowmeter.h"

static void							cb_valve_open(int *);
static void							cb_valve_close(int *);

namespace System {

Manager* Manager::_inst = NULL;

TaskHandle_t IrrigationManager :: _flow_meter_task_handle;
TaskHandle_t IrrigationManager :: _mqtt_task_handle;
TaskHandle_t IrrigationManager :: _sntp_task_handle;

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

	// Connect to WiFi
	wifi_sta_conf_t w_cfg;
	w_cfg.ssid = CONFIG_WIFI_SSID;
	w_cfg.pass = CONFIG_WIFI_PASS;
	w_cfg.retry = 10;
	if(!wifi_sta_init(&w_cfg)) {
		printf("ERR: Initializing Wifi STA SSID(%s)\n", CONFIG_WIFI_SSID);
	}
	if(!wifi_sta_connect()) {
		printf("ERR: WiFI STA Not Connected\n");
	}
	// Flow meter 
	xTaskCreate(run_flow_meter_task, "FlowMeter_task", 512, this, 5, &_flow_meter_task_handle);
	// OpenHAB Connection (MQTT)
	xTaskCreate(run_mqtt_task, "MQTT_task", 512, this, 10, &_mqtt_task_handle);
	// NTP Task
	xTaskCreate(run_sntp_task, "SNTP_task", 512, this, 20, &_sntp_task_handle);
	vTaskStartScheduler();
}

void IrrigationManager :: Service(void) {
	DPRINTF("%s() Servicing...\n", __FUNCTION__);
	bool valve_control_req = true;
	uint8_t countdown_sec = 180;
	time_t cur_time;
	struct tm cur_time_info;
	time(&cur_time);
	localtime_r(&cur_time, &cur_time_info);
	//if(cur_time_info->tm_hour == 6 || cur_time_info->tm_hour == 18) {
	if(cur_time_info.tm_sec == 0 || cur_time_info.tm_sec == 30) { // Just for Debug
		valve_open();
		do {
			valve_control_req = true;
			sys_delay(1);
			countdown_sec--;
		} while(valve_control_req && countdown_sec != 0);
		valve_close();
	}
}

void IrrigationManager :: run_flow_meter_task (void* arg) {
	
	for(;;) {
		sys_delay(1);	
	}
}

void IrrigationManager :: run_mqtt_task (void* arg) {
	valve_ops_t v_ops;
	v_ops.open = cb_valve_open;
	v_ops.close = cb_valve_close;
	valve_probe(&v_ops);
	for(;;) {
		// If command is passed through MQTT update valve status
		sys_delay(1);
	}
}

void IrrigationManager :: run_sntp_task (void* arg) {
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, SNTP_SERVER_NAME);
	sntp_init();
	setenv("TZ", "IST", 1);
	tzset();
	for(;;) {
		printf("INFO: Updating local time from NTP Server(%s)\n", SNTP_SERVER_NAME); 	
		time_t now = 0;
		struct tm timeinfo = {0};
		unsigned retry = 0;
		while(timeinfo.tm_year < (2016 - 1900) && ++retry < SNTP_RETRY_COUNT) {
			printf("INFO: Waiting for system time to be set... (%d)\n", retry);
			sys_delay(2);
			time(&now);
			localtime_r(&now, &timeinfo);
		}
		sys_delay(SNTP_OBTAIN_TIME_INTERVAL_SEC);
	}
}

} //namespace System

void cb_valve_open(int* ret) {
	(void) ret;
	//Update MQTT topic
	printf("MQTT Publish: Valve Open update\n");
}

void cb_valve_close(int* ret) {
	(void) ret;
	//Update MQTT topic
	printf("MQTT Publish: Valve Close update\n");
}

