#ifndef KRTOS_H
#define KRTOS_H

/* thread control block (TCB) */
typedef struct {
    void *sp; /* stack sointer */
} OSThread;

typedef void (*OSThreadHandler)();

void OSThread_start(
    OSThread *me,
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize);

#endif /*KRTOS_H*/