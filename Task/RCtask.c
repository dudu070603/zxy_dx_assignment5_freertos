#include "task5.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

#define RC_FRAME_SIZE 18u
#define RC_DMA_SIZE 36u

static uint8_t rc_dma_buf[RC_DMA_SIZE];
static uint16_t rc_last_pos=0;
static volatile uint8_t rc_restart=1;

typedef struct {
    int16_t ch[4];
    uint8_t s[2];
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;
    uint8_t mouse_left;
    uint8_t mouse_right;
    uint16_t keyboard;
    uint16_t wheel_raw;
}RCData_t;

RCData_t g_rc;

static uint16_t RC_BytesToNumber(const uint8_t *data)
{
    return(uint16_t)(
        (uint16_t)data[0]|
        ((uint16_t)data[1]<<8));

}
static uint8_t RC_ParseData(const uint8_t *data,RCData_t*rc)
{
    uint16_t raw[4];
    raw[0]=(
        data[0]|((uint16_t)data[1]<<8))&0x07ffu;
    raw[1]=(
        (data[1]>>3)|((uint16_t)data[2]<<5))&0x07ffu;
    raw[2]=(
        (data[2]>>6)|((uint16_t)data[3]<<2)|((uint16_t)data[4]<<10))&0x07ffu;
    raw[3]=(
        (data[4]>>1|((uint16_t)data[5]<<7))&0x07ffu);

    for(uint32_t i=0;i<4;i++)
    {
        rc->ch[i]=(int16_t)raw[i]-1024;
        if (rc->ch[i]<-700||rc->ch[i]>700)
        {
            return 0;
        }
    }
    rc->s[0]=(data[5]>>4)&0x03u;
    rc->s[1]=(data[5]>>6)&0x03u;
    if (rc->s[0]==0||rc->s[1]==0)
    {
        return 0;
    }
    rc->mouse_x=(int16_t)RC_BytesToNumber(&data[6]);
    rc->mouse_y=(int16_t)RC_BytesToNumber(&data[8]);
    rc->mouse_z=(int16_t)RC_BytesToNumber(&data[10]);
    rc->mouse_left=data[12];
    rc->mouse_right=data[13];
    rc->keyboard=RC_BytesToNumber(&data[14]);
    rc->wheel_raw=RC_BytesToNumber(&data[16]);
    return 1;

}

static HAL_StatusTypeDef RC_StartReceive(void)
{
    __HAL_UART_DISABLE_IT(&huart3,UART_IT_IDLE);
    HAL_UART_DMAStop(&huart3);
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);

    rc_last_pos=0;
    osMessageQueueReset(rc_rx_queueHandle);

    taskENTER_CRITICAL();

    HAL_StatusTypeDef status=HAL_UART_Receive_DMA(&huart3,rc_dma_buf,RC_DMA_SIZE);

    if (status==HAL_OK) //判断DMA接受启动是否完成
    {
        __HAL_DMA_DISABLE_IT(huart3.hdmarx,DMA_IT_HT);

        rc_restart=0;
        __HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);

    }
    taskEXIT_CRITICAL();
    return status;

}

//处理遥控器串口空闲数据，把数据发入队列
void Task5_DbusIdleISR(void)
{
   //判断IDLE事件是否发生
    if (!__HAL_UART_GET_IT_SOURCE(&huart3,UART_IT_IDLE)||
        !__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE))
    {
        return;
    }
    //检查错误位
    uint32_t status = huart3.Instance->SR;

    if (status & (USART_SR_PE | USART_SR_FE |
                  USART_SR_NE | USART_SR_ORE))
    {
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_IDLE);
        return;
    }

    __HAL_UART_CLEAR_IDLEFLAG(&huart3);

    //确定DMA写入缓冲区的数据下标
   uint16_t remaining=(uint16_t)__HAL_DMA_GET_COUNTER(huart3.hdmarx);
   uint16_t pos=
       (RC_DMA_SIZE-remaining)%RC_DMA_SIZE;
    uint16_t received =
    (pos + RC_DMA_SIZE - rc_last_pos) % RC_DMA_SIZE;

    rc_last_pos = pos;

    if (received != RC_FRAME_SIZE)
    {
        return;
    }

    RCframe_t frame;

    uint16_t start =
        (pos + RC_DMA_SIZE - RC_FRAME_SIZE) % RC_DMA_SIZE;

    for (uint16_t i=0;i<RC_FRAME_SIZE;i++)
    {
        frame.data[i]=rc_dma_buf[(start+i)%RC_DMA_SIZE];
    }

    osMessageQueuePut(rc_rx_queueHandle,&frame,0,0);
}

//完成错误处理后的恢复操作
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    (void)huart;
    __HAL_UART_DISABLE_IT(&huart3,UART_IT_IDLE);
        rc_restart=1;
}

void StartRCtask(void*argument) {
    (void)argument;
    RCframe_t frame;
    RCData_t decoded;

    //判断是否需要恢复接收
    for (;;)
    {
        if (rc_restart)
        {
            if (RC_StartReceive()!=HAL_OK)
            {
                osDelay(20);
                continue;
            }
        }
        //从遥控器队列取消息，存进frame
        if (osMessageQueueGet(rc_rx_queueHandle,&frame,NULL,20)==osOK)
        {
            //解算，结构体整体赋值
            if (RC_ParseData(frame.data,&decoded))
            {
                g_rc=decoded;
            }
        }
    }
}
