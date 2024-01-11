#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "rom/uart.h" 


#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else 
    static const BaseType_t app_cpu = 1; 
#endif

const char *msg = "Testing 1,2,3 ... Testing \n"; 

void TaskOne(void *arg){
    while (1){
        printf("%sFrom task\n", msg); 
        char *taskName = pcTaskGetName(NULL); 
        ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL));       // investigate how this changes when we go from 1800 to 1700 bytes of task stack
        vTaskDelay(10000/portTICK_PERIOD_MS); 
    }
    
    
}

void TaskTwo(void *arg){
    printf("%s", msg);
}

void setup(){
    vTaskDelay(1000/portTICK_PERIOD_MS); 
    char *taskName = pcTaskGetName(NULL); 
    printf("\n"); 
    printf("%sFrom setup\n", msg);
    xTaskCreatePinnedToCore(TaskOne, "Task One", 1800, NULL, 1, NULL, app_cpu); 
}
void app_main(void){
    setup(); 
}