#include "task5.h"
#include "DJI_motor_driver.h"
#include <cmath>

#define MOTOR_ID 1//根据实际电调ID修改这个宏
#define MOTOR_FREQUENCY 0.2f//正弦频率
#define MOTOR_SPEED 2.0f//最大转速
#define MOTOR_DELAY 2//计算延时，单位tick

extern "C" void StartMOTORtask(void*argument)
{
    (void)argument;
    //依旧笃行神秘小代码
    static DJI_motor motor(&hcan1,nullptr,MOTOR_ID,DJI_M3508,0);
    const float speed=MOTOR_SPEED;
    //w=2*pi*f
    const float w=2.0f*3.1415926535f*MOTOR_FREQUENCY;
    const float start_position=motor.motor_get_rounds();
    const uint32_t start_tick=osKernelGetTickCount();

    //利用节拍数计算时间：时间=经过节拍数/每秒节拍数
    for (;;)
    {
      float time=(float)(osKernelGetTickCount()-start_tick)/(float)osKernelGetTickFreq();
        //目标速度为正弦曲线，据此计算出目标位置
        float position=start_position+speed/w*(1.0f-std::cos(w*time));

        motor.motor_set_rounds(position);

        osDelay(MOTOR_DELAY);
    }
}
