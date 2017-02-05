#ifndef AST_H_
#define AST_H_ 

#include <stdarg.h>
#include <stdbool.h>

#include "node.h"

#define Ast_Id()               Ast_New(AST_ID, 0)
#define Ast_Fst()              Ast_New(AST_FST, 0)
#define Ast_Snd()              Ast_New(AST_SND, 0)
#define Ast_App()              Ast_New(AST_APP, 0)
#define Ast_Cur(child)         Ast_New(AST_CUR, 1, (child))
#define Ast_Pair(left, right)  Ast_New(AST_PAIR,2,(left),(right))
#define Ast_Comp(cnt, ...)     Ast_New(AST_COMP,(cnt),__VA_ARGS__)

#define Ast_Node(type)               Ast_New((type), 0)
#define Ast_AddChild(me,child)       Push(&(me)->rchild,(child))
#define Ast_SetChildren(me,children) (me)->rchild = (children)

typedef struct ast_s ast_t;

typedef enum {
  AST_ID,
  AST_APP,
  AST_QUOTE,
  AST_PLUS,
  AST_FST,
  AST_SND,
  AST_COMP,
  AST_PAIR,
  AST_CUR
} astType_t;

typedef struct visit_s visit_t;

typedef enum {
  SC_CONTINUE,
  SC_SKIP,
} statusCode_t;


typedef statusCode_t (*visitFunc_t)(visit_t * const, const ast_t *);

typedef struct {
  visitFunc_t   VisitId;
  visitFunc_t   VisitApp;
  visitFunc_t   VisitQuote;
  visitFunc_t   VisitPlus;
  visitFunc_t   VisitFst;
  visitFunc_t   VisitSnd;
  visitFunc_t   PreVisitComp;
  visitFunc_t   PreVisitPair;
  visitFunc_t   PreVisitCur;
  visitFunc_t   InVisitPair;
  visitFunc_t   PostVisitComp;
  visitFunc_t   PostVisitPair;
  visitFunc_t   PostVisitCur;
} visitVtbl_t;

struct ast_s {
  node_t    base;
  ast_t *   rchild;
  int       value;
  astType_t type;
};

struct visit_s {
  const visitVtbl_t * vptr;
};

extern ast_t * Ast_New(const astType_t, int, ...);
extern ast_t * Ast_Quote(const int);
extern ast_t * Ast_Plus(void);
extern void    Ast_Free(ast_t ** const);
extern void    Ast_Traverse(const ast_t * const, visit_t * const);

extern statusCode_t VisitDefault(visit_t * const, const ast_t *);

#endif /* AST_H_ */

