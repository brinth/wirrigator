/*
 * led_ctrl.h
 *
 *  Created on: 07-Sep-2023
 *      Author: bkhanna
 */

#ifndef COMPONENTS_LED_CONTROL_LED_CTRL_H_
#define COMPONENTS_LED_CONTROL_LED_CTRL_H_

typedef struct led_conf {
	unsigned red;
	unsigned yellow;
	unsigned green;
}led_conf_t;

bool led_ctrl_init(const led_conf_t* cfg);

void led_ctrl_start(void);

#endif /* COMPONENTS_LED_CONTROL_LED_CTRL_H_ */
