/**
 * @file    fifo.c
 *
 * @note    This is a FIFO (First In-First Out) buffer for chars
 *
 * @note    Uses a global data defined by FIFO_DECLARE_AREA macro
 *
 * @note    It does not use dynamic allocation (malloc or free)
 *
 * @note    Size must be defined in FIFO_DECLARE_AREA and in fifo_init
 *          This is ugly
 *
 * @note    Uses as many dependencies as possible
 */

#include "fifo.h"

//#define USE_ACCESSCONTROL


#ifdef USE_ACESSCONTROL
#define ENTER_CRITICAL_SECTION()    _disable_irq()
#define EXIT_CRITICAL_SECTION()     _enable_irq()
#else
#define ENTER_CRITICAL_SECTION()
#define EXIT_CRITICAL_SECTION()
#endif

/**
 * @brief   initializes a fifo area
 */

Fifo_TypeDef
fifo_init(void *b, int n) {
Fifo_TypeDef f = (Fifo_TypeDef) b;

    ENTER_CRITICAL_SECTION();
    f->front = f->rear = f->data;
    f->size = 0;
    f->capacity = n;
    EXIT_CRITICAL_SECTION();
    return f;
}

/**
 * @brief   Clears fifo
 *
 * @note    Does not free any area, because it is static
            In future, it will free area
 */

void
fifo_deinit(Fifo_TypeDef f) {

    ENTER_CRITICAL_SECTION();
    f->size = 0;
    f->front = f->rear = f->data;
    EXIT_CRITICAL_SECTION();

}

/**
 * @brief   Clears fifo
 *
 * @note    Does not free area. For now identical to deinit
 */
void
fifo_clear(Fifo_TypeDef f) {

    ENTER_CRITICAL_SECTION();
    f->size = 0;
    f->front = f->rear = f->data;
    EXIT_CRITICAL_SECTION();

}

/**
 * @brief   Insert an element in fifo
 *
 * @note    return -1 when full
 */

int
fifo_insert(Fifo_TypeDef f, char ch) {

    if( fifo_full(f) )
        return -1;
    ENTER_CRITICAL_SECTION();
    *(f->rear++) = ch;
    f->size++;
    if( (f->rear - f->data) > f->capacity )
        f->rear = f->data;
    EXIT_CRITICAL_SECTION();
    return 0;
}

/**
 * @brief   Removes an element from fifo
 *
 * @note    return -1 when empty
 */

int
fifo_remove(Fifo_TypeDef f) {
char ch;

    if( fifo_empty(f) )
        return -1;
    ENTER_CRITICAL_SECTION();
    ch = *(f->front++);
    f->size--;
    if( (f->front - f->data) > f->capacity )
        f->front = f->data;
    EXIT_CRITICAL_SECTION();

    return ch;
}
