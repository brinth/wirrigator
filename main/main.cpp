#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_console.h"
#include "../sys_conf.h"
#include "sys_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* * @file
 * @brief This is the entry point component
 */

/* *
 * @brief Entry Point function
 *
 * Gets Instance of System::Manager class (which is Irrigation Controller)
 * Initializes it and starts Service loop
 */
void app_main(void) {
	printf("Starting System...\n");	
	for(uint8_t i = STARTUP_DELAY; i > 0; --i) { 
		printf("%d ...\n", i);
		sys_delay(1);
	}
	System::Manager::GetInstance()->Init();

	// Main Service Loop
	while(true){
		System::Manager::GetInstance()->Service();
		sys_delay(1);
	}
}


#ifdef __cplusplus
}
#endif
