#ifndef TASK5_H
#define TASK5_H

#include<stdint.h>
#include "cmsis_os.h"

typedef struct {
    uint8_t data[18];
}RCframe_t;

typedef struct {
    uint8_t data[4];
}Pframe_t;


#ifdef __cplusplus
extern "C" {
#endif
extern osMessageQueueId_t rc_rx_queueHandle;
extern osMessageQueueId_t p_rx_queueHandle;

void StartLEDtask(void*argument);
void StartIMUtask(void*argument);
void StartMOTORtask(void*argument);
void StartRCtask(void*argument);
void StartPtask(void*argument);

void Task5_DbusIdleISR(void);
void P_SendToQueue(const uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif
