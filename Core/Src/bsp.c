// Board Support Package (BSP) for the EK-TM4C123GXL board
#include "uc_ao.h"
#include "qassert.h"
#include "bsp.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>

// on-board led
#define LED_GREEN 14
#define LED_BLUE 9
#define B2_PIN 13

void App_TimeTickHook(void) {
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current;
    uint32_t tmp;

    TimeEvent_tick(); /* process all uC/AO time events */

    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~GPIOC->IDR; // read Port C with state of Button B1
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
    if ((tmp & (1U << B2_PIN)) != 0U) { // debounced B1 state changed?
        if ((current & (1U << B2_PIN)) != 0U) { // is B1 depressed?
            /* post the "button-pressed" semaphore */
            static Event const buttonPressedEvt = {BUTTON_PRESSED_SIG};
            Active_post(AO_BlinkyButton, &buttonPressedEvt);
        }
        else { /* the button is released */
            /* post the "button-release" semaphore */
            static Event const buttonReleasedEvt = {BUTTON_RELEASED_SIG};
            Active_post(AO_BlinkyButton, &buttonReleasedEvt);
        }
    }
}
/*..........................................................................*/
void App_TaskIdleHook(void) {
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
}
/*..........................................................................*/
void App_TaskCreateHook (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskDelHook    (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskReturnHook (OS_TCB *ptcb) { (void)ptcb; }
void App_TaskStatHook   (void)         {}
void App_TaskSwHook     (void)         {}
void App_TCBInitHook    (OS_TCB *ptcb) { (void)ptcb; }


void BSP_init(void) {
    SystemCoreClockUpdate();

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

void BSP_start(void){
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    NVIC_SetPriority(SysTick_IRQn, CPU_CFG_KA_IPL_BOUNDARY);
}

void Q_onAssert(char const *module, int id) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)id;     /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}

// error-handling function called by exception handlers in the startup code
Q_NORETURN assert_failed(char const * const module, int const id);
Q_NORETURN assert_failed(char const * const module, int const id) {
    Q_onAssert(module, id);
}