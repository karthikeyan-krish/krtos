#ifndef __BSP_H__
#define __BSP_H__

// system clock tick [Hz]
#define BSP_TICKS_PER_SEC 1000U

// initialize gpio for LED interface
void BSP_init(void);

// led control functions
void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);

void BSP_ledGreenToggle(void);

extern QXSemaphore SW1_sema;

void BSP_sendMorseCode(uint32_t bitmask);

#endif // __BSP_H__