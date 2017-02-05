#include "optim.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "pool.h"

static statusCode_t PreVisitParent(optim_t * const, const ast_t *);
static statusCode_t PostVisitParent(optim_t * const, const ast_t *);
static statusCode_t VisitLeaf(optim_t * const, const ast_t *);
static statusCode_t VisitFst(optim_t * const, const ast_t *);
static statusCode_t VisitSnd(optim_t * const, const ast_t *);
static statusCode_t VisitApp(optim_t * const, const ast_t *);

void
Optim_Init(optim_t * const me)
{
  static const visitVtbl_t vtbl = {
    (visitFunc_t) VisitLeaf,        /* VisitId */
    (visitFunc_t) VisitApp,         /* VisitApp */
    (visitFunc_t) VisitLeaf,        /* VisitQuote */
    (visitFunc_t) VisitLeaf,        /* VisitPlus */
    (visitFunc_t) VisitFst,         /* VisitFst */
    (visitFunc_t) VisitSnd,         /* VisitSnd */
    (visitFunc_t) PreVisitParent,   /* PreVisitComp */
    (visitFunc_t) PreVisitParent,   /* PreVisitPair */
    (visitFunc_t) PreVisitParent,   /* PreVisitCur */
                  VisitDefault,     /* InVisitPair */
    (visitFunc_t) PostVisitParent,  /* PostVisitComp */
    (visitFunc_t) PostVisitParent,  /* PostVisitPair */
    (visitFunc_t) PostVisitParent   /* PostVisitCur */
  };

  assert(me);

  me->stack = NULL;
  me->cnt = 0;
  me->base.vptr = &vtbl;
}

static inline bool
IsSibling(const ast_t * const ap)
{
  return ap->rchild != ap;
}

static statusCode_t
PreVisitParent(optim_t * const me, const ast_t *ap)
{
  ast_t * copy;

  (void)ap;
  copy = Ast_Node(ap->type);
  copy->rchild = copy;
  Push(&me->stack, copy);
  return SC_CONTINUE;
}

static statusCode_t
PostVisitParent(optim_t * const me, const ast_t *ap)
{
  ast_t *   head;
  ast_t *   children = NULL;

  for (;;) {
    head = Pop(&me->stack);
    assert(head);
    if (!IsSibling(head)) {
      /* head must be a parent */
      break;
    }
    if (ap->type == AST_COMP) {
      switch (head->type) {
      case AST_COMP:
        Prepend(&children, head->rchild);
        /* Fall-through */
      case AST_ID:
        Pool_Free(&g_ast_pool, (node_t *)head);
        ++me->cnt;
        continue;
      default:
        break;
      }
    }
    Push(&children, head);
  }

  Ast_SetChildren(head, children);

  if (head->type == AST_COMP && head->rchild == NULL) {
    head->type = AST_ID;
  }

  Push(&me->stack, head);

  return SC_CONTINUE;
}

static statusCode_t
VisitLeaf(optim_t * const me, const ast_t *ap)
{
  ast_t * copy;

  copy = Ast_Node(ap->type);
  copy->value = ap->value;
  Push(&me->stack, copy);

  return SC_CONTINUE;
}

static statusCode_t
VisitFst(optim_t * const me, const ast_t *ap)
{
  ast_t * head;

  if ((head = Peek(me->stack)) && IsSibling(head)
        && head->type == AST_PAIR) {
    Pop(&me->stack);
    Push(&me->stack, Pop(&head->rchild));
    Ast_Free(&head);
    ++me->cnt;
    return SC_CONTINUE;
  }
  return VisitLeaf(me, ap);
}

static statusCode_t
VisitSnd(optim_t * const me, const ast_t *ap)
{
  ast_t * head;

  if ((head = Peek(me->stack)) && IsSibling(head)
        && head->type == AST_PAIR) {
    Pop(&me->stack);
    Ast_Free((ast_t **)&head->rchild->base.link);
    Push(&me->stack, head->rchild);
    Pool_Free(&g_ast_pool, (node_t *)head);
    ++me->cnt;
    return SC_CONTINUE;
  }
  return VisitLeaf(me, ap);
}

static statusCode_t
VisitApp(optim_t * const me, const ast_t *ap)
{
  ast_t * head;
  ast_t * left;

  if ((head = Peek(me->stack)) && IsSibling(head)
      && head->type == AST_PAIR && (left = Peek(head->rchild))
      && left->type == AST_CUR) {
    Pop(&head->rchild);
    Ast_AddChild(head, Ast_Id());
    Push(&me->stack, left->rchild);
    Pool_Free(&g_ast_pool, (node_t *)left);
    ++me->cnt;
    return SC_CONTINUE;
  }
  return VisitLeaf(me, ap);
}

