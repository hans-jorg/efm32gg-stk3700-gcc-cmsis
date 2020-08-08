/**
 *  @file   timers.c
 *
 *  @note   There is no gap in the list. All 'timer_cnt' entries must be valid
 *          in order to speed up processing
 *
 *  @author hans
 *  @date   2020/08/05
 */

#include "timers.h"


static timers_t timers[TIMERS_N] = { 0 };
static int timers_cnt = 0;

/**
 *  @brief  Adds an entry into the list
 */

int timers_add(int p, void (*fcnt)(void) ) {
int k;

    if( timers_cnt >= TIMERS_N )
        return -1;

    k = timers_cnt++;
    timers[k].counter  = 0;
    timers[k].period   = p;
    timers[k].function = fcnt;
    return k;
}

/**
 *  @brief  Deletes an entry in the list
 */
int timers_del( void (*fcnt)(void) ) {
int i;
int k;

    // Find entry with the function pointer
    for(i=0;(i<timers_cnt)&&(fcnt!=timers[i].function);i++) {}
    // If not found, returns
    if( i == timers_cnt )
        return -1;
    // If found, move the following entry over it, recursively
    for(k=i+1;k<timers_cnt;k++) {
        timers[k-1].counter = timers[k].counter;
        timers[k-1].period  = timers[k].period;
        timers[k-1].function= timers[k].function;
    }
    timers_cnt--;
    return 0;
}

void timers_dispatch(void) {
int i;

    for(i=0;i<timers_cnt;i++) {
        if( timers[i].counter == 0 ) {
            timers[i].function();
            timers[i].counter = timers[i].period;
        }
        timers[i].counter--;
    }
}

