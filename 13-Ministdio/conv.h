#ifndef CONV_H
#define CONV_H
/**
 * @file     conv.h
 * @brief    Header for conv.c
 *
 * @version  V1.00
 * @date     25/3/2016
 *
 * @note
 *
 **/

// This should be in ctype.h
int isspace(int c);
int isdigit(int c);
int isxdigit(int c);
int isalpha(int c);
int isupper(int c);
int islower(int c);
int iscntrl(int c);
int isalnum(int c);

int atoi(char *s);
void itoa(int v, char *s);
void utoa(unsigned x, char *s);
int hextoi(char *s);
int itohex(unsigned x, char *s);

#endif // CONV_H
