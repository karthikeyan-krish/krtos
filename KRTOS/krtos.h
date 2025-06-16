#ifndef KRTOS_H
#define KRTOS_H

/* thread control block (TCB) */
typedef struct {
    void *sp; /* stack sointer */
} OSThread;

typedef void (*OSThreadHandler)();

#endif /*KRTOS_H*/