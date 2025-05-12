/*
 * =====================================================================================
 *
 *       Filename:  wifi_station.h
 *
 *    Description:  Connect to Wifi AP as Station 
 *
 *        Version:  1.0
 *        Created:  30/07/23 01:54:43 PM IST
 *       Revision:  none
 *       Compiler:  xtensa-l106-elf-gcc
 *
 *         Author:  Brinth Khanna 
 *
 * =====================================================================================
 */

#ifndef __WIFI_STATION_H__
#define __WIFI_STATION_H__

typedef struct wifi_sta_conf {
	const char* ssid;
	const char* pass;
	unsigned	retry;
}wifi_sta_conf_t;

typedef enum wifi_sta_status {
	STA_DISCONNECTED,
	STA_CONNECTING,
	STA_CONNECTED
}wifi_sta_status_t;

#ifdef __cplusplus
extern "C" {
#endif 

bool						wifi_sta_init(const wifi_sta_conf_t *cfg);

bool						wifi_sta_connect(void);

bool						wifi_sta_disconnect(void);

wifi_sta_status_t			wifi_sta_status(void);

#ifdef __cplusplus
}
#endif

#endif //__WIFI_STATION_H__
