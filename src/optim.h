#ifndef OPTIM_H_
#define OPTIM_H_

#include "ast.h"

typedef struct {
  visit_t   base;
  node_t *  stack;
  int       cnt;
} optim_t;

extern void Optim_Init(optim_t * const);

#endif /* OPTIM_H_ */

