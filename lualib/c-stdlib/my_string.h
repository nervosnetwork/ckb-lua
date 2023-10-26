#ifndef LUA_C_STDLIB_STRING_H_
#define LUA_C_STDLIB_STRING_H_

#include <stddef.h>
#include <string.h>

char *strchr (const char *, int);
int strncmp(const char *_l, const char *_r, size_t n);
char *strpbrk (const char *, const char *);
size_t strcspn (const char *, const char *);
size_t strspn (const char *, const char *);

int strcoll (const char *, const char *);

char *strerror (int);

char *strstr(const char *str, const char *substr);

void *memchr(const void *, int, size_t);
void* memcpy(void *dest, const void *src, size_t count);

#endif /* LUA_C_STDLIB_STRING_H_ */
