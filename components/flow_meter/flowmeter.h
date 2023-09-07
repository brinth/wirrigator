/*
 * =====================================================================================
 *
 *       Filename:  flowmeter.h
 *
 *    Description:  Flow Meter component 
 *
 *        Version:  1.0
 *        Created:  01/07/23 05:01:09 PM IST
 *       Revision:  none
 *       Compiler:  xtensa-l106-elf-gcc
 *
 *         Author:  Brinth Khanna(brinthtest@gmail.com), 
 *
 * =====================================================================================
 */
#include <stdbool.h>

#ifndef __FLOWMETER_H_
#define __FLOWMETER_H_

typedef struct flow_meter_conf {
	unsigned pin;
	unsigned long ppl;
}flow_meter_conf_t;

typedef enum flow_meter_state {
	NO_FLOW_DETECTED,
	FLOW_DETECTED,
}flow_meter_state_t;

bool flow_meter_init(const flow_meter_conf_t *cfg);

void flow_meter_start(void);

flow_meter_state_t flow_meter_get_state(void);

long flow_meter_get_discharge(void);

long flow_meter_get_volume(void);

#endif // __FLOWMETER_H_
