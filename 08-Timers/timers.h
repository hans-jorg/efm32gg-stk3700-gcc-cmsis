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
} timers_t;

#ifndef TIMERS_H
#define TIMERS_N  10
#endif

int timers_add(int p, void (*fcnt)(void));
int timers_del(void (*fcnt)(void));
void timers_dispatch(void);

#endif
