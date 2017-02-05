#include "env.h"

#include <assert.h>

#include "pool.h"

static env_t    g_pool[N_ELEMS];
pool_t          g_env_pool = INIT_POOL(g_pool, N_ELEMS, env_t);

env_t *
Env_New(envType_t type)
{
  env_t * me = Pool_Calloc(&g_env_pool);
  me->type = type;
  return me;
}

env_t *
Env_Int(const int num)
{
  env_t * me = Pool_Calloc(&g_env_pool);
  me->type = ENV_INT;
  me->u.num = num;
  return me;
}

env_t *
Env_Pair(env_t * const left, env_t * const right)
{
  env_t *  me;

  assert(left);
  assert(right);

  me = Pool_Calloc(&g_env_pool);
  me->type = ENV_PAIR;
  Push(&me->u.rchild, right);
  Push(&me->u.rchild, left);
  return me;
}

env_t *
Env_Closure(env_t * const ctx, ast_t * const code)
{
  env_t *  me;

  assert(ctx);
  assert(code);

  me = Pool_Calloc(&g_env_pool);
  me->type = ENV_CLOSURE;
  me->u.cl.ctx = ctx;
  me->u.cl.code = code;
  return me;
}

env_t *
Env_Copy(const env_t * const me)
{
  env_t *  copy;

  assert(me);

  copy = Env_New(me->type);
  switch(me->type) {
  case ENV_NIL:
    break;
  case ENV_INT:
    copy->u.num = me->u.num;
    break;
  case ENV_CLOSURE:
    copy->u.cl.ctx = Env_Copy(me->u.cl.ctx);
    copy->u.cl.code = me->u.cl.code;
    break;
  case ENV_PAIR:
    Push(&copy->u.rchild, Env_Copy(me->u.rchild));
    Push(&copy->u.rchild, Env_Copy((env_t *)Link(me->u.rchild)));
    break;
  default:
    assert(0);
  }
  return copy;
}

static node_t *
Flatten(env_t * me)
{
  env_t * it;

  assert(me);

  it = Link(me);
  do {
    assert(it);
    if (it->type == ENV_PAIR) {
      Append(&me, it->u.rchild);
    } else if (it->type == ENV_CLOSURE) {
      Append(&me, it->u.cl.ctx);
    }
  } while ((it = Link(it)) != Link(me));

  return (node_t *)me;
}

void
Env_Free(env_t ** const me)
{
  assert(me);

  if (*me == NULL) {
    return;
  }
  (*me)->base.link = (node_t *)*me;
  Pool_FreeList(&g_env_pool, Flatten(*me));
  *me = NULL;
}

void
Env_FreeList(env_t ** const me)
{
  assert(me);

  if (*me == NULL) {
    return;
  }
  Pool_FreeList(&g_env_pool, Flatten(*me));
  *me = NULL;
}

