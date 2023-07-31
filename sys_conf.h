#ifndef __SYS_CONF_H__
#define __SYS_CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Comment out the below line to disable all debug prints
#define DEBUG 1

#if defined(DEBUG)
#define DPRINTF(X...) printf(X)
#else
#define DPRINTF(X...)
#endif

static inline void sys_delay_ms(uint16_t delay_ms) {
	vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

static inline void sys_delay(uint8_t delay) {
	vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);
}

#endif //__SYS_CONF_H__
