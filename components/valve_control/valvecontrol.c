#include "valvecontrol.h"
#include "driver/gpio.h"

#define VALVE_GPIO_PIN 10

static valve_conf_t _cfg;
static gpio_config_t _gpio_cfg;

bool valve_init(const valve_conf_t cfg) {	
	// Setup cofig
	_cfg = cfg;
	_cfg.init = true;
	_gpio_cfg.intr_type = GPIO_INTR_DISABLE;
	_gpio_cfg.mode = GPIO_MODE_OUTPUT;
	_gpio_cfg.pin_bit_mask = VALVE_GPIO_PIN;
	_gpio_cfg.pull_down_en = 0;
	_gpio_cfg.pull_up_en = 0;
	gpio_config(&_gpio_cfg);
	return true;
}

bool valve_open(void) {
	// Assert GPIO 
	if(!_cfg.init) return false;
	int ret = gpio_set_level((_cfg.level) ? true : false);
	return (ret == ESP_OK) ? true : false;
}

bool valve_close(void) {
	// Assert GPIO
	if(!_cfg.init) return false;
	int ret = gpio_set_level((_cfg.level) ? false : true);
	return (ret == ESP_OK) ? true : false;
}

bool valve_open_ac(const unsigned long sec) {
	if(!_cfg.init) return false;
	valve_open();
	// Start Timer 
	time.sleep(sec);
	valve_close();
	return true;
}
