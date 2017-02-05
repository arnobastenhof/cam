#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "cam.h"
#include "env.h"
#include "except.h"
#include "lexer.h"
#include "optim.h"
#include "parser.h"
#include "pool.h"

enum {
  BUFF_SZ = 256
};

jmp_buf *   g_handler;

static int
Evaluate(const char * const buff)
{
  ast_t * ap;
  cam_t   cam;
  lexer_t lexer;
  optim_t optim;
  int     result = -1;

  Lexer_Init(&lexer, buff);
  ap = Parse(&lexer);

  do {
    Optim_Init(&optim);
    Ast_Traverse(ap, (visit_t *)&optim);
    Ast_Free(&ap);
    ap = Pop(&optim.stack);
    assert(IsEmpty(optim.stack));
  } while (optim.cnt != 0);

  Cam_Init(&cam);
  Ast_Traverse(ap, (visit_t *)&cam);
  assert(cam.env->type == ENV_INT);
  result = cam.env->u.num;

  Cam_Free(&cam);
  Ast_Free(&ap);
  return result;
}

int
main()
{
  char    buff[BUFF_SZ];
  char *  cp;

  for (;;) {
    for (cp=buff; cp-buff<BUFF_SZ && (*cp=getchar())!='\n'; ++cp)
      ;
    if (cp - buff == BUFF_SZ) {
      fprintf(stderr, "Input too long.\n");
      continue;
    }
    *cp = '\0';

    if (strcmp("halt", buff) == 0) {
      return 0;
    }

    TRY
      printf("%d\n", Evaluate(buff));
    CATCH
      Pool_Clear(&g_ast_pool);
      Pool_Clear(&g_env_pool);
      Pool_Clear(&g_symbol_pool);
    END
  }
}

