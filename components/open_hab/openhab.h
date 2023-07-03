/*
 * =====================================================================================
 *
 *       Filename:  openhab.h
 *
 *    Description:  OpenHAB Connection component
 *
 *        Version:  1.0
 *        Created:  01/07/23 04:49:44 PM IST
 *       Revision:  none
 *       Compiler:  xtensa-lx106-elf-gcc
 *
 *         Author: Brinth Khanna(brinthtest@gmail.com) 
 *
 * =====================================================================================
 */
#ifndef __OPENHAB_H_
#define __OPENHAB_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum openhab_io {
	OPENHAB_INPUT,
	OPENHAB_OUTPUT,
	NUM_OPENHAB_IO
}openhab_io_e;


typedef struct openhab_conf {
	char* mqtt_broker;
	unsigned long mqtt_port;
	char* mqtt_topic_prefix;
	char* mqtt_qos;
}openhab_conf_t;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_init
 *  Description: Init OpenHAB Component
 * =====================================================================================
 */
bool openhab_init(const openhab_conf_t conf);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_register_int
 *  Description: Register Int variable for OpenHAB comm
 * =====================================================================================
 */
bool openhab_register_int(const openhab_io_e io_type, const int* var);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_register_float
 *  Description:  Register Float variable for OpenHAB comm
 * =====================================================================================
 */
bool openhab_register_float(const openhab_io_e io_type, const float* var);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_register_str
 *  Description: Register str variable for OpenHAB comm 
 * =====================================================================================
 */
bool openhab_register_str(const openhab_io_e io_type, const char* var, const unsigned long size);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_connect
 *  Description: Connect to MQTT Broker 
 * =====================================================================================
 */
bool openhab_connect(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_disconnect
 *  Description: Disconnect from MQTT Broker
 * =====================================================================================
 */
bool openhab_disconnect(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_register_cb
 *  Description: register callbacks for getting variable values 
 * =====================================================================================
 */
bool openhab_register_cb(openhab_cb cb, void* arg);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  openhab_service
 *  Description: OpenHAB MQTT service loop spawns a new task
 * =====================================================================================
 */
void openhab_service_start(void);

#endif //__OPENHAB_H_
