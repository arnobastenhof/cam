#ifndef ENV_H_
#define ENV_H_

#include "ast.h"

#define Env_Nil()  Env_New(ENV_NIL)

typedef struct env_s env_t;

typedef enum {
  ENV_PAIR,
  ENV_NIL,      /* sentinel */
  ENV_INT,      /* non-zero integers */
  ENV_CLOSURE,
} envType_t;

typedef struct {
  env_t *       ctx;
  ast_t *       code;
} closure_t;

struct env_s {
  node_t        base;
  union {
    int         num;
    env_t *     rchild;
    closure_t   cl;
  }             u;
  envType_t     type;
};

extern env_t *    Env_New(envType_t);
extern env_t *    Env_Int(const int);
extern env_t *    Env_Pair(env_t * const, env_t * const);
extern env_t *    Env_Closure(env_t * const, ast_t * const);
extern env_t *    Env_Copy(const env_t * const);
extern void       Env_Free(env_t ** const);
extern void       Env_FreeList(env_t ** const);

#endif /* ENV_H_ */

