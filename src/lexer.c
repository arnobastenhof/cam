#include "lexer.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void
Lexer_Init(lexer_t * const me, const char * const input)
{
  assert(me);
  assert(input);

  me->token[0] = '\0';
  me->type = LEX_NONE;
  me->ptr = input;
}

int
Lexer_NextToken(lexer_t * const me)
{
  char *  cp;

  assert(me);

  for (; isspace(*me->ptr); ++me->ptr)
    ;
  cp = me->token;
  switch (*me->ptr) {
  case '\0':
    me->type = LEX_NONE;
    break;
  case '+': case '(': case ')':
    me->type = *cp++ = *me->ptr++;
    break;
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9':
    do {
      *cp++ = *me->ptr++;
    } while (cp - me->token < MAXTOK && isdigit(*me->ptr));
    me->type = LEX_NUM;
    break;
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
  case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
  case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
  case 's': case 't': case 'u': case 'v': case 'w': case 'x':
  case 'y': case 'z':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
  case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
  case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
  case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
  case 'Y': case 'Z':
    do {
      *cp++ = *me->ptr++;
    } while (cp - me->token < MAXTOK && isalpha(*me->ptr));
    me->type = (strcmp(me->token, "lambda") == 0)
      ? LEX_LAMBDA
      : LEX_VAR;
    break;
  default:
    fprintf(stderr, "Unexpected character: %c.\n", *me->ptr);
    *cp = '\0';
    me->type = LEX_NONE;
    ++me->ptr;
    return -1;
  }
  *cp = '\0';
  return cp - me->token;
}

