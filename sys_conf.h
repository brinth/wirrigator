#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"

// Comment out the below line to disable all debug prints
//#define DEBUG 1

#if defined(DEBUG)
#define DPRINTF(X...) printf(X)
#else
#define DPRINTF(X...)
#endif

#define SNTP_SERVER_NAME_0 				"pool.ntp.org"
#define SNTP_SERVER_NAME_1 				"asia.pool.ntp.org"
#define SNTP_SERVER_NAME_2 				"in.pool.ntp.org"
#define SNTP_RETRY_COUNT 				5

#define IRRIGATION_TIME_SEC				180 // 3 minutes
#define IRRIGATION_VOLUME_L				10  // 10 litre

#define MQTT_BROKER_IP					"192.168.1.10"
#define MQTT_BROKER_PORT				1886

enum operation_priority {
	PRIORITY_OTA = 10,
	PRIORITY_FLOW_METER = 20,
	PRIORITY_LED_CTRL = 30,
};

static inline void sys_delay_msec(uint16_t delay_ms) {
	vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

static inline void sys_delay(uint16_t delay) {
	vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);
}

#endif //__SYS_CONF_H__
