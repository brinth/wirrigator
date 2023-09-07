/*
 * sntp_client.h
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include <time.h>
#include "esp_netif.h"
#include "esp_event.h"
#include "lwip/apps/sntp.h"

#ifndef COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_
#define COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_

void sntp_update_localtime(void);

#endif /* COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_ */
