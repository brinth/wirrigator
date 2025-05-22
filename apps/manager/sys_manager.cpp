#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_console.h"
#include "../sys_conf.h"

namespace Manager {

bool SysManager::init(void) {
    // Init Serial Port and console menu 

    // Create task for each module services
}

bool SysManager::service(void* args) {
    // Handle any idle priority operations here
}

bool SysManager::cleanup(void) {
    // Cleanup tasks memory or resourses before maybe putting MCU to sleep
}
    
}; // namespace Manager