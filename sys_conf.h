#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

// Comment out the below line to disable all debug prints
//#define DEBUG 1

#if defined(DEBUG)
#define DPRINTF(X...) printf(X)
#else
#define DPRINTF(X...)
#endif

#define SNTP_SERVER_NAME "pool.ntp.org"
#define SNTP_RETRY_COUNT 5
#define SNTP_OBTAIN_TIME_INTERVAL_SEC	(uint16_t)(60 * 60 * 2)  //Update NTP Time for every 2 hour

static inline void sys_delay_msec(uint16_t delay_ms) {
	vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

static inline void sys_delay(uint16_t delay) {
	vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);
}

#endif //__SYS_CONF_H__
