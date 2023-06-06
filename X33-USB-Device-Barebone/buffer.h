#ifndef BUFFER_H
#define BUFFER_H
/**
 *  @file   buffer.h
 */


/**
 *  @brief  Data structure to store info about fifo, including its data
 *
 * @note    Uses x[0] hack. This structure is a header
 * @note    First element is a pointer to force data alignement
 */

struct buffer_s {
    char    *front;             // pointer to first char in fifo
    char    *rear;              // pointer to last char in fifo
    int     size;               // number of char stored in fifo
    int     capacity;           // number of chars in data
    char    data[];             // flexible array
};

typedef struct buffer_s *buffer;

#define DECLARE_BUFFER_AREA(AREANAME,SIZE) unsigned AREANAME[(sizeof(struct buffer_s)+(SIZE)+sizeof(unsigned)-1)/sizeof(unsigned)]

buffer  buffer_init(void *area,int size);
void    buffer_deinit(buffer f);
int     buffer_insert(buffer f, char x);
int     buffer_remove(buffer f);
void    buffer_clear(buffer f);

#define buffer_capacity(F) ((F)->capacity)
#define buffer_size(F) ((F)->size)
#define buffer_empty(F) ((F)->size==0)
#define buffer_full(F) ((F)->size==buffer_capacity(F))

#endif
