#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"


#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else 
    static const BaseType_t app_cpu = 1; 
#endif

#define led_pin 13 

const char *msg = " -- Task Messaging Demo --"; //remove at some point; random check
char * TxPtr = NULL; 
int TxFlag = 0; 


//stores input onto heap; prints heap to confirm 
void TaskOne(void *arg){
    //bool TxRdy = false; 
    //char charArr[100] ; 
    //char *sCharArr = &charArr; 
    char *taskName = pcTaskGetName(NULL);
    uint8_t inChar;
    int count = 0; 
    while (1){
        inChar = fgetc(stdin); 
        if (0 == TxFlag){
            ESP_LOGI(taskName, "Enter Msg... "); 
            TxPtr = (char *)pvPortMalloc(100*sizeof(char));                                                     //Default heap has space for 100 Chars
            TxFlag = 1; 
        }
        if ('\n' == inChar){
            TxPtr[count] = inChar; 
            count = 0;    
            TxFlag = 2; 
            ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL));
            ESP_LOGW(taskName, "Remaining mem on heap (bytes): %d\n", xPortGetFreeHeapSize());
            //charArr[count] = inChar; 
        } else if (0xFF == inChar){
            ; 
        } else {
            TxPtr[count] = inChar; 
            count++; 
            //charArr[count] = inChar; 
        }
        //will keep in code below commented out for potential future reference 
        //if (true == TxRdy){
        //    for (size_t i = 0; i < count; i++){
        //        printf("%c", charArr[i]);
        //        charArr[i] = ' '; 
        //        ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL));       // investigate how this changes when we go from 1800 to 1700 bytes of task stack
        //        //v2; above does same
        //        //printf("%c", *sCharArr); 
        //        //sCharArr++; 
        //        //charArr[i] = ' '; 
        //    }
        //    TxRdy = false; 
        //    count = 0; 
        //    //v2; above does same  
        //    //sCharArr = &charArr;
        //}
        //
        vTaskDelay(50/portTICK_PERIOD_MS); 
    }
    
    
}

//Task that prints input to serial and frees heap; prints heap afterwards to confirm 
void TaskTwo(void *arg){
    char *taskName = pcTaskGetName(NULL); 
    while (1){      
        if (2 == TxFlag){
            int i = 0; 
            char checkChar = ' '; 
            ESP_LOGI(taskName, "Msg Received!");  
            if (NULL != TxPtr){
                while ('\n' != checkChar){
                    checkChar = TxPtr[i]; 
                    printf("%c", TxPtr[i]);
                    i++; 
                }  
                vPortFree(TxPtr); 
                ESP_LOGW(taskName, "Remaining mem on heap (bytes): %d", xPortGetFreeHeapSize());
            }
            TxFlag = 0;
        }
        vTaskDelay(100/portTICK_PERIOD_MS); 
    }
    
}

//Blinks onboard LED when Msg received
void TaskThree(void *arg){
    char lvl = 0; 
    while (1){
        if (2 == TxFlag){
            lvl = 0; 
            for (size_t i = 0; i < 11; i++){
                vTaskDelay(75/portTICK_PERIOD_MS);                                        //Period = .1s; Duty cycle = 50%
                gpio_set_level(led_pin, lvl); 
                lvl = !lvl; 
            }  
        }
        vTaskDelay(50/portTICK_PERIOD_MS); 
    }
    
    
}

void setup(){
    vTaskDelay(1000/portTICK_PERIOD_MS); 
    char *taskName = pcTaskGetName(NULL); 
    printf("\n"); 
    printf("%s\n \n", msg);
    gpio_reset_pin(led_pin); 
    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);     
    xTaskCreatePinnedToCore(TaskOne, "Task One", 2000, NULL, 1, NULL, app_cpu); 
    xTaskCreatePinnedToCore(TaskTwo, "Task Two", 1800, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(TaskThree, "TxRcvdBlink", 1024, NULL, 2, NULL, app_cpu); 
}
void app_main(void){
    setup(); 
}