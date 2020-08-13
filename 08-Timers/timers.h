#ifndef TIMERS_H
/** **************************************************************************
 * @file    timers.h
 * @brief   timers for systick
 * @version 1.0
 *
 *****************************************************************************/


typedef struct {
    int counter;
    int period;
    void (*function)(void);
} Timers_t;

#ifndef TIMERS_N
#define TIMERS_N  10
#endif

int Timers_add(int p, void (*fcnt)(void));
int Timers_del(void (*fcnt)(void));
void Timers_dispatch(void);

#endif
