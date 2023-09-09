/*
 * mqtt_client.cpp
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include "mqtt_client.h"
#include "../../sys_conf.h"

void mqtt_client_start(const char* broker, const unsigned port) {
	(void) broker;
	(void) port;
	printf("Started MQTT Client\n");
}
