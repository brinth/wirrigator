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


void wifi_station_init(void);

void wifi_connect();

void wifi_disconnect();

#endif //__WIFI_STATION_H__
