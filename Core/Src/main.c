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
#include "ucos_ii.h"
#include "qassert.h"
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
Q_DEFINE_THIS_MODULE("main")
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
OS_STK stack_blinky[100]; /* task stack */

enum { INITIAL_BLINK_TIME = (OS_TICKS_PER_SEC / 4) };

/* data shared between tasks */
INT32U volatile shared_blink_time = INITIAL_BLINK_TIME;
OS_EVENT *shared_blink_time_mutex;

void main_blinky(void *pdata) { /* task function */
    (void)pdata; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        INT8U err;
        INT32U bt; /* local copy of shared_blink_time */

        OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
        Q_ASSERT(err == 0);
        bt = shared_blink_time;
        OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

        BSP_ledGreenOn();
        OSTimeDly(bt);       /* BLOCKING! */
        BSP_ledGreenOff();
        OSTimeDly(bt * 3U);  /* BLOCKING! */
    }
}

OS_STK stack_button[100]; /* task stack */

void main_button(void *pdata) { /* task function */
    (void)pdata; /* unused parameter(s) */

    while (1) { /* endless "superloop" */
        INT8U err; /* uC/OS-II error status */

        /* wait on the button-press semaphore (BLOCK indefinitely) */
        OSSemPend(BSP_semaPress, 0, &err); /* BLOCKING! */
        Q_ASSERT(err == 0);
        BSP_ledBlueOn();

        /* update the blink time for the 'blink' thread */
        OSMutexPend(shared_blink_time_mutex, 0, &err); /* mutual exclusion */
        Q_ASSERT(err == 0);
        shared_blink_time >>= 1; /* shorten the blink time by factor of 2 */
        if (shared_blink_time == 0U) {
            shared_blink_time = INITIAL_BLINK_TIME;
        }
        OSMutexPost(shared_blink_time_mutex); /* mutual exclusion */

        /* wait on the button-release semaphore (BLOCK indefinitely) */
        OSSemPend(BSP_semaRelease, 0, &err); /* BLOCKING! */
        BSP_ledBlueOff();
    }
}

OS_EVENT *BSP_semaPress;   /* global semaphore handle */
OS_EVENT *BSP_semaRelease; /* global semaphore handle */

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

    /* initialize the RTOS objects before using them */
    BSP_semaPress   = OSSemCreate(0);
    Q_ASSERT(BSP_semaPress);
    BSP_semaRelease = OSSemCreate(0);
    Q_ASSERT(BSP_semaRelease);
    shared_blink_time_mutex = OSMutexCreate(OS_LOWEST_PRIO - 5, &err);
    Q_ASSERT(err == 0);

    /* create uC/OS-II task, see NOTE1 */
    err = OSTaskCreateExt(&main_blinky, /* the task function */
          (void *)0,      /* the 'pdata' parameter (not used) */
          &stack_blinky[(sizeof(stack_blinky) / sizeof(OS_STK)) - 1], /* ptos */
          OS_LOWEST_PRIO - 4, /* uC/OS-II task priority */
          OS_LOWEST_PRIO - 4, /* unique priority is used as the task ID */
          stack_blinky,   /* pbos */
          (INT32U)(sizeof(stack_blinky)/sizeof(OS_STK)),/* stack depth */
          (void *)0,      /* pext */
          (INT16U)0);     /* task options */
    Q_ASSERT(err == 0);

    /* create uC/OS-II task, see NOTE1 */
    err = OSTaskCreateExt(&main_button, /* the task function */
          (void *)0,      /* the 'pdata' parameter (not used) */
          &stack_button[(sizeof(stack_button) / sizeof(OS_STK)) - 1], /* ptos */
          OS_LOWEST_PRIO - 3, /* uC/OS-II task priority */
          OS_LOWEST_PRIO - 3, /* unique priority is used as the task ID */
          stack_button,   /* pbos */
          (INT32U)(sizeof(stack_button)/sizeof(OS_STK)),/* stack depth */
          (void *)0,      /* pext */
          (INT16U)0);     /* task options */
    Q_ASSERT(err == 0);

    BSP_start(); /* configure and start the interrupts */

    OSStart(); /* start the uC/OS-II scheduler... */
    return 0; /* NOTE: the scheduler does NOT return */
}