// Board Support Package (BSP) for the EK-TM4C123GXL board
#include <stdint.h>
#include "bsp.h"
#include "stm32l4xx_hal.h"

// on-board led
#define LED_GREEN 14
#define LED_BLUE 9

void BSP_init(void) {
    RCC->AHB2ENR  |= ((1U << 1) | (1U << 2));   // Enable clock for gpiob (bit 1) and gpioc (bit 2)

    GPIOB->MODER &= ~(3U << 28);                // Clear mode bits for pin 14 (set as input by default)
    GPIOB->MODER |= (1U << 28);                 // Set pin 14 as general-purpose output mode
    GPIOB->OTYPER |= ~(1U << 14);               // Configure pin 14 as push-pull output
    GPIOB->OSPEEDR |= (1U << 29);               // Set pin 14 to high-speed mode
    GPIOB->PUPDR |= ~((1U << 28) | (1U << 29)); // Disable pull-up and pull-down resistors for pin 14

    GPIOC->MODER &= ~(3U << 18);                // Clear mode bits for pin 9 (set as input by default)
    GPIOC->MODER |= (1U << 18);                 // Set pin 9 as general-purpose output mode
    GPIOC->OTYPER |= ~(1U << 9);                // Configure pin 9 as push-pull output
    GPIOC->OSPEEDR |= (1U << 19);               // Set pin 9 to high-speed mode
    GPIOC->PUPDR |= ~((1U << 18) | (1U << 19)); // Disable pull-up and pull-down resistors for pin 9

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    NVIC_SetPriority(SysTick_IRQn, 0U);

    __enable_irq();
}

void BSP_ledGreenOn(void) {
    GPIOB->BSRR |= (1U << LED_GREEN);
}

void BSP_ledGreenOff(void) {
	GPIOB->BSRR |= (1U << (LED_GREEN + 16));
}

void BSP_ledBlueOff(void) {
    GPIOC->BSRR |= (1U << LED_BLUE);
}

void BSP_ledBlueOn(void) {
	GPIOC->BSRR |= (1U << (LED_BLUE + 16));
}