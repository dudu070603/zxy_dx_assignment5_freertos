#include "task5.h"
#include "gpio.h"

void StartLEDtask(void *argument) {
    (void)argument;

    const uint16_t pins[3]={
        GPIO_PIN_10,//蓝色
        GPIO_PIN_11,//绿色
        GPIO_PIN_12,//红色
    };

    for (;;)
    {
        for (uint32_t i=0; i<3; i++) {
            HAL_GPIO_WritePin(
                GPIOH,
                GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12,
                GPIO_PIN_RESET
                );

            HAL_GPIO_WritePin(GPIOH,pins[i],GPIO_PIN_SET);

            osDelay(300);
        }
    }
}
