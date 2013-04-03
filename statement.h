/******************************************
	statement.h: Statement parser
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_STATEMENT
#define __HEADER_STATEMENT
#include "io_calls.h"
#include "memory.h"
#include "expression.h"
#include "token.h"
#include "common.h"
Data* parse_program(char **program);
void  output_error(Data* error);
#endif
