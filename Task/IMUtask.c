#include "task5.h"
#include "spi.h"
#include "BMI088driver.h"
#include "ins_task.h"
#include "controller.h"
#include "bsp_dwt.h"

extern uint32_t INS_DWT_Count;
extern PID_t TempCtrl;

void StartIMUtask(void*argument)
{
    (void)argument;
    DWT_Init(168);
    while (BMI088_init(&hspi1,1)!=BMI088_NO_ERROR)
    {
        osDelay(100);
    }
    INS_Init();
    INS_DWT_Count=DWT->CYCCNT;//CYCCNT是cpu时钟周期计数寄存器
    TempCtrl.DWT_CNT=INS_DWT_Count;

    for (;;)
    {
        osDelay(INS_TASK_PERIOD);
        INS_Task();
    }

}
