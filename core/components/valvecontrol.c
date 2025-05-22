#include "valvecontrol.h"
#include "../../sys_conf.h"
#include "driver/gpio.h"

static bool					_init;
static valve_conf_t			_cfg;
static gpio_config_t		_gpio_cfg;
static valve_ops_t			_ops;

bool valve_init(const valve_conf_t *cfg) {	
	_init = false;
	// Setup cofig
	if(cfg) {
		_cfg = *cfg;
		_gpio_cfg.intr_type = GPIO_INTR_DISABLE;
		_gpio_cfg.mode = GPIO_MODE_OUTPUT;
		_gpio_cfg.pin_bit_mask = (1ULL << _cfg.pin);
		_gpio_cfg.pull_down_en = _cfg.logic;
		_gpio_cfg.pull_up_en = !_cfg.logic;
		gpio_config(&_gpio_cfg);
		DPRINTF("%s() Valve Initialized\n", __FUNCTION__);
		_init = true;
	}
	return _init;
}

bool valve_open(void) {
	// Assert GPIO 
	if(!_init) return false;
	DPRINTF("%s() Opening Valve -< >-\n", __FUNCTION__);
	int ret = gpio_set_level(_cfg.pin, (_cfg.logic == VALVE_ACTIVE_LOW) ? true : false);
	xEventGroupSetBits(_sys_events, EVENT_BIT(VALVE_OPENED));
	if(_ops.open) _ops.open(&ret);
	return (ret == ESP_OK) ? true : false;
}

bool valve_close(void) {
	// Assert GPIO
	if(!_init) return false;
	DPRINTF("%s() Closing Valve -<>-\n", __FUNCTION__);
	int ret = gpio_set_level(_cfg.pin, (_cfg.logic == VALVE_ACTIVE_LOW) ? false : true);
	xEventGroupSetBits(_sys_events, EVENT_BIT(VALVE_CLOSED));
	if(_ops.close) _ops.close(&ret);
	return (ret == ESP_OK) ? true : false;
}

bool valve_open_ac(const unsigned long sec) {
	if(!_init) return false;
	// TODO: Make it an async operation
	valve_open();
	sys_delay(sec);
	valve_close();
	return true;
}

bool valve_probe(const valve_ops_t *ops) {
	if (ops) _ops = *ops;
	return !!(ops);
}
