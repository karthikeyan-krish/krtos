#ifndef KRTOS_H
#define KRTOS_H

/* thread control block (TCB) */
typedef struct {
    void *sp; /* stack sointer */
    uint32_t timeout; /* timeout delay counter */
    uint8_t prio; /* thread priority */
} OSThread;

typedef void (*OSThreadHandler)();

void OS_init(void *stkSto, uint32_t stkSize);

void OS_onIdle(void);

/* this function must be called with interrupts disabled */
void OS_sched(void);

/* non blocking delay that block only the running thread */
void OS_delay(uint32_t ticks);

/* process timeouts */
void OS_tick(void);
/* callback to configure and start interrupts */
void OS_onStartup(void);

void OSThread_start(
    OSThread *me,
    uint8_t prio,
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize);

#endif /*KRTOS_H*/