#ifndef POOL_H_
#define POOL_H_

#include <stddef.h>

#include "node.h"

#define INIT_POOL(pool, elems, type) {              \
  sizeof(type),                       /* size */    \
  (char * const)(pool),               /* start */   \
  (char * const)((pool) + (elems)),   /* limit */   \
  (char *)(pool),                     /* max */     \
  NULL                                /* avail */   \
}

#define Pool_Free(me, item)       Push(&(me)->avail, (item))
#define Pool_FreeList(me, item)   Append(&(me)->avail, (item))

enum {
  N_ELEMS = 1024
};

typedef struct {
  size_t        size;
  char * const  start;
  char * const  limit;
  char *        max;
  node_t *      avail;
} pool_t;

extern pool_t   g_ast_pool;
extern pool_t   g_env_pool;
extern pool_t   g_symbol_pool;

extern void *   Pool_Alloc(pool_t * const);
extern void *   Pool_Calloc(pool_t * const);
extern void     Pool_Clear(pool_t * const);

#endif /* POOL_H_ */

