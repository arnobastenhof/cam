#ifndef LEXER_H_
#define LEXER_H_

enum {
  MAXTOK = 10
};

typedef enum {
  /* multi-character tokens */
  LEX_LAMBDA = 1,   /* "lambda" */
  LEX_VAR    = 2,   /* e.g., "foo", "Bar" */
  LEX_NUM    = 3,   /* integers */

  /* single-character tokens */
  LEX_LBRACK = 40,  /* '(' */
  LEX_RBRACK = 41,  /* ')' */
  LEX_PLUS   = 43,  /* '+' */

  /* special value for errors and end-of-string */
  LEX_NONE   = 0,
} tokenType_t;

typedef struct lexer_s {
  char          token[MAXTOK + 1];
  tokenType_t   type;
  const char *  ptr;
} lexer_t;

extern void   Lexer_Init(lexer_t * const, const char * const);
extern int    Lexer_NextToken(lexer_t * const);

#endif /* LEXER_H_ */

