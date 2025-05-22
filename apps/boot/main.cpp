/* * @file
 * @brief This is the entry point component
 */

/* *
 * @brief Entry Point function
 *
 * Gets Instance of System::Manager class (which is Irrigation Controller)
 * Initializes it and starts Service loop
 */
#include "sys_manager.h"

static Manager::Manager *managers = {
	Manager::SysManager::Instance(),
	Manager::NetworkManager::Instance(),
	Manager::UpdateManager::Instance(),
};

void app_main(void) {
	printf("Starting System...\n");	
	for(uint8_t i = STARTUP_DELAY; i > 0; --i) { 
		printf("%d ...\n", i);
		sys_delay(1);
	}

	for(const Manager *manager : managers){
		manager->init();
	}

	while(true){
		for(const Manager *manager : managers){
			manager->service();
		}
		sys_delay(1);
	}

	for(const Manager *manager : managers){
		manager->cleanup();
	}

}


#ifdef __cplusplus
}
#endif
