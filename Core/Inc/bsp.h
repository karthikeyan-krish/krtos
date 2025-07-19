#ifndef __BSP_H__
#define __BSP_H__

// system clock tick [Hz]
#define BSP_TICKS_PER_SEC 1000U

// initialize gpio for LED interface
void BSP_init(void);
void BSP_start(void);

// led control functions
void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);

void BSP_ledGreenToggle(void);

extern OS_EVENT *BSP_semaPress;   /* global semaphore handle */
extern OS_EVENT *BSP_semaRelease; /* global semaphore handle */

#endif // __BSP_H__