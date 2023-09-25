/*
 * led_ctrl.c
 *
 *  Created on: 07-Sep-2023
 *      Author: bkhanna
 */
#include "led_ctrl.h"
#include "driver/gpio.h"

static led_conf_t _cfg;
static void run_led_task(void* arg);

bool led_ctrl_init(const led_conf_t *cfg) {
	if(cfg) {
		_cfg = *cfg;
		// Init all configured pins as output
		gpio_config_t io_cfg;
		io_cfg.intr_type = GPIO_INTR_DISABLE;
		io_cfg.mode = GPIO_MODE_OUTPUT;
		io_cfg.pin_bit_mask = (1ULL << _cfg.red) | (1ULL << _cfg.yellow) | (1ULL << _cfg.green);
		io_cfg.pull_down_en = true;
		io_cfg.pull_up_en = false;
		gpio_config(&io_cfg);
		return true;
	}
	return false;
}

void led_ctrl_start(void) {
	xTaskCreate(run_led_task, "LED Task", 4000, NULL, PRIORITY_LED_CTRL, NULL);
}

void run_led_task(void* arg) {
	gpio_set_level(_cfg.red, false);
	gpio_set_level(_cfg.yellow, false);
	gpio_set_level(_cfg.green, false);
	printf("Starting LED Task\n");
	// Blink All LEDs (Green -> Yellow -> Red)
	gpio_set_level(_cfg.green, true);
	sys_delay(1);
	gpio_set_level(_cfg.green, false);
	gpio_set_level(_cfg.yellow, true);
	sys_delay(1);
	gpio_set_level(_cfg.yellow, false);
	gpio_set_level(_cfg.red, true);
	sys_delay(1);
	for(;;) {
		EventBits_t wait_bits = 0xFFFF; // Wait for all events
		EventBits_t bits = xEventGroupWaitBits(_sys_events, wait_bits, pdTRUE, pdFALSE, portMAX_DELAY);
		if(bits & (EVENT_BIT(WIFI_CONNECTION_SUCCESS) | EVENT_BIT(FLOW_STARTED) | EVENT_BIT(OTA_UPGRADE_COMPLETE))) {
			gpio_set_level(_cfg.green, true);
			if(bits & EVENT_BIT(OTA_UPGRADE_COMPLETE)) gpio_set_level(_cfg.yellow, false);
		}
		if(bits & (EVENT_BIT(VALVE_OPENED) | EVENT_BIT(OTA_UPGRADE_START))) {
			gpio_set_level(_cfg.yellow, true);
		}
		if(bits & (EVENT_BIT(WIFI_CONNECTION_FAILURE) | EVENT_BIT(VALVE_CLOSED) | EVENT_BIT(FLOW_STOPPED) | EVENT_BIT(OTA_UPGRADE_ERROR))) {
			gpio_set_level(_cfg.red, true);
			if(bits & EVENT_BIT(FLOW_STOPPED)) gpio_set_level(_cfg.green, false);
		}
	}
}

