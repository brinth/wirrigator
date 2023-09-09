#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"

// Comment out the below line to disable all debug prints
#define DEBUG 1

#if defined(DEBUG)
#define DPRINTF(X...) printf(X)
#else
#define DPRINTF(X...)
#endif

#define EVENT_BIT(pos)					(1U << pos)

#define STARTUP_DELAY					0

#define SNTP_SERVER_NAME_0 				"pool.ntp.org"
#define SNTP_SERVER_NAME_1 				"asia.pool.ntp.org"
#define SNTP_SERVER_NAME_2 				"in.pool.ntp.org"
#define SNTP_RETRY_COUNT 				5

#define IRRIGATION_TIME_SEC				5 // 3 minutes
#define IRRIGATION_VOLUME_L				10  // 10 litre

#define MQTT_BROKER_IP					"192.168.1.10"
#define MQTT_BROKER_PORT				1886

enum system_pin_desc {
	VALVE_PIN = 15,
	FLOW_METER_PIN = 13,
	RED_LED_PIN = 12,
	YELLOW_LED_PIN = 14,
	GREEN_LED_PIN = 16,
};

enum operation_priority {
	PRIORITY_OTA = 10,
	PRIORITY_FLOW_METER = 20,
	PRIORITY_LED_CTRL = 30,
};

enum event_bits_base {
	WIFI_EVENT_BASE = 0,
	OTA_EVENT_BASE = 8,
	VALVE_EVENT_BASE = 16,
	FLOW_EVENT_BASE = 20,
	MISC_EVENT_BASE = 24,
};

enum wifi_sta_events {
	WIFI_CONNECTION_FAILURE = WIFI_EVENT_BASE,
	WIFI_CONNECTION_CONNECTING,
	WIFI_CONNECTION_SUCCESS,
};

enum ota_events {
	OTA_UPGRADE_START = OTA_EVENT_BASE,
	OTA_UPGRADE_IN_PROGRESS,
	OTA_UPGRADE_COMPLETE,
};

enum valve_events {
	VALVE_CLOSED = VALVE_EVENT_BASE,
	VALVE_OPENED,
};

enum flow_events {
	FLOW_STOPPED = FLOW_EVENT_BASE,
	FLOW_STARTED,
};

extern EventGroupHandle_t				_sys_events;

inline void sys_delay_msec(uint16_t delay_ms) {
	vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

inline void sys_delay(uint16_t delay) {
	vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);
}

#endif //__SYS_CONF_H__
