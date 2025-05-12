#include "flowmeter.h"
#include "../../sys_conf.h"
#include "driver/gpio.h"

static TaskHandle_t				_flow_task_handle;
static flow_meter_conf_t 		_cfg;
static flow_meter_state_t 		_state;
static long 					_discharge;
static long						_volume;
static unsigned long long 		flow_pulse_count;
static unsigned long long		prev_pulse_count;
static xQueueHandle 			flow_pulse_queue = NULL;

static void flow_intr_handler(void* arg);
static void run_flow_meter_task(void* arg);

bool flow_meter_init(const flow_meter_conf_t* cfg) {
	if(cfg) {
		_cfg = *cfg;
		// Configure pin as interrupt
		gpio_config_t io_cfg;
		io_cfg.intr_type = GPIO_INTR_POSEDGE;
		io_cfg.pin_bit_mask = _cfg.pin;
		io_cfg.mode = GPIO_MODE_INPUT;
		io_cfg.pull_up_en = 1;
		gpio_config(&io_cfg);
		// Create event queue
		flow_pulse_queue = xQueueCreate(10, sizeof(uint32_t));
		gpio_install_isr_service(0);
		gpio_isr_handler_add(_cfg.pin, flow_intr_handler, (void*)_cfg.pin);
		return true;
	}
	return false;
}

void flow_meter_start(void) {
	xTaskCreate(run_flow_meter_task, "FlowMeter Task", 4000, NULL, PRIORITY_FLOW_METER, &_flow_task_handle);
}

void flow_meter_stop(void) {
	vTaskDelete(&_flow_task_handle);
}

flow_meter_state_t flow_meter_get_state(void) {
	return _state;
}

long flow_meter_get_discharge(void) {
	return _discharge;
}

long flow_meter_get_volume(void) {
	return _volume;
}

void flow_intr_handler(void* arg) {
	++flow_pulse_count;
	xQueueSendFromISR(flow_pulse_queue, &flow_pulse_count, NULL);
}

void run_flow_meter_task(void* arg) {
	unsigned long long pulse_count = 0;
	unsigned pulse_count_trigger = 2;
	printf("Starting Flow Meter Task\n");
	for(;;) {
		if(xQueueReceive(flow_pulse_queue, &pulse_count, portMAX_DELAY)) {
			if(pulse_count - prev_pulse_count > pulse_count_trigger) {
				printf("Flow Detected\n");
				xEventGroupSetBits(_sys_events, EVENT_BIT(FLOW_STARTED));
				//Send Event, Measure Flow & Volume
			}
			xEventGroupSetBits(_sys_events, EVENT_BIT(FLOW_STOPPED));
		}
	}
}
