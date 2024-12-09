#ifndef LUA_C_STDLIB_STDDEF_H_
#define LUA_C_STDLIB_STDDEF_H_

typedef signed long ptrdiff_t;

#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#endif  /* LUA_C_STDLIB_STDDEF_H_ */
