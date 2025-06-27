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
    for(uint32_t i = 0U; i < 1500U; ++i) {
          BSP_ledGreenOn();
          BSP_ledGreenOff();
    }
    QXThread_delay(1U);
  }
}

uint32_t stack_blinky2[40];
QXThread blinky2;
void main_blinky2(QXThread * const me){
  while(1){
    for(uint32_t i = 0U; i < 3*1500U; ++i) {
          BSP_ledBlueOn();
          BSP_ledBlueOff();
    }
    QXThread_delay(50U);
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

QF_run();

}
  QF_run();