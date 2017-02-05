#include "ast.h"

#include <assert.h>

#include "pool.h"

static ast_t  g_pool[N_ELEMS];
pool_t        g_ast_pool = INIT_POOL(g_pool, N_ELEMS, ast_t);

ast_t *
Ast_New(const astType_t type, int cnt, ...)
{
  ast_t * me;
  va_list argp;

  me = Pool_Calloc(&g_ast_pool);
  me->type = type;

  va_start(argp, cnt);
  while (cnt-- > 0) {
    Enqueue(&me->rchild, va_arg(argp, ast_t *));
  }
  va_end(argp);

  return me;
}

ast_t *
Ast_Quote(const int value)
{
  ast_t * me;

  me = Pool_Calloc(&g_ast_pool);
  me->type = AST_QUOTE;
  me->value = value;
  return me;
}

ast_t *
Ast_Plus(void)
{
  return Ast_Cur(Ast_Comp(2, Ast_Snd(), Ast_Node(AST_PLUS)));
}

static node_t *
Flatten(ast_t *me)
{
  ast_t * it = me;

  assert(me);

  me->base.link = (node_t *)me;
  do {
    Append(&me, it->rchild);
  } while ((it = Link(it)) != Link(me));

  return (node_t *)me;
}

void
Ast_Free(ast_t ** const me)
{
  assert(me);

  if (*me == NULL) {
    return;
  }
  Pool_FreeList(&g_ast_pool, Flatten(*me));
  *me = NULL;
}

static inline statusCode_t
Visit(const ast_t * const me, visit_t * const vp, const size_t offset)
{
  return (*((visitFunc_t *)vp->vptr)[offset])(vp, me);
}

void
Ast_Traverse(const ast_t * const me, visit_t * const vp)
{
  ast_t *       ap;
  statusCode_t  sc;

  assert(me);
  assert(vp);

  sc = Visit(me, vp, me->type);

  if (sc == SC_CONTINUE && (me->rchild)) {
    ap = Link(me->rchild);
    Ast_Traverse(ap, vp);
    if (me->type == AST_PAIR) {
      Visit(me, vp, 9); /* InVisitPair */
    }
    while ((ap = Link(ap)) != Link(me->rchild)) {
      Ast_Traverse(ap, vp);
    }
  }

  if (me->type == AST_CUR || me->type == AST_COMP 
      || me->type == AST_PAIR) {
    Visit(me, vp, me->type + 4);
  }
}

statusCode_t
VisitDefault(visit_t * const me, const ast_t *ap)
{
  (void)me;
  (void)ap;
  return SC_CONTINUE;
}

