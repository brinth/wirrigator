/*
 * mqtt_client.h
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */

#ifndef COMPONENTS_MQTT_CLIENT_MQTT_CLIENT_H_
#define COMPONENTS_MQTT_CLIENT_MQTT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_client_start(const char* broker, const unsigned port);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_MQTT_CLIENT_MQTT_CLIENT_H_ */
