#include "task5.h"
#include "CAN_bsp.h"
#include <cstring>

float chassis_power=0.0f;
float buffer_energy=0.0f;

//对数据进行神秘小处理(拼接加符号）
static int16_t P_BytesToNumber(const uint8_t *data)
{
    uint16_t value=(uint16_t)data[0]|((uint16_t)data[1]<<8);
    return (int16_t)value;
}

//将原始数据传入队列
extern "C"  void P_SendToQueue(const uint8_t *data)
{
   Pframe_t frame;//Pframe_t是我定义的结构体
    //c++语句：std::memcpy(复制到哪里，从哪里复制，复制多少字节)
    std::memcpy(frame.data,data,sizeof(frame.data));
    osMessageQueuePut(p_rx_queueHandle,&frame,0,0);

}

static void P_ReceiveData(can_device_receive*device,uint8_t *data)
{
    (void)device;
    P_SendToQueue(data);
}
extern "C" void StartPtask(void*argument)
{
    (void)argument;

    //使用笃行神秘代码
    static can_device_receive power_receive(&hcan2,0x31,P_ReceiveData);
    Pframe_t frame;
    for (;;)
    {
      if (osMessageQueueGet(p_rx_queueHandle,&frame,nullptr,osWaitForever)==osOK)
      {
          chassis_power=P_BytesToNumber(&frame.data[0])*0.1f;
          buffer_energy=P_BytesToNumber(&frame.data[2])*0.1f;
      }
    }
}