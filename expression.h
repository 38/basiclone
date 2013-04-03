/******************************************
	expression.h: Expression parser
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_EXPRESSION
#define __HEADER_EXPRESSION
#include "common.h"


#ifdef  __EXPRESSION_IMP
#define EXPRESSION_VARIABLE
#else
#define EXPRESSION_VARIABLE extern
#endif 

#include "token.h"
Data* parse_expression(char** p);
void  parse_ws(char** p);
char* parse_id(char** p);
Data* parse_string(char** p);
Data* parse_aexpr(char** p);
Subscript* parse_arg(char** p);
void  dispose( Data* );
void dispose_arg( Subscript* );
#endif
