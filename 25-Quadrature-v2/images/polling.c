


typedef enum { Q0, Q1, Q2, Q3 } state_t;

static state_t state = Q0;  /* initial state */

static int counter = 0;     /* counter */
#define X00   0
#define X01   1
#define X10   2
#define X11   3

void 
processquadrature(unsigned ab) {

    switch(state) {
    case Q0:
        if( ab == X01 ) {
            state = Q1;
            counter++;
        } else if ( ab == X10 ) {
            state = Q3;
            counter--;
        }
        break;
    case Q1:
        if( ab == X11 ) {
            state = Q2;
            counter++;
        } else if ( ab == X00 ) {
            state = Q0;
            counter--;
        }
        break;
    case Q2:
        if( ab == X10 ) {
            state = Q3;
            counter++;
        } else if ( ab == X01 ) {
            state = Q1;
            counter--;
        }
        break;
    case Q3:
        if( ab == X10 ) {
            state = Q0;
            counter++;
        } else if ( ab == X11 ) {
            state = Q2;
            counter--;
        }
        break;
    }
}


