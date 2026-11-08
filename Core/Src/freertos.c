/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task5.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    CAN_HandleTypeDef *hcan;
    uint32_t id;
    uint8_t len;
    uint8_t *buf_data;
}can_device_transmit_member;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for CAN1_Sending_Service */
osThreadId_t CAN1_Sending_ServiceHandle;
const osThreadAttr_t CAN1_Sending_Service_attributes = {
  .name = "CAN1_Sending_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CAN2_Sending_Service */
osThreadId_t CAN2_Sending_ServiceHandle;
const osThreadAttr_t CAN2_Sending_Service_attributes = {
  .name = "CAN2_Sending_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for DJI_Motor_Service */
osThreadId_t DJI_Motor_ServiceHandle;
const osThreadAttr_t DJI_Motor_Service_attributes = {
  .name = "DJI_Motor_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for LEDtask */
osThreadId_t LEDtaskHandle;
const osThreadAttr_t LEDtask_attributes = {
  .name = "LEDtask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for IMUtask */
osThreadId_t IMUtaskHandle;
const osThreadAttr_t IMUtask_attributes = {
  .name = "IMUtask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for MOTORtask */
osThreadId_t MOTORtaskHandle;
const osThreadAttr_t MOTORtask_attributes = {
  .name = "MOTORtask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for RCtask */
osThreadId_t RCtaskHandle;
const osThreadAttr_t RCtask_attributes = {
  .name = "RCtask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for Ptask */
osThreadId_t PtaskHandle;
const osThreadAttr_t Ptask_attributes = {
  .name = "Ptask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for can1_send_fifo */
osMessageQueueId_t can1_send_fifoHandle;
const osMessageQueueAttr_t can1_send_fifo_attributes = {
  .name = "can1_send_fifo"
};
/* Definitions for can2_send_fifo */
osMessageQueueId_t can2_send_fifoHandle;
const osMessageQueueAttr_t can2_send_fifo_attributes = {
  .name = "can2_send_fifo"
};
/* Definitions for rc_rx_queue */
osMessageQueueId_t rc_rx_queueHandle;
const osMessageQueueAttr_t rc_rx_queue_attributes = {
  .name = "rc_rx_queue"
};
/* Definitions for p_rx_queue */
osMessageQueueId_t p_rx_queueHandle;
const osMessageQueueAttr_t p_rx_queue_attributes = {
  .name = "p_rx_queue"
};
/* Definitions for CAN1CountingSem */
osSemaphoreId_t CAN1CountingSemHandle;
const osSemaphoreAttr_t CAN1CountingSem_attributes = {
  .name = "CAN1CountingSem"
};
/* Definitions for CAN2CountingSem */
osSemaphoreId_t CAN2CountingSemHandle;
const osSemaphoreAttr_t CAN2CountingSem_attributes = {
  .name = "CAN2CountingSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void can1_sending_service(void *argument);
void can2_sending_service(void *argument);
void DJI_motor_service(void *argument);
void StartLEDtask(void *argument);
void StartIMUtask(void *argument);
void StartMOTORtask(void *argument);
void StartRCtask(void *argument);
void StartPtask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of CAN1CountingSem */
  CAN1CountingSemHandle = osSemaphoreNew(3, 3, &CAN1CountingSem_attributes);

  /* creation of CAN2CountingSem */
  CAN2CountingSemHandle = osSemaphoreNew(3, 3, &CAN2CountingSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of can1_send_fifo */
  can1_send_fifoHandle = osMessageQueueNew (16, sizeof(can_device_transmit_member), &can1_send_fifo_attributes);

  /* creation of can2_send_fifo */
  can2_send_fifoHandle = osMessageQueueNew (16, sizeof(can_device_transmit_member), &can2_send_fifo_attributes);

  /* creation of rc_rx_queue */
  rc_rx_queueHandle = osMessageQueueNew (8, sizeof(RCframe_t), &rc_rx_queue_attributes);

  /* creation of p_rx_queue */
  p_rx_queueHandle = osMessageQueueNew (16, sizeof(Pframe_t), &p_rx_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of CAN1_Sending_Service */
  CAN1_Sending_ServiceHandle = osThreadNew(can1_sending_service, NULL, &CAN1_Sending_Service_attributes);

  /* creation of CAN2_Sending_Service */
  CAN2_Sending_ServiceHandle = osThreadNew(can2_sending_service, NULL, &CAN2_Sending_Service_attributes);

  /* creation of DJI_Motor_Service */
  DJI_Motor_ServiceHandle = osThreadNew(DJI_motor_service, NULL, &DJI_Motor_Service_attributes);

  /* creation of LEDtask */
  LEDtaskHandle = osThreadNew(StartLEDtask, NULL, &LEDtask_attributes);

  /* creation of IMUtask */
  IMUtaskHandle = osThreadNew(StartIMUtask, NULL, &IMUtask_attributes);

  /* creation of MOTORtask */
  MOTORtaskHandle = osThreadNew(StartMOTORtask, NULL, &MOTORtask_attributes);

  /* creation of RCtask */
  RCtaskHandle = osThreadNew(StartRCtask, NULL, &RCtask_attributes);

  /* creation of Ptask */
  PtaskHandle = osThreadNew(StartPtask, NULL, &Ptask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_can1_sending_service */
/**
  * @brief  Function implementing the CAN1_Sending_Service thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_can1_sending_service */
__weak void can1_sending_service(void *argument)
{
  /* USER CODE BEGIN can1_sending_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END can1_sending_service */
}

/* USER CODE BEGIN Header_can2_sending_service */
/**
* @brief Function implementing the CAN2_Sending_Service thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can2_sending_service */
__weak void can2_sending_service(void *argument)
{
  /* USER CODE BEGIN can2_sending_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END can2_sending_service */
}

/* USER CODE BEGIN Header_DJI_motor_service */
/**
* @brief Function implementing the DJI_Motor_Service thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DJI_motor_service */
__weak void DJI_motor_service(void *argument)
{
  /* USER CODE BEGIN DJI_motor_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DJI_motor_service */
}

/* USER CODE BEGIN Header_StartLEDtask */
/**
* @brief Function implementing the LEDtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLEDtask */
__weak void StartLEDtask(void *argument)
{
  /* USER CODE BEGIN StartLEDtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartLEDtask */
}

/* USER CODE BEGIN Header_StartIMUtask */
/**
* @brief Function implementing the IMUtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIMUtask */
__weak void StartIMUtask(void *argument)
{
  /* USER CODE BEGIN StartIMUtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIMUtask */
}

/* USER CODE BEGIN Header_StartMOTORtask */
/**
* @brief Function implementing the MOTORtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMOTORtask */
__weak void StartMOTORtask(void *argument)
{
  /* USER CODE BEGIN StartMOTORtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMOTORtask */
}

/* USER CODE BEGIN Header_StartRCtask */
/**
* @brief Function implementing the RCtask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRCtask */
__weak void StartRCtask(void *argument)
{
  /* USER CODE BEGIN StartRCtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartRCtask */
}

/* USER CODE BEGIN Header_StartPtask */
/**
* @brief Function implementing the Ptask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPtask */
__weak void StartPtask(void *argument)
{
  /* USER CODE BEGIN StartPtask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartPtask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

