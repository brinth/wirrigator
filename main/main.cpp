#include "esp_system.h"
#include "esp_console.h"
#include "sys_manager.h"
#include "valvecontrol.h"
#include "flowmeter.h"
#include "openhab.h"


void app_main() {
	printf("Starting System...\n");	
	System::Manager.Init();
	while(true){
		System::Manager.Service();
		sleep(1);
	}
}
