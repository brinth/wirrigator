/*
 * wifi_ota.c
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include "wifi_ota.h"
#include "../../sys_conf.h"

static void run_ota_task(void* arg);

void wifi_ota_start(void) {
  xTaskCreate(run_ota_task, "OTA Task", 4000, NULL, PRIORITY_OTA, NULL);
}


void run_ota_task(void* arg) {
  printf("WiFi OTA Task Started\n");
  for(;;) { 
    // Start webserver
    // Send LED Alert when OTA update is on progress
    sys_delay(1);
  }
}


