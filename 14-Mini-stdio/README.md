14  Mini Standard I/O Package
=============================

##Input/output standard routines

This implements a minimal set of routines provided in the standard input/output library:

-   *int printf(const char *fmt, ...)*: equivalent to standard printf but with no support for float, field sizes, zero filling, etc.
-   *int puts(const char *s)*: same as standard puts
-   *int fputs(const char *s, void *ignored)*: same as standard fputs, but redirects all output to stdout
-   *char *fgets(char *s, int n, void *ignored)*: same as standard fgets but gets all data from stdin.

These routines make use of *putchar* and *getchar* routines, which must be provided by the application.

##Conversion routines

In the files *conv[ch]* the following (non standard) set of conversion and character manipulation was implemented:

-   *int atoi(char s)** : standard routine to convert a string of digits in *s* considering it as a decimal integer and returns its value.
-   *void itoa(int v, char s)** : non standard routine to convert the integer *v* into a decimal representations in *s*.
-   *void utoa(unsigned x, char s)** : non standard routine to convert the unsigned *x* into a string in *s*.
-   *int hextoi(char s): non standard routine to convert a string in *s* considering it as a hexadecimal integer and returns its value.
-   *int itohex(unsigned x, char s)** : non standard routine to convert the integer *x* into a hexadecimal representations in *s*.

##Character classification routines

Besides these routines, the following routines for classification of char was implemented:

-   *int isspace(int c)*: returns 1 if *c* is a space, tab, carriage return or line feed, otherwise returns 0.
-   *int isdigit(int c)*: returns 1 if *c* is a decimal digital, otherwise returns 0.
-   *int isxdigit(int c)*: returns 1 if *c* is a decimal digit or a character in the range *a* to *f* or *A* to *F* , otherwise 0.
-   *int isalpha(int c)*; returns 1 if *c* is a character in the range *a* to *z* or *A* to *Z*.
-   *int isupper(int c)*: returns 1 if c is in the range *A* to *Z*, otherwise returns 0.
-   *int islower(int c)*: returns 1 if c is in the range *a* to *z*, otherwise returns 0.
-   *int iscntrl(int c)*: returns 1 if *c* is a control character (range 0 to 31, includes tab, carriage return, line feed, etc), otherwise returns 0.
-   *int isalnum(int c)*: returns 1 if *c* is in the range *a* to *z*, or *A* to *Z* or a digit, otherwise returns 0.
