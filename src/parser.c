#include "parser.h"

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "except.h"
#include "lexer.h"
#include "node.h"
#include "pool.h"

typedef struct {
  node_t  base;
  char    value[MAXTOK + 1];
} symbol_t;

static symbol_t g_pool[N_ELEMS];
pool_t          g_symbol_pool = INIT_POOL(g_pool, N_ELEMS, symbol_t);

static ast_t * ParseExpr(lexer_t * const, const symbol_t *);
static ast_t * ParseVar(const char * const, const symbol_t * const);
static ast_t * ParseNum(const char *);
static ast_t * ParseSum(lexer_t * const, const symbol_t *);
static ast_t * ParseApp(lexer_t * const, const symbol_t *);
static ast_t * ParseAbs(lexer_t * const, const symbol_t *, int *);

static void
PushNewSymbol(const symbol_t ** scope, const char * const token)
{
  symbol_t *  symbol;

  assert(scope);
  assert(token);

  symbol = Pool_Alloc(&g_symbol_pool);
  strcpy(symbol->value, token);
  Push(scope, symbol);
}

static void
Consume(lexer_t * const lexer)
{
  int cnt;

  switch (cnt = Lexer_NextToken(lexer)) {
  case 0:
    fprintf(stderr, "Unexpected end of input.\n");
    /* fall-through */
  case -1:
    THROW;
  default:
    return;
  }
}

static void
Match(lexer_t * const lexer, const tokenType_t type)
{
  if (type != lexer->type) {
    fprintf(stderr, "Unexpected token: %s.\n", lexer->token);
    THROW;
  }
}

static inline void
Expect(lexer_t * const lexer, const tokenType_t type)
{
  Consume(lexer);
  Match(lexer, type);
}

ast_t *
Parse(lexer_t * const lexer)
{
  Consume(lexer);
  return ParseExpr(lexer, NULL);
}

static ast_t *
ParseExpr(lexer_t * const lexer, const symbol_t *scope)
{
  assert(lexer->type != LEX_NONE);

  switch (lexer->type) {
  case LEX_VAR:
    return ParseVar(lexer->token, scope);
  case LEX_NUM:
    return ParseNum(lexer->token);
  case LEX_LBRACK:
    Consume(lexer);
    if (lexer->type == LEX_PLUS) {
      return ParseSum(lexer, scope);
    }
    return ParseApp(lexer, scope);
  default:
    fprintf(stderr, "Unexpected token: %s.\n", lexer->token);
    THROW;
  }
}

static ast_t *
ParseVar(const char * const token, const symbol_t * const scope)
{
  ast_t *     ap;
  symbol_t *  it;

  assert(token);

  if (IsEmpty(scope)) {
    goto error;
  }
  ap = Ast_Node(AST_COMP);
  Ast_AddChild(ap, Ast_Snd());
  it = Link(scope);
  do {
    if (strcmp(token, it->value) == 0) {
      return ap;
    } else {
      Ast_AddChild(ap, Ast_Fst());
    }
  } while ((it = Link(it)) != Link(scope));
error:
  fprintf(stderr, "Unbound variable: %s.\n", token);
  THROW;
}

static ast_t *
ParseNum(const char *cp)
{
  int total = 0;

  assert(isdigit(*cp));

  do {
    assert(isdigit(*cp));
    total = (10 * total) + (*cp - '0');
  } while (*++cp != '\0');
  return Ast_Quote(total);
}

static ast_t *
ParseSum(lexer_t * const lexer, const symbol_t *scope)
{
  ast_t * root;

  assert(lexer);
  assert(lexer->type == LEX_PLUS);

  Consume(lexer);
  root = ParseExpr(lexer, scope);
  Consume(lexer);
  do {
    root = Ast_Pair(root, ParseExpr(lexer, scope));
    root = Ast_Pair(Ast_Plus(), root);
    root = Ast_Comp(2, root, Ast_App());
    Consume(lexer);
  } while (lexer->type != LEX_RBRACK);

  return root;
}

static ast_t *
ParseApp(lexer_t * const lexer, const symbol_t *scope)
{
  ast_t * root;
  int     cnt;

  assert(lexer);

  root = ParseAbs(lexer, scope, &cnt);
  while (cnt-- > 0) {
    Consume(lexer);
    root = Ast_Pair(root, ParseExpr(lexer, scope));
    root = Ast_Comp(2, root, Ast_App());
  }
  Expect(lexer, LEX_RBRACK);
  return root;
}

static ast_t *
ParseAbs(lexer_t * const lexer, const symbol_t *scope, int *cnt)
{
  ast_t * ap;
  int     i;

  assert(lexer);

  Match(lexer, LEX_LBRACK);
  Expect(lexer, LEX_LAMBDA);
  Expect(lexer, LEX_LBRACK);

  Expect(lexer, LEX_VAR);
  PushNewSymbol(&scope, lexer->token);
  Consume(lexer);
  for (*cnt = 1; lexer->type != LEX_RBRACK; ++*cnt) {
    Match(lexer, LEX_VAR);
    PushNewSymbol(&scope, lexer->token);
    Consume(lexer);
  }

  Consume(lexer);
  ap = ParseExpr(lexer, scope);
  Expect(lexer, LEX_RBRACK);

  for (i = *cnt; i > 0; --i) {
    ap = Ast_Cur(ap);
    Pool_Free(&g_symbol_pool, Pop(&scope));
  }

  return ap;
}

