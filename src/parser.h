#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"
#include "lexer.h"

extern ast_t *  Parse(lexer_t * const);

#endif /* PARSER_H_ */

