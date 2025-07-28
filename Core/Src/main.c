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
#include "uc_ao.h"
#include "qassert.h"
#include "bsp.h"
#include <stdbool.h>
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
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC / 4) };

typedef struct {
    Active super; /* inherit Active base class */
    /* add private data for the AO... */
    TimeEvent te;
    bool isLedOn;
    uint32_t blink_time;
} BlinkyButton;

static void BlinkyButton_dispatch(BlinkyButton * const me, Event const * const e) {
    switch (e->sig) {
        case INIT_SIG:
            BSP_ledBlueOff();
            /* intentionally fall through...*/
        case TIMEOUT_SIG: {
            if (!me->isLedOn) { /* LED not on */
                BSP_ledGreenOn();
                me->isLedOn = true;
                TimeEvent_arm(&me->te, me->blink_time, 0U);
            }
            else {  /* LED is on */
                BSP_ledGreenOff();
                me->isLedOn = false;
                TimeEvent_arm(&me->te, me->blink_time * 3U, 0U);
            }
            break;
        }
        case BUTTON_PRESSED_SIG: {
            INT8U err; /* uC/OS-II error status */

            BSP_ledBlueOn();

            me->blink_time >>= 1; /* shorten the blink time by factor of 2 */
            if (me->blink_time == 0U) {
                me->blink_time = INITIAL_BLINK_TIME;
            }
            break;
        }
        case BUTTON_RELEASED_SIG: {
            BSP_ledBlueOff();
            break;
        }
        default: {
            break;
        }
    }
}

}

OS_STK stack_blinkyButton[100]; /* task stack */
static Event *blinkyButton_queue[10];
static BlinkyButton blinkyButton;
Active *AO_BlinkyButton = &blinkyButton.super;

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
  INT8U err;
  BSP_init();
  OSInit();
  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */

}