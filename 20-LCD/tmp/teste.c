


#include <stdio.h>
#include <stdint.h>

#define BIT(N) (1U<<(N))
#define BITLONG(N) (1UL<<(N))
#define BITTRUNCATED(N) ( (N)>=32?(1UL<<((N)-32)):(1UL<<(N)) )
#define BIT32(N) (1UL<<((N)%32))


typedef struct {
    uint32_t    hi;
    uint32_t    lo;
} SegEncoding_t;

const SegEncoding_t seg_encoding[] = {
    //   SEG19-16     SEG15-0
    {   0x00000000,  BIT(12)    },  //    S0       ->      Seg 12    SEGEN3
    {   0x00000000,  BIT(13)    },  //    S1       ->      Seg 13    SEGEN3
    {   0x00000000,  BIT(14)    },  //    S2       ->      Seg 14    SEGEN3
    {   0x00000000,  BIT(15)    },  //    S3       ->      Seg 15    SEGEN3
    {   0x00000000,  BIT(16)    },  //    S4       ->      Seg 16    SEGEN4
    {   0x00000000,  BIT(17)    },  //    S5       ->      Seg 17    SEGEN4
    {   0x00000000,  BIT(18)    },  //    S6       ->      Seg 18    SEGEN4
    {   0x00000000,  BIT(19)    },  //    S7       ->      Seg 19    SEGEN4
    {   0x00000000,  BIT(28)    },  //    S8       ->      Seg 28    SEGEN7
    {   0x00000000,  BIT(29)    },  //    S9       ->      Seg 29    SEGEN7
    {   0x00000000,  BIT(30)    },  //    S10      ->      Seg 30    SEGEN7
    {   0x00000000,  BIT(31)    },  //    S11      ->      Seg 31    SEGEN7
    {   BIT32(32),   0x00000000 },  //    S12      ->      Seg 32    SEGEN8
    {   BIT32(33),   0x00000000 },  //    S13      ->      Seg 33    SEGEN8
    {   BIT32(34),   0x00000000 },  //    S14      ->      Seg 34    SEGEN8
    {   BIT32(35),   0x00000000 },  //    S15      ->      Seg 35    SEGEN8
    {   BIT32(36),   0x00000000 },  //    S16      ->      Seg 36    SEGEN0
    {   BIT32(37),   0x00000000 },  //    S17      ->      Seg 37    SEGEN9
    {   BIT32(38),   0x00000000 },  //    S18      ->      Seg 38    SEGEN9
    {   BIT32(39),   0x00000000 },  //    S19      ->      Seg 39    SEGEN9
};


main() {
uint32_t segen;
int i;
uint32_t dispctrl;
uint32_t m,m4;
SegEncoding_t used = { 0,0 };

    /* Scan all character encoding and accumulates all bits used */
    for(i=0;i<sizeof(seg_encoding)/sizeof(SegEncoding_t);i++) {
        used.hi |= seg_encoding[i].hi;
        used.lo |= seg_encoding[i].lo;
    }

    /*
     * Configure pins
     * A bit in the segen register corresponds to 4 segment pins
     */
    m = 1;
    m4 = 0xF;
    segen = 0;
    while ( (m&BIT(8)) == 0 ) {
        if( (used.lo & m4) != 0 ) {
            segen |= m;
        }
        m <<= 1;
        m4 <<= 4;
    }
    while ( (m&BIT(10)) == 0 ) {
        if( (used.hi & m4) != 0 ) {
            segen |= m;
        }
        m <<= 1;
        m4 <<= 4;
    }

    printf("%08X\n",segen);
    return 0;
}

