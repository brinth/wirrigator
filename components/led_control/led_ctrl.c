/*
 * led_ctrl.c
 *
 *  Created on: 07-Sep-2023
 *      Author: bkhanna
 */
#include "led_ctrl.h"
#include "../../sys_conf.h"

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
		gpio_config(&io_cfg);
		return true;
	}
	return false;
}

void led_ctrl_start(void) {
	xTaskCreate(run_led_task, "LED Task", 2000, NULL, PRIORITY_LED_CTRL, NULL);
}

void run_led_task(void* arg) {
	printf("Running LED Task\n");
	// Blink All LEDs (Green -> Yellow -> Red)
	gpio_set_level((1ULL << _cfg.green), true);
	sys_delay_ms(500);
	gpio_set_level((1ULL << _cfg.green), false);
	gpio_set_level((1ULL << _cfg.yellow), true);
	sys_delay_ms(500);
	gpio_set_level((1ULL << _cfg.yellow), false);
	gpio_set_level((1ULL << _cfg.red), true);
	sys_delay_ms(500);
	for(;;) {
		sys_delay(1);
	}
}

