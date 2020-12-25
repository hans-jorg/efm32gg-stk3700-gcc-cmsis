/**
 * @file    buffer.c
 *
 * @note    FIFO buffer for chars
 * @note    Uses a global data defined by DECLARE_BUFFER_AREA macro
 * @note    It does not use malloc
 * @note    Size must be defined in DECLARE_BUFFER_AREA and in buffer_init (Ugly)
 * @note    Uses as many dependencies as possible
 */

#include "buffer.h"
#include "app_cfg.h"
#include "os.h"


#define USE_ACCESSCONTROL


#ifdef USE_ACCESSCONTROL
#define ENTER_CRITICAL_SECTION()    OS_ENTER_CRITICAL()
#define EXIT_CRITICAL_SECTION()     OS_EXIT_CRITICAL()
#else
#define ENTER_CRITICAL_SECTION()
#define EXIT_CRITICAL_SECTION()
#endif

/**
 * @brief   initializes a fifo area
 */

buffer
buffer_init(void *b, int n) {
buffer f = (buffer) b;
#if OS_CRITICAL_METHOD == 3      /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

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
buffer_deinit(buffer f) {
#if OS_CRITICAL_METHOD == 3      /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

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
 buffer_clear(buffer f) {
#if OS_CRITICAL_METHOD == 3      /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

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
buffer_insert(buffer f, char x) {
#if OS_CRITICAL_METHOD == 3      /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    if( buffer_full(f) )
        return -1;
    ENTER_CRITICAL_SECTION();
    *(f->rear++) = x;
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
buffer_remove(buffer f) {
char ch;
#if OS_CRITICAL_METHOD == 3      /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    if( buffer_empty(f) )
        return -1;
    ENTER_CRITICAL_SECTION();
    ch = *(f->front++);
    f->size--;
    if( (f->front - f->data) > f->capacity )
        f->front = f->data;
    EXIT_CRITICAL_SECTION();

    return ch;
}
