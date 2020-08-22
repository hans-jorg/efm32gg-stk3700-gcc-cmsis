#ifndef SYSCALLS_H
#define SYSCALLS_H
/**
 * @file syscalls.h
 *
 * @note    Following 11. System Calls in Newlib LibC documentation
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>

void _exit(void);
int _close(int file);
extern char *__env[1];
extern char **environ;
int _execve(char *name, char **argv, char **env);
int _fork(void);
int _fstat(int file, struct stat *st);
int _getpid(void);
int _isatty(int file);
int _kill(int pid, int sig);
int _link(char *old, char *new);
int _lseek(int file, int ptr, int dir);
int _open(const char *name, int flags, int mode);
int _read(int file, char *ptr, int len);
caddr_t _sbrk(int incr);
int _stat(char *file, struct stat *st);
int _times(struct tms *buf);
int _unlink(char *name);
int _wait(int *status);
int _write(int file, char *ptr, int len);

#endif
