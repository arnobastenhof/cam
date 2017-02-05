#ifndef EXCEPT_H_

#include <setjmp.h>
#include <stdlib.h>

#define TRY {                     \
  jmp_buf handler;                \
                                  \
  g_handler = &handler;           \
  if (!setjmp(handler)) {

#define CATCH } else {

#define END }                     \
    g_handler = NULL;             \
  }

#define THROW do {                \
  if (*g_handler) {               \
      longjmp(*g_handler, 1);     \
  } else {                        \
    exit(1);                      \
  }                               \
} while (0)
extern jmp_buf *g_handler;

#endif /* EXCEPT_H_ */

