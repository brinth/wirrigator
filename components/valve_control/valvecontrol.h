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

#include <stdlib.h>
#include <stdbool.h>

typedef enum valve_check {
	VALVE_VALIDATE,
	VALVE_NO_VALIDATE
} valve_check_e;

typedef enum valve_logic {
	VALVE_ACTIVE_LOW,
	VALVE_ACTIVE_HIGH
} valve_logic_e;

typedef struct valve_conf {
	unsigned short pin;
	valve_logic_e logic; 
	valve_check_e check;
	unsigned long def_ac_sec;
	bool init;
} valve_conf_t;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  valve_init
 *  Description: Sets the GPIO and other SW conf and initializes the Hardware
 * =====================================================================================
 */
bool valve_init(const valve_conf_t conf);

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

#endif // __VALVECONTROL_H
