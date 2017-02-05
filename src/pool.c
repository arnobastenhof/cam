#include "pool.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "except.h"
#include "node.h"

void *
Pool_Alloc(pool_t * const me)
{
  assert(me);

  if ((me->avail)) {
    return Pop(&me->avail);
  }
  if (me->max < me->limit) {
    me->max += me->size;
    assert(me->max <= me->limit);
    return me->max - me->size;
  }
  fprintf(stderr, "Out of memory.\n");
  THROW;
}

void *
Pool_Calloc(pool_t * const me)
{
  void *  ptr;

  assert(me);

  ptr = Pool_Alloc(me);
  memset(ptr, 0, me->size);
  return ptr;
}

void
Pool_Clear(pool_t * const me)
{
  assert(me);

  me->avail = NULL;
  me->max = (char *)me->start;
}

