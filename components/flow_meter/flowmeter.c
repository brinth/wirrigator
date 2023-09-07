#include "flowmeter.h"
#include "freertos/queue.h"
#include "../../sys_conf.h"

static flow_meter_conf_t _cfg;
static unsigned long long flow_pulse_count;
static xQueueHandle flow_pulse_queue = NULL;
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
	xTaskCreate(run_flow_meter_task, "FlowMeter Task", 2000, NULL, PRIORITY_FLOW_METER, NULL);
}

void flow_intr_handler(void* arg) {
	void (arg);
	xQueueSendFromISR(flow_pulse_queue, &(++flow_pulse_count), NULL);
}

void run_flow_meter_task(void* arg) {
	void (arg);
	printf("Starting Flow Meter Task\n");
	unsigned pulse_count_trigger = 2;
	unsigned long long prev_pulse_count;
	unsigned long long pulse_count;
	for(;;) {
		if(xQueueReceive(flow_pulse_queue, &pulse_count, portMAX_DELAY)) {
			if(pulse_count - prev_pulse_count > pulse_count_trigger) {
				printf("Flow Detected\n");
				//Send Event & Measure Flow
			}
		}
	}
}
