/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "qpc.h"
#include "bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
QXSemaphore SW1_sema;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t stack_blinky1[40];
QXThread blinky1;
void main_blinky1(QXThread * const me) {
  while(1){
    for(uint32_t i = 0U; i < 1900U + 1U; ++i) {
        BSP_sendMorseCode(0xA8EEE2A0U); /* "SOS" */
        QXThread_delay(1U); /* block for 1 tick */
    }
  }
}

uint32_t stack_blinky2[40];
QXThread blinky2;
void main_blinky2(QXThread * const me){
  while(1){
    QXSemaphore_wait(&SW1_sema,  /* pointer to semaphore to wait on */
                  QXTHREAD_NO_TIMEOUT); /* timeout for waiting */
    for(uint32_t i = 0U; i < 3*1500U; ++i) {
          // BSP_ledBlueOn();
          // BSP_ledBlueOff();
    }
  }
}

uint32_t stack_blinky3[40];
QXThread blinky3;
void main_blinky3(QXThread * const me) {
    while (1) {
        BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
        BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
        QXThread_delay(5U);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* USER CODE BEGIN Init */
  BSP_init();
  QF_init();
  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */

  /* initialize the SW1_sema semaphore as binary, signaling semaphore */
  QXSemaphore_init(&SW1_sema, /* pointer to semaphore to initialize */
                  0U,  /* initial semaphore count (singaling semaphore) */
                  1U); /* maximum semaphore count (binary semaphore) */

  /* fabricate Cortex-M ISR stack frame for blinky1 */
  QXThread_ctor(&blinky1, &main_blinky1, 0U);
  QXTHREAD_START(&blinky1,
                  5U,
                  (void *)0, 0,
                  stack_blinky1, sizeof(stack_blinky1),
                  (void *)0);

  /* fabricate Cortex-M ISR stack frame for blinky2 */
  QXThread_ctor(&blinky2, &main_blinky2, 0U);
  QXTHREAD_START(&blinky2,
                  2U,
                  (void *)0, 0,
                  stack_blinky2, sizeof(stack_blinky2),
                  (void *)0);

    /* initialize and start blinky3 thread */
    QXThread_ctor(&blinky3, &main_blinky3, 0);
    QXTHREAD_START(&blinky3,
                   1U, /* priority */
                   (void *)0, 0, /* message queue (not used) */
                   stack_blinky3, sizeof(stack_blinky3), /* stack */
                   (void *)0); /* extra parameter (not used) */

  QF_run();
}