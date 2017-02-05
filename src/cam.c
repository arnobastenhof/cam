#include "cam.h"

#include <assert.h>

#include "pool.h"

static statusCode_t VisitFst(cam_t * const, const ast_t *);
static statusCode_t VisitSnd(cam_t * const, const ast_t *);
static statusCode_t VisitQuote(cam_t * const, const ast_t *);
static statusCode_t VisitApp(cam_t * const, const ast_t *);
static statusCode_t VisitCur(cam_t * const, const ast_t *);
static statusCode_t VisitPush(cam_t * const, const ast_t *);
static statusCode_t VisitSwap(cam_t * const, const ast_t *);
static statusCode_t VisitCons(cam_t * const, const ast_t *);
static statusCode_t VisitPlus(cam_t * const, const ast_t *);

void Cam_Init(cam_t * const me)
{
  static const visitVtbl_t vtbl = {
                  VisitDefault,   /* VisitId */
    (visitFunc_t) VisitApp,       /* VisitApp */
    (visitFunc_t) VisitQuote,     /* VisitQuote */
    (visitFunc_t) VisitPlus,      /* VisitPlus */
    (visitFunc_t) VisitFst,       /* VisitFst */
    (visitFunc_t) VisitSnd,       /* VisitSnd */
                  VisitDefault,   /* PreVisitComp */
    (visitFunc_t) VisitPush,      /* PreVisitPair */
    (visitFunc_t) VisitCur,       /* PreVisitCur */
    (visitFunc_t) VisitSwap,      /* InVisitPair */
                  VisitDefault,   /* PostVisitComp */
    (visitFunc_t) VisitCons,      /* PostVisitPair */
                  VisitDefault    /* PostVisitCur */
  };

  assert(me);

  me->env = Env_Nil();
  me->stack = NULL;
  me->base.vptr = &vtbl;
}

void Cam_Free(cam_t * const me)
{
  Env_Free(&me->env);
  Env_FreeList(&me->stack);
}

static statusCode_t
VisitQuote(cam_t * const me, const ast_t *ap)
{
  Env_Free(&me->env);
  me->env = Env_Int(ap->value);

  return SC_CONTINUE;
}

static statusCode_t
VisitPush(cam_t * const me, const ast_t *ap)
{
  (void)ap;

  Push(&me->stack, Env_Copy(me->env));

  return SC_CONTINUE;
}

static statusCode_t
VisitSwap(cam_t * const me, const ast_t *ap)
{
  env_t *  tmp;

  (void)ap;
  assert(me->stack != NULL);

  tmp = Pop(&me->stack);
  Push(&me->stack, me->env);
  me->env = tmp;

  return SC_CONTINUE;
}

static statusCode_t
VisitCons(cam_t * const me, const ast_t *ap)
{
  (void)ap;
  assert(me->stack != NULL);

  me->env = Env_Pair((env_t *)Pop(&me->stack), me->env);

  return SC_CONTINUE;
}

static statusCode_t
VisitFst(cam_t * const me, const ast_t *ap)
{
  env_t *  proj;

  (void)ap;
  assert(me->env->type == ENV_PAIR);

  proj = Pop(&me->env->u.rchild);
  proj->base.link = NULL; /* prevent dangling pointer */
  Env_Free(&me->env);
  me->env = proj;

  return SC_CONTINUE;
}

static statusCode_t
VisitSnd(cam_t * const me, const ast_t *ap)
{
  env_t *  proj;

  (void)ap;
  assert(me->env->type == ENV_PAIR);

  proj = me->env->u.rchild;
  Env_Free((env_t **)&proj->base.link);
  Pool_Free(&g_env_pool, (node_t *)me->env);
  me->env = proj;

  return SC_CONTINUE;
}

static statusCode_t
VisitCur(cam_t * const me, const ast_t *ap)
{
  me->env = Env_Closure(me->env, ap->rchild);

  return SC_SKIP;
}

static statusCode_t
VisitApp(cam_t * const me, const ast_t *ap)
{
  env_t *   closure;

  (void)ap;
  assert(me->env->type == ENV_PAIR);

  closure = Pop(&me->env->u.rchild);
  assert(closure->type == ENV_CLOSURE);

  Push(&me->env->u.rchild, closure->u.cl.ctx);
  Ast_Traverse(closure->u.cl.code, (visit_t *)me);
  Pool_Free(&g_env_pool, (node_t *)closure);

  return SC_CONTINUE;
}

static statusCode_t
VisitPlus(cam_t * const me, const ast_t *ap)
{
  env_t *  left;
  env_t *  right;

  (void)ap;

  assert(me->env->type == ENV_PAIR);
  left = Pop(&me->env->u.rchild);
  assert(left->type == ENV_INT);
  right = Pop(&me->env->u.rchild);
  assert(right->type == ENV_INT);

  left->u.num += right->u.num;
  left->base.link = NULL; /* prevent dangling pointer */
  Pool_Free(&g_env_pool, (node_t *)right);
  Pool_Free(&g_env_pool, (node_t *)me->env);
  me->env = left;

  return SC_CONTINUE;
}

