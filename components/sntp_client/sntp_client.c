/*
 * sntp_client.c
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */


void sntp_update_localtime(void) {
	unsigned		retry		= 0;
	char			buf[64]		= {};
	time_t			now			= 0;
	struct tm		timeinfo	= {};
	sntp_stop();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, SNTP_SERVER_NAME_0);
	sntp_setservername(1, SNTP_SERVER_NAME_1);
	sntp_setservername(2, SNTP_SERVER_NAME_2);
	sntp_init();
	setenv("TZ", "UTC-05:30", 1); //IST
	tzset();
	//printf("INFO: Updating local time from NTP Server\n");
	while(timeinfo.tm_year < (2023 - 1900) && ++retry < SNTP_RETRY_COUNT) {
		printf("INFO: Waiting for system time to be set... (%d)\n", retry);
		sys_delay(2);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
	strftime(buf, sizeof(buf), "%c", &timeinfo);
	printf("INFO: Localtime updated to %s\n", buf);
}
