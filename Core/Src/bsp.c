// Board Support Package (BSP) for the EK-TM4C123GXL board
#include "bsp.h"
#include "stm32l4xx_hal.h"

// on-board led
#define LED_GREEN 14
#define LED_BLUE 9
#define B2_PIN 13

        }
    }
}

void BSP_init(void) {
    RCC->AHB2ENR  |= ((1U << 1) | (1U << 2));                                   // Enable clock for gpiob (bit 1) and gpioc (bit 2)

    GPIOB->MODER &= ~(3U << (LED_GREEN * 2));                                   // Clear mode bits for pin 14 (set as input by default)
    GPIOB->MODER |= (1U << (LED_GREEN * 2));                                    // Set pin 14 as general-purpose output mode
    GPIOB->OTYPER &= ~(1U << LED_GREEN);                                        // Configure pin 14 as push-pull output
    GPIOB->OSPEEDR |= (1U << ((LED_GREEN * 2)+1));                              // Set pin 14 to high-speed mode
    GPIOB->PUPDR &= ~(3U << (2 * LED_GREEN));                                   // Clear
    GPIOB->PUPDR |=  (1U << (2 * LED_GREEN));                                   // Disable pull-up and pull-down resistors for pin 14

    GPIOC->MODER &= ~(3U << (LED_BLUE * 2));                                    // Clear mode bits for pin 9 (set as input by default)
    GPIOC->MODER |= (1U << (LED_BLUE * 2));                                     // Set pin 9 as general-purpose output mode
    GPIOC->OTYPER &= ~(1U << LED_BLUE);                                         // Configure pin 9 as push-pull output
    GPIOC->OSPEEDR |= (1U << ((LED_BLUE * 2)+1));                               // Set pin 9 to high-speed mode
    GPIOB->PUPDR &= ~(3U << (2 * LED_BLUE));                                    // Clear
    GPIOB->PUPDR |=  (1U << (2 * LED_BLUE));                                    // Disable pull-up and pull-down resistors for pin 9

    GPIOC->MODER   &= ~(3U << 2*B2_PIN);                                        // configure Button B1 (PC.13) pins as input
    GPIOC->PUPDR   &= ~(3U << 2*B2_PIN);                                        // Clear
    GPIOC->PUPDR   |=  (1U << 2*B2_PIN);                                        // Pull-up

    SYSCFG->EXTICR[3] &= ~(0xF << (1 * 4));                                     // Clear EXTI13 bits
    SYSCFG->EXTICR[3] |=  (2 << (1 * 4));                                       // Set EXTI13 to Port C (2 is GPIOC)

    EXTI->PR1 = (1U << B2_PIN);                                                 // Clear pending bit for EXTI line 13

    EXTI->RTSR1 &= ~(1U << B2_PIN);                                             // disable rising edge trigger
    EXTI->FTSR1 &= ~(1U << B2_PIN);                                             // disable falling edge trigger
    EXTI->FTSR1 |= (1U << B2_PIN);                                              // configure falling edge trigger

    EXTI->IMR1 |= (1U << B2_PIN);                                               // configure Button B1 interrupt as falling edge
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

void BSP_ledGreenToggle(void) {
	GPIOB->ODR ^= (1U << LED_GREEN);
}

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

}

void Q_onAssert(char const *module, int id) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)id;     /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}