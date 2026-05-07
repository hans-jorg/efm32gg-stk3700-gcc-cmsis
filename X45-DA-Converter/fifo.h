#ifndef FIFO_H
#define FIFO_H
/**
 *  @file   fifo.h
 */


/**
 *  @brief  Data structure to store info about fifo, including its data
 *
 * @note    Uses x[1] hack. This structure is a header of a memory area with
 *          size equal to sizeof(struct fifo_s)-1+SIZE*sizeof(char)
 *
 * @note    First element is a pointer to force data alignement
 */

struct fifo_s {
    char    *front;             // pointer to first char in fifo
    char    *rear;              // pointer to last char in fifo
    int     size;               // number of char stored in fifo
    int     capacity;           // number of chars in data
    char    data[1];             // flexible array
};

typedef struct fifo_s *Fifo_TypeDef;

#define FIFO_DECLARE_AREA(AREANAME,SIZE) \
        unsigned AREANAME[(sizeof(struct fifo_s)+(SIZE)+sizeof(unsigned)-1)/\
                           sizeof(unsigned)]

Fifo_TypeDef  fifo_init(void *area, int size);
void    fifo_deinit(Fifo_TypeDef f);
int     fifo_insert(Fifo_TypeDef f, char ch);
int     fifo_remove(Fifo_TypeDef f);
void    fifo_clear(Fifo_TypeDef f);

#define fifo_capacity(F) ((F)->capacity)
#define fifo_size(F) ((F)->size)
#define fifo_empty(F) ((F)->size==0)
#define fifo_full(F) ((F)->size==fifo_capacity(F))

#endif
