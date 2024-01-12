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

const char *msg = "Testing 1,2,3 ... Testing";

void TaskOne(void *arg){
    char *taskName = pcTaskGetName(NULL); 
    char charArr[100] ; 
    char *sCharArr = &charArr; 
    uint8_t inChar;
    int count = 0; 
    bool testPrint = false; 
    while (1){
        inChar = fgetc(stdin); 
        if ('\n' == inChar){
            charArr[count] = inChar; 
            count++; 
            //char *TxArr = (char*)pvPortMalloc(count*sizeof(char));
            testPrint = true;  
        } else if (0xFF == inChar){
            ; 
        } else {
            charArr[count] = inChar; 
            count++; 
        }
        
        if (true == testPrint){
            for (size_t i = 0; i < count; i++){
                //printf("%c", charArr[i]);
                //charArr[i] = ' '; 
                //v2; above does same
                printf("%c", *sCharArr); 
                sCharArr++; 
                charArr[i] = ' '; 
            }
            testPrint = false; 
            count = 0; 
            //v2; above does same  
            sCharArr = &charArr;
        }
        ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL));       // investigate how this changes when we go from 1800 to 1700 bytes of task stack
        vTaskDelay(100/portTICK_PERIOD_MS); 
    }
    
    
}

void TaskTwo(void *arg){
    vTaskDelay(1000/portTICK_PERIOD_MS);
    uint8_t testChar;  
    char *taskName = pcTaskGetName(NULL); 
    while (1){      
        //ETS_STATUS s = uart_rx_one_char(&testChar); 
        //if (NULL != testChar){
        //    printf("%c, %d : T2\n", testChar, s); 
        //}
        //testChar = fgetc(stdin);
        //if (0xFF != testChar){
        //    printf("%c : T2\n", testChar); 
        //} 
        //ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(10000/portTICK_PERIOD_MS); 
    }
    
}

void setup(){
    vTaskDelay(1000/portTICK_PERIOD_MS); 
    char *taskName = pcTaskGetName(NULL); 
    printf("\n"); 
    printf("%s: From setup\n", msg);
    xTaskCreatePinnedToCore(TaskOne, "Task One", 3000, NULL, 1, NULL, app_cpu); 
    xTaskCreatePinnedToCore(TaskTwo, "Task Two", 1800, NULL, 1, NULL, app_cpu);
}
void app_main(void){
    setup(); 
}