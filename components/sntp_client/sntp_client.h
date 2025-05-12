/*
 * sntp_client.h
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include <time.h>
#include "lwip/apps/sntp.h"

#ifndef COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_
#define COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

void sntp_update_localtime(void);

bool sntp_get_localtime(struct tm *tm);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_SNTP_CLIENT_SNTP_CLIENT_H_ */
