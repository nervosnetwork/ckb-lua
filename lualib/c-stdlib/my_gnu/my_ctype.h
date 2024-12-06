#ifndef LUA_C_STDLIB_CTYPE_H_
#define LUA_C_STDLIB_CTYPE_H_
#ifdef __cplusplus
extern "C" {
#endif

int islower(int);
int isupper(int);

int tolower(int);
int toupper(int);

int isalnum(int ch);
int isdigit(int ch);
int isxdigit(int ch);
int isspace(int ch);
int isalpha(int ch);
int iscntrl(int ch);
int isgraph(int ch);
int ispunct(int ch);

#ifdef __cplusplus
}
#endif
#endif
