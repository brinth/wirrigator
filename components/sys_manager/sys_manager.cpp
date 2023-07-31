#include "sys_manager.h"
#include "../../sys_conf.h"
#include "ntp_client.h"
#include "valvecontrol.h"
#include "flowmeter.h"
#include "openhab.h"

#define NTP_SERVER_IP			"10.10.10.10"
#define NTP_SERVER_PORT			300

namespace System {

Manager* Manager::_inst = NULL;

Manager* Manager::GetInstance(void) {
	if(_inst == NULL)
		_inst = new IrrigationManager();
	return _inst;
}


void IrrigationManager::Init(void) {
	DPRINTF("%s() Initializing...\n", __FUNCTION__);	

	// Configure Solenoid Valve
	valve_conf_t v_cfg;
	v_cfg.pin = 2; // ESP8266 GPIO02 is connected to D4 pin of Wemos D1 mini
	v_cfg.logic = VALVE_ACTIVE_HIGH;
	if(!valve_init(&v_cfg)) {
		printf("ERR: %s() Initializing Valve failed\n", __FUNCTION__);
	}
	// Configure Flow Meter
/*
	// Update Local Time 
	if(!ntp_client_connect(NTP_SERVER_IP, NTP_SERVER_PORT)) {
		printf("ERR: %s() Unable to connect to NTP server %s, port %u\n", __FUNCTION__, NTP_SERVER_IP, NTP_SERVER_PORT);
	}
	unsigned long long local_time;
	ntp_client_get_time(&local_time);
*/
}

void IrrigationManager::Service(void) {
	DPRINTF("%s() Servicing...\n", __FUNCTION__);
//-- DEBUG START
	valve_open();
	sys_delay(5);
	valve_close();
	sys_delay(5);
//-- DEBUG END
	/* When time is morning 6.am irrigate for 3 mins 
	 *   or time is eveining 6.pm irrigate for 3 mins
	*/
}

} //namespace System

