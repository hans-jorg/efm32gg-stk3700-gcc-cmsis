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


static Timers_t Timers[TIMERS_N] = { 0 };
static int Timers_cnt = 0;

/**
 *  @brief  Adds an entry into the list
 */

int Timers_add(int p, void (*fcnt)(void) ) {
int k;

    if( Timers_cnt >= TIMERS_N )
        return -1;

    k = Timers_cnt++;
    Timers[k].counter  = 0;
    Timers[k].period   = p;
    Timers[k].function = fcnt;
    return k;
}

/**
 *  @brief  Deletes an entry in the list
 */
int Timers_del( void (*fcnt)(void) ) {
int i;
int k;

    // Find entry with the function pointer
    for(i=0;(i<Timers_cnt)&&(fcnt!=Timers[i].function);i++) {}
    // If not found, returns
    if( i == Timers_cnt )
        return -1;
    // If found, move the following entry over it, recursively
    for(k=i+1;k<Timers_cnt;k++) {
        Timers[k-1].counter = Timers[k].counter;
        Timers[k-1].period  = Timers[k].period;
        Timers[k-1].function= Timers[k].function;
    }
    Timers_cnt--;
    return 0;
}

void Timers_dispatch(void) {
int i;

    for(i=0;i<Timers_cnt;i++) {
        if( Timers[i].counter == 0 ) {
            Timers[i].function();
            Timers[i].counter = Timers[i].period;
        }
        Timers[i].counter--;
    }
}

