#include <time.h>
#include "esp_netif.h"
#include "esp_event.h"
#include "lwip/apps/sntp.h"
#include "sys_manager.h"
#include "../../sys_conf.h"
#include "../../secret.h"
#include "wifi_station.h"
#include "valvecontrol.h"
#include "flowmeter.h"

static void							cb_valve_open(int *);
static void							cb_valve_close(int *);
static void							update_localtime(void);

namespace System {

Manager* Manager::_inst = NULL;

TaskHandle_t IrrigationManager :: _flow_meter_task_handle;
TaskHandle_t IrrigationManager :: _mqtt_task_handle;

Manager* Manager::GetInstance(void) {
	if(_inst == NULL)
		_inst = new IrrigationManager();
	return _inst;
}


void IrrigationManager :: Init(void) {
	bool wifi_connected = false;
	DPRINTF("%s() Initializing...\n", __FUNCTION__);
	
	// Configure Solenoid Valve
	valve_conf_t v_cfg;
	v_cfg.pin = 2; // ESP8266 GPIO02 is connected to D4 pin of Wemos D1 mini
	v_cfg.logic = VALVE_ACTIVE_HIGH;
	if(!valve_init(&v_cfg)) {
		printf("ERR: Initializing Valve failed\n");
	}
	valve_ops_t v_ops;
	v_ops.open = cb_valve_open;
	v_ops.close = cb_valve_close;
	valve_probe(&v_ops);
	// Configure Flow Meter

	// Connect to WiFi
	wifi_sta_conf_t w_cfg;
	w_cfg.ssid = CONFIG_WIFI_SSID;
	w_cfg.pass = CONFIG_WIFI_PASS;
	w_cfg.retry = 10;
	if(!wifi_sta_init(&w_cfg)) {
		printf("ERR: Initializing Wifi STA SSID(%s)\n", CONFIG_WIFI_SSID);
	}
	if(!(wifi_connected = wifi_sta_connect())) {
		printf("ERR: WiFI STA Not Connected\n");
	}
	if(wifi_connected) update_localtime();
	// Flow meter 
	xTaskCreate(run_flow_meter_task, "FlowMeter_task", 5000, this, 5, &_flow_meter_task_handle);
	// OpenHAB Connection (MQTT)
	xTaskCreate(run_mqtt_task, "MQTT_task", 5000, this, 10, &_mqtt_task_handle);
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
		printf("INFO: Running Flow Meter Task...\n");
		sys_delay(1);	
	}
}

void IrrigationManager :: run_mqtt_task (void* arg) {
	for(;;) {
		// If command is passed through MQTT update valve status
		printf("INFO: Running MQTT Task...\n");
		sys_delay(1);
	}
}


} //namespace System

void update_localtime(void) {
	unsigned		retry		= 0;
	char			buf[64]		= {};
	time_t			now			= 0;
	struct tm		timeinfo	= {};
	sntp_stop();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, SNTP_SERVER_NAME_0); 
	sntp_setservername(1, SNTP_SERVER_NAME_1);
	sntp_setservername(2, SNTP_SERVER_NAME_2);
	sntp_init();
	setenv("TZ", "UTC-05:30", 1); //IST
	tzset();
	//printf("INFO: Updating local time from NTP Server\n");
	while(timeinfo.tm_year < (2023 - 1900) && ++retry < SNTP_RETRY_COUNT) {
		printf("INFO: Waiting for system time to be set... (%d)\n", retry);
		sys_delay(2);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
	strftime(buf, sizeof(buf), "%c", &timeinfo);
	printf("INFO: Localtime updated to %s\n", buf);
}

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

