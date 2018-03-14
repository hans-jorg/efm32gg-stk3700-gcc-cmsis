/**
 * @file     conv.c
 * @brief    itoa, utoa and itoh conversion routines
 * @version  V1.0
 * @date     23/01/2016
 *
 **/


#include <stdint.h>
#include "conv.h"


/***********************************************************************//**
 *                                                                         *
 *              Character type routines                                    *
 *                                                                         *
 ***************************************************************************/

/**
 * This flag can be set to use a (faster) table based approach, but it
 * demands more flash memory
 */

//#define USE_TABLE


#ifdef USE_TABLE
/**
 * When using a table, all of this can be in a header like ctype.h
 */
/**
 * Encoding of char classes
 */
///@{
#define ALPHA '\x01' // Alpha
#define DIGIT '\x02' // Decimal
#define HEXA  '\x04' // Hexa
#define CTRL  '\x08' // Control
#define SPACE '\x10' // Space
#define UPPER '\x20' // Upper case
#define LOWER '\x40' // Lower case
///@}

/// Char class table
static const char typetable[128] =  {
/* '\x00' = . */ CTRL,
/* '\x01' = . */ CTRL,
/* '\x02' = . */ CTRL,
/* '\x03' = . */ CTRL,
/* '\x04' = . */ CTRL,
/* '\x05' = . */ CTRL,
/* '\x06' = . */ CTRL,
/* '\x07' = . */ CTRL,
/* '\x08' = . */ CTRL,
/* '\x09' = . */ CTRL|SPACE,
/* '\x0a' = . */ CTRL|SPACE,
/* '\x0b' = . */ CTRL|SPACE,
/* '\x0c' = . */ CTRL|SPACE,
/* '\x0d' = . */ CTRL|SPACE,
/* '\x0e' = . */ CTRL,
/* '\x0f' = . */ CTRL,
/* '\x10' = . */ CTRL,
/* '\x11' = . */ CTRL,
/* '\x12' = . */ CTRL,
/* '\x13' = . */ CTRL,
/* '\x14' = . */ CTRL,
/* '\x15' = . */ CTRL,
/* '\x16' = . */ CTRL,
/* '\x17' = . */ CTRL,
/* '\x18' = . */ CTRL,
/* '\x19' = . */ CTRL,
/* '\x1a' = . */ CTRL,
/* '\x1b' = . */ CTRL,
/* '\x1c' = . */ CTRL,
/* '\x1d' = . */ CTRL,
/* '\x1e' = . */ CTRL,
/* '\x1f' = . */ CTRL,
/* '\x20' =   */ SPACE,
/* '\x21' = ! */ 0,
/* '\x22' = " */ 0,
/* '\x23' = # */ 0,
/* '\x24' = $ */ 0,
/* '\x25' = % */ 0,
/* '\x26' = & */ 0,
/* '\x27' = ' */ 0,
/* '\x28' = ( */ 0,
/* '\x29' = ) */ 0,
/* '\x2a' = * */ 0,
/* '\x2b' = + */ 0,
/* '\x2c' = , */ 0,
/* '\x2d' = - */ 0,
/* '\x2e' = . */ 0,
/* '\x2f' = / */ 0,
/* '\x30' = 0 */ DIGIT|HEXA,
/* '\x31' = 1 */ DIGIT|HEXA,
/* '\x32' = 2 */ DIGIT|HEXA,
/* '\x33' = 3 */ DIGIT|HEXA,
/* '\x34' = 4 */ DIGIT|HEXA,
/* '\x35' = 5 */ DIGIT|HEXA,
/* '\x36' = 6 */ DIGIT|HEXA,
/* '\x37' = 7 */ DIGIT|HEXA,
/* '\x38' = 8 */ DIGIT|HEXA,
/* '\x39' = 9 */ DIGIT|HEXA,
/* '\x3a' = : */ 0,
/* '\x3b' = ; */ 0,
/* '\x3c' = < */ 0,
/* '\x3d' = = */ 0,
/* '\x3e' = > */ 0,
/* '\x3f' = ? */ 0,
/* '\x40' = @ */ 0,
/* '\x41' = A */ A|HEXA|UPPER,
/* '\x42' = B */ A|HEXA|UPPER,
/* '\x43' = C */ A|HEXA|UPPER,
/* '\x44' = D */ A|HEXA|UPPER,
/* '\x45' = E */ A|HEXA|UPPER,
/* '\x46' = F */ A|HEXA|UPPER,
/* '\x47' = G */ A|UPPER,
/* '\x48' = H */ A|UPPER,
/* '\x49' = I */ A|UPPER,
/* '\x4a' = J */ A|UPPER,
/* '\x4b' = K */ A|UPPER,
/* '\x4c' = L */ A|UPPER,
/* '\x4d' = M */ A|UPPER,
/* '\x4e' = N */ A|UPPER,
/* '\x4f' = O */ A|UPPER,
/* '\x50' = P */ A|UPPER,
/* '\x51' = Q */ A|UPPER,
/* '\x52' = R */ A|UPPER,
/* '\x53' = S */ A|UPPER,
/* '\x54' = T */ A|UPPER,
/* '\x55' = U */ A|UPPER,
/* '\x56' = V */ A|UPPER,
/* '\x57' = W */ A|UPPER,
/* '\x58' = X */ A|UPPER,
/* '\x59' = Y */ A|UPPER,
/* '\x5a' = Z */ A|UPPER,
/* '\x5b' = [ */ 0,
/* '\x5c' = \ */ 0,
/* '\x5d' = ] */ 0,
/* '\x5e' = ^ */ 0,
/* '\x5f' = _ */ 0,
/* '\x60' = ` */ 0,
/* '\x61' = a */ A|HEXA|LOWER,
/* '\x62' = b */ A|HEXA|LOWER,
/* '\x63' = c */ A|HEXA|LOWER,
/* '\x64' = d */ A|HEXA|LOWER,
/* '\x65' = e */ A|HEXA|LOWER,
/* '\x66' = f */ A|HEXA|LOWER,
/* '\x67' = g */ A|LOWER,
/* '\x68' = h */ A|LOWER,
/* '\x69' = i */ A|LOWER,
/* '\x6a' = j */ A|LOWER,
/* '\x6b' = k */ A|LOWER,
/* '\x6c' = l */ A|LOWER,
/* '\x6d' = m */ A|LOWER,
/* '\x6e' = n */ A|LOWER,
/* '\x6f' = o */ A|LOWER,
/* '\x70' = p */ A|LOWER,
/* '\x71' = q */ A|LOWER,
/* '\x72' = r */ A|LOWER,
/* '\x73' = s */ A|LOWER,
/* '\x74' = t */ A|LOWER,
/* '\x75' = u */ A|LOWER,
/* '\x76' = v */ A|LOWER,
/* '\x77' = w */ A|LOWER,
/* '\x78' = x */ A|LOWER,
/* '\x79' = y */ A|LOWER,
/* '\x7a' = z */ A|LOWER,
/* '\x7b' = { */ 0,
/* '\x7c' = | */ 0,
/* '\x7d' = } */ 0,
/* '\x7e' = ~ */ 0,
/* '\x7f' = . */ CTRL
};

/**
 * Implementation of table based functions
 */

///@{
int isspace(int c)  { if( c < 128 ) return typetable[c]&SPACE; else return 0; }
int isdigit(int c)  { if( c < 128 ) return typetable[c]&DIGIT; else return 0; }
int isxdigit(int c) { if( c < 128 ) return typetable[c]&HEXA; else return 0; }
int isalpha(int c)  { if( c < 128 ) return typetable[c]&ALPHA; else return 0; }
int isupper(int c)  { if( c < 128 ) return typetable[c]&UPPER; else return 0; }
int islower(int c)  { if( c < 128 ) return typetable[c]&LOWER; else return 0; }
int iscntrl(int c)  { if( c < 128 ) return typetable[c]&CNTRL; else return 0; }
int isalnum(int c)  { if( c < 128 ) return typetable[c]&(DIGIT|ALPHA); else return 0; }
///@}

#else

/**
 * Implementation of comparison based functions
 */

///@{
int isspace(int c) {
    if( (c==' ')||(c=='\t')||(c=='\r')||(c=='\n') ) return 1;
    return 0;
}

int isdigit(int c) {
    if( (c>='0')&&(c<='9') ) return 1;
    return 0;
}

int isxdigit(int c) {
    if( (c>='0')&&(c<='9') ) return 1;
    if( (c>='A')&&(c<='F') ) return 1;
    if( (c>='a')&&(c<='f') ) return 1;
    return 0;
}

int isalpha(int c) {
    if( (c>='A')&&(c<='Z') ) return 1;
    if( (c>='a')&&(c<='z') ) return 1;
    return 0;
}

int isupper(int c)  {
    if( (c>='A')&&(c<='Z') ) return 1;
    return 0;
}
int islower(int c)  {
    if( (c>='a')&&(c<='z') ) return 1;
    return 0;
}
int iscntrl(int c)  {
    if( (c=='\x7F')&&(c<='\x1F') ) return 1;
    return 0;
}

int isalnum(int c)  {
    if( (c>='A')&&(c<='Z') ) return 1;
    if( (c>='a')&&(c<='z') ) return 1;
    if( (c>='0')&&(c<='9') ) return 1;
    return 0;
}
///@}

#endif

/***************************************************************************
 *                                                                         *
 *              Conversion routines                                        *
 *                                                                         *
 ***************************************************************************/

/**
 * @brief atoi
 *
 * @note  Converts a string with decimal ASCII string with signal to an integer
 * @note  Assumes 32 bit integer
 *
 */

int atoi(char *s) {
char *p = s;
int n = 0;
int neg = 0;

    while( *p && isspace(*p) ) p++;
    if( *p == '-' ) {
        neg = 1;
        p++;
    }
    if( *p == 0 ) return n;
    while ( *p && isspace(*p) ) {
        n *= 10; // n = (n<<3)+(n<<1);
        n += (*p-'0');
    }
    if( *p != 0 ) return 0;
    if( neg ) return -n;
    return n;
}



/**
 * @brief itoa
 *
 * @note  Converts an signed integer to an decimal ASCII string with signal
 * @note  Assumes 32 bit integer
 *
 */

void
itoa(int v, char *s) {
int sign=0;
int p10 = 10;
int p10ant = 0;
char *p;
unsigned x;
int n;

    n = 1;
    x = v;
    if( v < 0 ) {
        x = -v;
        sign = 1;
        n++;
    }

    p = s;
    while( x >= p10 ) {
        p10ant = p10;
        p10 *= 10;
        if( p10 < p10ant ) // if overflow
            break;
        n++;
        *p++ = 'X';
    }
    p = s+n;
    *p--='\0';
    do {
        *p-- = ((x%10)+'0');
        x /= 10;
    } while( x > 0 );
    if( sign )
        *p = '-';
    return;
}

/**
 * @brief utoa
 *
 * @note  Converts an unsigned integer to an decimal ASCII string
 * @note  Assumes 32 bit integer
 *
 */

void
utoa(unsigned x, char *s) {
int p10 = 10;
int p10ant = 0;
char *p;
int n;

    n = 1;
    p = s;
    while( x >= p10 ) {
        p10ant = p10;
        p10 *= 10;
        if( p10 < p10ant ) // if overflow
            break;
        n++;
        *p++ = 'X';
    }
    p = s+n;
    *p--='\0';
    do {
        *p-- = ((x%10)+'0');
        x /= 10;
    } while( x > 0 );
    return;
}

/**
 * @brief itohex
 *
 * @note  Converts an integer to an hexadecimal ASCII string
 * @note  Assumes 32 bit integer
 *
 */
int hextoi(char *s) {
int n;

    while( *s && isspace(*s) ) s++;
    if( *s == '\0' )
        return 0;
    n = 0;
    while( *s && isxdigit(*s) ) {
        int c= *s++;
        if(isdigit(c)) n |= (c-'0');
        else if( islower(c) ) n |= (c-'a'+10);
        else if( isupper(c) ) n |= (c-'A'+10);
        n <<= 4;
    }
    return n;
}


/**
 * @brief itohex
 *
 * @note  Converts an integer to an hexadecimal ASCII string
 * @note  Assumes 32 bit integer
 *
 */
//@{
static const char tohex[] = "0123456789ABCDEF";

int itohex(unsigned x, char *s) {
int n = sizeof(unsigned)*8;

    do {
        n -= 4;
        *s++ = tohex[(x>>n)&0xF];
    } while ( n > 0 );
    *s = '\0';

    return 0;
}
//@}
