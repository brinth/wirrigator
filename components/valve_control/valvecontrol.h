/*
 * =====================================================================================
 *
 *       Filename:  valvecontrol.h
 *
 *    Description: Solenoid valve control operation Module 
 *
 *        Version:  1.0
 *        Created:  01/07/23 01:08:36 AM IST
 *       Revision:  none
 *       Compiler:  xtensa-lx106-elf-gcc
 *
 *         Author:  Brinth Khanna(brinthtest@gmail.com)
 *
 * =====================================================================================
 */
#ifndef __VALVECONTROL_H
#define __VALVECONTROL_H

#include "../../sys_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum valve_logic {
	VALVE_ACTIVE_LOW,
	VALVE_ACTIVE_HIGH
} valve_logic_e;

typedef struct valve_conf {
	unsigned short pin;
	valve_logic_e logic; 
} valve_conf_t;

typedef void (*valve_ops_cb) (int *ret);

typedef struct valve_ops {
	valve_ops_cb open;
	valve_ops_cb close;
} valve_ops_t;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_init
 *  Description: Sets the GPIO and other SW conf and initializes the Hardware
 * =====================================================================================
 */
bool valve_init(const valve_conf_t *conf);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_open
 *  Description: Asserts configured GPIO to open valve
 * =====================================================================================
 */
bool valve_open(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_close
 *  Description: Asserts configured GPIO to close valve 
 * =====================================================================================
 */
bool valve_close(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_open_ac
 *  Description: Asserts configured GPIO to open valve waits for given seconds
 *  			and auto closes the valve 
 * =====================================================================================
 */
bool valve_open_ac(const unsigned long sec);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_probe
 *  Description: Register callbacks to be invoked on any event 
 * =====================================================================================
 */
bool valve_probe(const valve_ops_t *ops);


#ifdef __cplusplus
}
#endif

#endif // __VALVECONTROL_H
