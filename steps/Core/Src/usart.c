/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

#include "cmsis_os2.h"

/*利用中断触发的方式使用串口�?�信�?
  比起直接使用 阻塞模式�? 下面两个API 
        (+) HAL_UART_Transmit()
        (+) HAL_UART_Receive()
中断模式的可�?  check and delay 来让出CPU 供其他任务使�?
        (+) HAL_UART_Transmit_IT()
        (+) HAL_UART_Receive_IT()
        (+) HAL_UART_IRQHandler()
*/

static char   send_complete = 0;
static char   recv_complete = 0;
static char   uart_error_occurs = 0;

static osMutexId_t uart1_send_mutex_id = NULL;  
static osMutexId_t uart1_recv_mutex_id = NULL;  
 
const osMutexAttr_t Thread_Mutex_attr = {
  "uart1Mutex",                          // human readable mutex name
  osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
};

void MX_USART1_Send(uint8_t *pData, uint16_t Size)
{
    if (uart1_send_mutex_id == NULL) {
        // todo: mutex id init error
    }
    osStatus_t status = osMutexAcquire(uart1_send_mutex_id, 0U);
    if (status != osOK)  {
        // mutex acquire error . handle failure code
    }
    if(uart_error_occurs){
        return;
    }
    send_complete = 0;
    HAL_UART_Transmit_IT(&huart1,pData,Size);
    while(0 == send_complete && 0 == uart_error_occurs){
        osDelay(1);
    }

    status = osMutexRelease(uart1_send_mutex_id);
    if (status != osOK)  {
        // mutex release error, handle failure code
    }
}

void MX_USART1_Recv(uint8_t *pData, uint16_t Size)
{
    if (uart1_recv_mutex_id == NULL) {
        // mutex id init error
    }
    osStatus_t status = osMutexAcquire(uart1_recv_mutex_id, 0U);
    if (status != osOK)  {
        // mutex acquire error . handle failure code
    }
    if(uart_error_occurs){
        return;
    }
    recv_complete = 0;
    HAL_UART_Receive_IT(&huart1,pData,Size);
    while(0 == recv_complete && 0 == uart_error_occurs){
        osDelay(1);
    }

    status = osMutexRelease(uart1_recv_mutex_id);
    if (status != osOK)  {
        // mutex release error, handle failure code
    }
}

void MX_UART1_mutex_init()
{
    uart1_send_mutex_id = osMutexNew(NULL);
    if (uart1_send_mutex_id == NULL) {
        // todo: send mutex create error. 
    }

    uart1_recv_mutex_id = osMutexNew(NULL);
    if (uart1_recv_mutex_id == NULL) {
        // todo: recv mutex create error. 
    }
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/*  中断处理的若干个callback 函数*/

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // 被中断处理调用，

    ++send_complete; 
}


/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 被中断处理函数调用，接收完成
    ++recv_complete ;
}


/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // 被中断处理函数调用串口硬件模  触发中断 告诉CPU它有错误啦！
    // 重新初始化它，或者停止使用它?
    ++uart_error_occurs;
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
