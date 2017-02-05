#ifndef CAM_H_
#define CAM_H_

#include <stdbool.h>

#include "ast.h"
#include "env.h"

typedef struct {
  visit_t   base;
  env_t *   env;
  env_t *   stack;
} cam_t;

extern void Cam_Init(cam_t * const);
extern void Cam_Free(cam_t * const);

#endif /* CAM_H_ */

