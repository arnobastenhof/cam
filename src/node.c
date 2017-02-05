#include "node.h"

#include <assert.h>
#include <stddef.h>

void
Node_AddFirst(node_t ** const me, node_t * const np)
{
  assert(me);
  assert(np);

  if ((*me)) {
    np->link = (*me)->link;
    (*me)->link = np;
  } else {
    *me = np;
    np->link = np;
  }
}

void
Node_AddLast(node_t ** const me, node_t * const np)
{
  assert(me);
  assert(np);

  Node_AddFirst(me, np);
  *me = np;
}

void *
Node_RemoveFirst(node_t ** const me)
{
  node_t *  np;

  assert(me);

  if (IsEmpty(*me)) {
    return NULL;
  }

  np = (*me)->link;
  (*me)->link = np->link;
  if (*me == np) {
    *me = NULL;
  }
  return np;
}

void
Node_Prepend(node_t ** const me, node_t * const np)
{
  node_t *  tmp;

  if (IsEmpty(np)) {
    return;
  }
  if (IsEmpty(*me)) {
    *me = np;
    return;
  }
  tmp = np->link;
  np->link = (*me)->link;
  (*me)->link = tmp;
}

void
Node_Append(node_t ** const me, node_t * const np)
{
  node_t  *tmp;

  if (IsEmpty(np)) {
    return;
  }
  if ((*me)) {
    tmp = np->link;
    np->link = (*me)->link;
    (*me)->link = tmp;
  }
  *me = np;
}

