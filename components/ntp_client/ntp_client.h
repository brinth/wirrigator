/*
 * =====================================================================================
 *
 *       Filename:  ntp_client.h
 *
 *    Description: Connect to NTP Server and update Local Time 
 *
 *        Version:  1.0
 *        Created:  29/07/23 04:48:41 PM IST
 *       Revision:  none
 *       Compiler:  xtensa-l106-elf-gcc
 *
 *         Author: Brinth Khanna 
 *			email: brinthkhanna@gmail.com
 * 
 * =====================================================================================
 */
#ifndef __NTP_CLIENT_H__
#define __NTP_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

int	ntp_client_connect(const char* server_ip, unsigned port);

int	ntp_client_disconnect(void);

int	ntp_client_get_time(unsigned long long *time);

#ifdef __cplusplus
}
#endif

#endif // __NTP_CLIENT_H__ 
