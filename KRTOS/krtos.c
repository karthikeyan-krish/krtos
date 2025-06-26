#include <stdint.h>
#include "krtos.h"
#include "qassert.h"

Q_DEFINE_THIS_FILE

OSThread * volatile OS_curr;
OSThread * volatile OS_next;

OSThread * OS_thread[32+1]; /* Array to hold thread pointers, size is 32+1 for 0-based indexing */
uint32_t OS_readySet; /* bitmask of threads that are ready to run */
uint32_t OS_delayedSet; /* bitmask of threads that are delayed */

#define LOG2(x) (32U - __builtin_clz(x))

OSThread idleThread;
void main_idleThread() {
    while (1) {
        OS_onIdle();
    }
}

void OS_init(void *stkSto, uint32_t stkSize) {
    /* set the PendSV interrupt priority to the lowest level 0xFF */
    *(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16);

    /* start idleThread thread */
    OSThread_start(&idleThread,
                    0U,
                    &main_idleThread,
                    stkSto, stkSize);
}

void OS_sched(void){
    /* OS_next = ... */
    if (OS_readySet == 0U) { /* idle condition? */
        OS_next = OS_thread[0]; /* the idle thread */
    }
    else {
        OS_next = OS_thread[LOG2(OS_readySet)];
        Q_ASSERT(OS_next != (OSThread *)0);
    }

    if(OS_next != OS_curr){
            /* trigger PendSV exception */
            *(uint32_t volatile *)0xE000ED04 = (1U << 28);
    }
}

void OS_run(void) {
    /* callback to configure and start interrupts */
    OS_onStartup();

    __asm volatile ("cpsid i");
    OS_sched();
    __asm volatile ("cpsie i");

    /* the following code should never execute */
    Q_ERROR();
}

void OS_tick(void) {
    uint32_t workingSet = OS_delayedSet;
    while (workingSet != 0U) {
        OSThread *t = OS_thread[LOG2(workingSet)];
        uint32_t bit;
        Q_ASSERT((t != (OSThread *)0) && (t->timeout != 0U));

        bit = (1U << (t->prio - 1U));
        --t->timeout;
        if (t->timeout == 0U) {
            OS_readySet   |= bit;  /* insert to ready set */
            OS_delayedSet &= ~bit; /* remove from delayed/waiting/blocked set */
        }
        workingSet &= ~bit; /* remove from delayed/waiting/blocked set */
    }
}

void OS_delay(uint32_t ticks) {
    uint32_t bit;
    __asm volatile ("cpsid i");

    /* never call OS_delay from the idleThread */
    Q_REQUIRE(OS_curr != OS_thread[0]);

    OS_curr->timeout = ticks;
    bit = (1U << (OS_curr->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
    OS_sched();
    __asm volatile ("cpsie i");
}

void OSThread_start(
    OSThread *me,
    uint8_t prio,
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize) {
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;

    Q_REQUIRE((prio < Q_DIM(OS_thread))
        && (OS_thread[prio] == (OSThread *)0));

    /* custom context switching stack */
    *(--sp) = (1U << 24);  /* xPSR */
    *(--sp) = (uint32_t)threadHandler; /* PC */
    *(--sp) = 0xFFFFFFFD; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */
    /* additional software push registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */

    /* save the top of the stack in the thread's attribute */
    me->sp = sp;

    /* round up the bottom of the stack to the 8-byte boundary */
    stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U)/8)+1U)*8);

    /* pre fill the unused part of the stack with 0xDEADBEEF */
    for(sp = sp - 1U; sp >= stk_limit; --sp){
            *sp = 0xDEADBEEF;
    }

    /* register the thread with the OS */
    OS_thread[prio] = me;
    /* make the thread ready to run */
    if (prio > 0U) {
        OS_readySet |= (1U << (prio - 1U));
    }
}

__attribute__((naked)) void PendSV_Handler(void) {

     __asm volatile (
         "cpsid   i\n"                      /*Disable interrupts*/
         /*if(os_next != (osThread*)0)*/
        "ldr     r3, =OS_curr\n"           /*Load address of OS_curr*/
        "ldr     r3, [r3, #0]\n"           /*Load OS_curr into r3*/
        "cbz     r3, save_context\n"       /*If OS_curr is null, skip saving context*/

        /*push register r4-r11 on the stack*/
        "push   {r4-r11}\n"
        "ldr	r3, =OS_curr\n"            /*Load address of OS_curr*/
        "ldr	r3, [r3, #0]\n"            /*Load OS_curr into r3*/
        /*OS_curr->sp = sp;*/
        "str	sp, [r3, #0]\n"            /*Save SP to OS_curr->sp*/

        "save_context:\n"
        /*sp = OS_next->sp;*/
        "ldr     r3, =OS_next\n"           /*Load address of OS_next*/
        "ldr     r3, [r3, #0]\n"           /*Load OS_next into r2*/
        "ldr     sp, [r3, #0]\n"           /*Load OS_next->sp into SP*/

        /*OS_curr = OS_next;*/
        "ldr     r3, =OS_next\n"           /*Load address of OS_next*/
        "ldr     r3, [r3, #0]\n"           /*Load OS_next into r2*/
        "ldr     r2, =OS_curr\n"           /*Load address of OS_curr*/
        "str     r3, [r2, #0]\n"           /*Load OS_curr into r2*/

        "pop            {r4-r11}\n"
        "cpsie   i\n"                      /*Enable interrupts*/
        "bx      lr\n"                     /*Return from exception*/
 );
}