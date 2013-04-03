/******************************************
	expression.c: Expression parser
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#include "expression.h"
#include "io_calls.h"
#include "math.h"
#define ISNUMERIC(x) ((x)>='0' && (x)<='9') 
#define ISALPHA(x)   ((x)>='A' && (x)<='Z')
void dispose( Data* data )
{
	
	if( data && data->disposable )       /*dispose the temp variable*/
	{
		if( data->type == TYPE_STRING && 
		    data->storage.String != init_value[0].String )
			free(data->storage.String);
		free( data );
	}
}
void dispose_arg( Subscript* arg )
{
	int i;
	for(  i = 0 ; i < arg->dimensions ; i ++ )
		dispose(arg->dimension[i]);
	free(arg);
}
void  parse_ws( char** p )
{
	while( (**p == '\t' || **p == ' ') && **p )
		(*p)++;
}
int   parse_sign( char** p )
{
	int ret = 0;
	while(1)
	{
		parse_ws(p);
		if( !**p ) return ret;
		if( **p != '+' && **p != '-' )
			return ret;
		else
		{
			if( ret == 0 ) ret = 1;
			if( **p == '-' )
				ret = -ret;
		}
		(*p)++;
	}
}
char* parse_id(char **p)
{
	char* ret = (char*)malloc(32);
	int   len = 0;
	while( ISNUMERIC(**p) || ISALPHA(**p) || **p == '_' )
	{
		if( len < 30)
			ret[len++] = **p;
		(*p)++;
	} 
	if(**p == '%' || **p == '$')
		ret[len++] = *((*p)++);
	ret[len] = 0;
	return ret;	
}
Data* parse_expression(char** p);
Data* parse_term( char** p );
Data* parse_string( char** p )
{
	char* ret = (char*)malloc(65536);
	int   len = 0;
	if( *((*p)++) != '"' )
	{
		free(ret);
		return Raise(SYNTAX);	
	}
	while( **p )
	{
		//putchar(**p);
		if( (*p)[0] == '"' )
			if( (*p)[1] == '"' && (*p)[2] == '"' && (*p)[3] == '"' )
				(*p) += 3 , ret[len++] = '"';
			else
			{
				//(*p)++;
				break;
			}
		else
			if( len < 65536 ) ret[len++] = **p;
		(*p)++; 
	}
	if( **p != '"' )
	{
		free(ret);
		return Raise(SYNTAX);
	}
	(*p)++;
	ret[len] = 0;
	return Data_new( TYPE_STRING , (Storage)ret , 1 );
}
Data* parse_numeric( char** p )
{
	double ret = 0;
	if( ISNUMERIC(**p) )
		while( ISNUMERIC(**p) )
		{
			ret = ret * 10 + (**p) - '0';
			(*p)++;
		} 
	if( **p == '.' )
	{
		(*p)++;	
		double scale = .1;
		while( ISNUMERIC(**p) )
		{
			ret = ret + scale * ( **p - '0' );
			scale /= 10;
			(*p)++;
		}
	}
	if( **p == 'E' )
	{
		int sign = 1;
		int exponent = 0;
		(*p)++;
		if( **p == '-' )
			sign = -1 , (*p)++;
		else if( **p == '+' )
			(*p)++;
		while( ISNUMERIC(**p) )
		{
			exponent = exponent * 10 + **p - '0';
			(*p)++;
		}
#ifndef __WIN32
		if(sign > 0)
			ret *= exp10(exponent);
		else
			ret /= exp10(exponent);
#else
		if(sign > 0)
			ret *= exp(log(10)*exponent);
		else
			ret /= exp(log(10)*exponent);
	
#endif
	}
	return Data_new( TYPE_REAL , (Storage)ret , 1 );
}
Subscript* parse_arg( char** p )
{
	Subscript* ret = (Subscript*)malloc(sizeof(Subscript));
	(*p)++;
	ret->dimensions = 0;
	while(1)
	{
		if( (ret->dimension[ret->dimensions++] = parse_expression(p))->type == TYPE_ERROR )
			break;
		parse_ws(p);
		if( **p == ')' )
		{
			(*p)++;
			break;
		}
		if( **p != ',' )
		{
			dispose_arg(ret);
			return NULL;
		}
		else (*p)++;
		
		
	}
	return ret;
}
Data* parse_factor( char** p)
{
	Data* ret;
	int curves = 0;
	while(1)
	{
		parse_ws(p);
		if( **p == '(' )
			curves ++;
		else 
			break;
		(*p)++;
		break;
	}
	if( curves )
	{/* (((...(( parsed , expression expected */
		ret = parse_expression(p);
		if( ret->type == TYPE_ERROR )
		{/* error occured*/
			return ret;
		}
		while(curves--)
		{
			/*check ')'*/
			parse_ws(p);
			if( **p != ')' )
			{/*syntax error , curves dose not match*/
				dispose(ret);
				return Raise(SYNTAX);
			}
			(*p)++;
		}
	}
	else
	{/* Constant , Variable , Array and Functions*/
	 /* INKEY$*/
		parse_ws(p);
		if( **p == '"' )
			ret = parse_string(p);
		else if(ISNUMERIC(**p) || **p == '.') 
			ret = parse_numeric(p);
		else if( ISALPHA(**p) || **p == '_' )
		{
			char* id = parse_id(p);
			parse_ws(p);
			if(**p == '(' )
			{
				Subscript *arg;
				Function* f;
				arg = parse_arg(p);
				f = get_fun(id);
				if( f == NULL )
					return Raise(SYNTAX);
				if( arg == NULL )
					return Raise(SYNTAX);
				if( arg->dimension[arg->dimensions-1]->type == TYPE_ERROR )
				{
					ret = Data_new( TYPE_ERROR , 
					                (Storage)arg->dimension[--arg->dimensions]->storage,
					                1);
					free(arg);
				}
				else
				{
					if( f->is_function )
					{/*function call*/
						ret = f->f.foo( arg );
					}
					else
					{/*array*/
						ret = get_arr_element( &f->f.arr , arg );
					}
					dispose_arg(arg);
				}
			}
			else
			{
				/*variable*/
				if( strcmp( id , "INKEY$") == 0 )
				{
					ret = Data_new( TYPE_STRING , (Storage)(char*)malloc(2) , 1 );
					ret->storage.String[0] = inkey();
					ret->storage.String[1] = 0;
				}
				else if( strcmp( id , "__LASTERR$" ) == 0 )
				{
					ret = Data_new( TYPE_STRING , (Storage)(char*)malloc(strlen(err_str[Last_Error.id])) , 1 );
					strcpy( ret->storage.String , err_str[Last_Error.id] );
				}
				else if( strcmp( id, "__LASTERRNO") == 0 )
				{
					ret = Data_new( TYPE_REAL , (Storage)(double)Last_Error.id,1);
				}
				else
					ret = get_var(id);
			}
			free(id);
		}
		else
		{
			ret =Raise(SYNTAX);
		}
	}
	return ret;
}
Data* parse_term( char** p )
{
	Data* ret = NULL;
	while(1)
	{
		char op;
		int sign;
		if( ret )
		{
			parse_ws(p);
			op = *((*p));
			if( op != '*' && op != '/' ) 
			{
				break; /*last chr should not be parsed*/
			}
			else
				(*p)++;
		}
		sign=parse_sign(p);
		if( sign == 0 ) sign = 1;
		Data* factor = parse_factor(p);
		if( factor->type == TYPE_ERROR )
		{
			dispose(ret);
			return factor; /*throw*/
		}
		if( sign == -1 )
		{
			if( factor->type == TYPE_STRING )
			{
				dispose(factor);
				dispose(ret);
				return Raise(SYNTAX);
			}
			else if( factor->type == TYPE_REAL )
				factor->storage.Real = -factor->storage.Real;
			else
				factor->storage.Integer = - factor->storage.Integer;
		}
		parse_ws(p);
		if( **p == '^' )
		{/*exponent*/
			(*p)++;
			Data* exponent = parse_factor(p);
			factor = temp_var(factor);
			if( exponent->type == TYPE_ERROR )
			{
				dispose(ret);
				dispose(factor);
				return exponent;
			}
			if( exponent->type == TYPE_STRING || factor->type == TYPE_STRING)
			{
				dispose(ret);
				dispose(factor);	
				dispose(exponent);
				return Data_new( TYPE_ERROR , (Storage)Error_new(TYPE_MISMATCH) , 1 );
			}
			if( factor->type == TYPE_INTEGER )
			{
				factor->storage.Real = factor->storage.Integer;
				factor->type = TYPE_REAL;
			}
			if( exponent->type == TYPE_INTEGER )
			{
				factor->storage.Real = factor->storage.Integer;
				factor->type = TYPE_REAL;
			}
			factor->storage.Real = exp( log(factor->storage.Real) * exponent->storage.Real); 
			dispose(exponent);
		}
		
		if( ret == NULL )
			ret = factor;
		else
		{
			ret = temp_var(ret);
			if( ret->type == TYPE_STRING )
			{
				dispose(ret);
				dispose(factor);
				return Raise(TYPE_MISMATCH);
			}
			else
			{
				if( op == '*' )
				{
					if( ret->type == TYPE_REAL || factor->type == TYPE_REAL )
					{
						if( ret->type == TYPE_INTEGER )
						{
							ret->type = TYPE_REAL;
							ret->storage.Real = ret->storage.Integer;
						}
						if( factor->type == TYPE_INTEGER )
						{
							factor->type = TYPE_REAL;
							factor->storage.Real = factor->storage.Integer;
						}
						ret->storage.Real *= factor->storage.Real;
											
					}
					else
						ret->storage.Integer *= factor->storage.Integer;
				}
				else
				{
					if( ret->type == TYPE_INTEGER )
					{
						ret->type = TYPE_REAL;
						ret->storage.Real = ret->storage.Integer;
					}
					if( factor->type == TYPE_INTEGER )
					{
						factor->type = TYPE_REAL;
						factor->storage.Real = factor->storage.Integer;
					}
					if( factor->storage.Real == 0 )
					{
						dispose( ret );
						dispose( factor );
						return Raise(DIV_BY_ZERO);
					}
					ret->storage.Real /= factor->storage.Real;
				}
					
			}
			dispose(factor);
		}
	}
	return ret;
}
Data* parse_aexpr(char** p)
{
	Data* ret = NULL;
	while(parse_ws(p),**p)
	{
		int sign ;
		sign = parse_sign(p);
		if( !ret || sign )
		{
			Data* term = parse_term(p);
			if( term->type == TYPE_ERROR )
			{
				dispose(ret);
				return term;    /*Throw the error*/
			}
			else if( !ret )      /* First term */
			{
				if( sign != 0 && term->type == TYPE_STRING )   
						  /* There are one or more negitive/positive sign before A string*/
						  /* e.g. : +"illegal"*/
				{
					dispose(term);
					dispose(ret);						
					return Raise(SYNTAX); /*raise a syntax error*/
				}
				else ret = term;
				if( sign < 0 )
				{/*only INTEGER and REAL */
					if( term->type == TYPE_INTEGER )
						term->storage.Integer = -term->storage.Integer;
					else
						term->storage.Real = -term->storage.Real;
				}
			}
			else if( term->type == TYPE_STRING )
			{
				ret = temp_var(ret);
				if( sign > 0 )  /* AC + STRING */
				{
					if( ret->type != TYPE_STRING )
					{
						/* non-string + string is illegal */
						dispose(term);
						dispose(ret);
						return Raise(TYPE_MISMATCH);
					}
					/* do strcat */
					char* A = ret->storage.String;
					char* B = term->storage.String;
					int  lA = strlen(A);
					int  lB = strlen(B);
					char* temp = (char*)malloc( lA + lB + 1);
					strcpy( temp , A );
					strcpy( temp + lA , B );
					ret->storage.String = temp;
					if( A != init_value[TYPE_STRING].String )
						free(A);   /*dispose old string*/
					dispose(term);     /*dispose this term*/
				}
				else
				{
					/* AC - string is illegal */
					dispose(term);
					dispose(ret);
					return Raise(TYPE_MISMATCH);
				}
			}
			else if( term->type == TYPE_INTEGER )
			{
				ret = temp_var(ret);
				/*AC + INTEGER*/
				if( ret->type == TYPE_STRING )
				{/*Type Mismatch*/
					dispose(ret);
					dispose(term);
					return Raise(TYPE_MISMATCH);
				}
				else if( ret->type == TYPE_INTEGER )
				{/*Integer + Integer*/
					ret->storage.Integer += sign * term->storage.Integer;
					dispose(term);
				}
				else if( ret->type == TYPE_REAL )
				{/*Real + Integer*/
					ret->storage.Real += sign * term->storage.Integer;
					dispose(term);					
				}
			} 
			else if( term->type == TYPE_REAL )
			{

				double org;
				ret = temp_var(ret);
				/*AC + REAL */
				if( ret->type == TYPE_STRING )
				{/*Type Mismatch*/
					dispose(ret);
					dispose(term);
					return Raise(TYPE_MISMATCH);
				}


				if( ret->type == TYPE_REAL )
					org = ret->storage.Real;
				else
					org = ret->storage.Integer;
				ret->type = TYPE_REAL;
				ret->storage.Real = org + sign * term->storage.Real;
				dispose(term);
			}
		}
		else
			break; 
	}
	if( ret == NULL )
		ret=Raise(SYNTAX);
	return ret;
}
int parse_neg( char** p )
{
	int ret = 0;
	while(1)
	{
		parse_ws(p);
		if( p[0][0] == 'N' &&
		    p[0][1] == 'O' &&
		    p[0][2] == 'T' &&
		   (p[0][3] == ' ' ||
		    p[0][3] == '\t') )
		{
			/*TODO*/
			(*p)+=4;
			if( ret == 0 ) ret = 1;
			ret = -ret;
		}
		else return ret;
	}
}
Data* parse_expression( char **p )
{
	/*TODO: logic */
	//return parse_aexpr(p);
	Data* ret = NULL;
	while(1)
	{
		char op;
		int neg ;
		Data* aexpr;
		if( ret )
		{/*if it is not the first expression , a logic operator expected*/
			if( p[0][0] == 'A' && p[0][1] == 'N' && p[0][2] == 'D' && ( p[0][3] = ' ' || p[0][3] == '\t' ) )
			{
				op = 'A';
				(*p)+=4;
			}
			else if( p[0][0] == 'O' && p[0][1] == 'R' && ( p[0][2] = ' ' || p[0][2] == '\t' ) )
			 {
				op = 'O';
				(*p)+=3;
			}
			else break; /*if not operator */
		}
		neg  = parse_neg(p);
		aexpr = parse_aexpr(p);
		parse_ws(p);
		if( aexpr->type == TYPE_ERROR )
		{
			dispose(ret);
			return aexpr;
		}
		if( **p == '=' || **p == '>' || **p == '<' )
		{ 
			int flag;
			Data* right/* = parse_aexpr(p)*/;
			switch(**p)
		 	{
			case '=':
#define EQ 0
				flag = 0;  /*EQ*/
				(*p)++;
				break;
			case '>':
				if( p[0][1] == '=' )
				{
					(*p)+=2;
#define GE 3
					flag = 3;   /*GE*/
				}
				else
				{
#define G 1
					(*p)++;
					flag = 1;   /*G*/
				}
				break;
			case '<':
				if( p[0][1] == '=' )
				{
					(*p)+=2;
#define LE 4
					flag = 4; /*LE*/
				}
				else if( p[0][1] == '>' )
				{
					(*p)+=2;
#define NE 5
					flag = 5;  /*NE*/
				}
				else
				{
					(*p)++;
#define L 2
					flag = 2; /*L*/
				}
			}
			right = parse_aexpr(p);
			if( right->type == TYPE_ERROR )
			{
				dispose(ret);
				dispose(aexpr);
				return right;
			}
			if( right->type == TYPE_STRING )
			{//String
				if( aexpr->type != TYPE_STRING )
				{
					dispose(ret);
					dispose(aexpr);
					dispose(right);
					return Raise(TYPE_MISMATCH);
				}
				else
				{
					Data* temp = Data_new(TYPE_INTEGER,(Storage)(short)0,1); 
					int result = strcmp( aexpr->storage.String , right->storage.String);
					switch(flag)
					{
					case EQ:
						temp->storage.Integer = (result == 0);
						break;
					case L:
						temp->storage.Integer = (result < 0 );
						break;
					case LE:
						temp->storage.Integer = (result <= 0 );
						break;
					case G:
						temp->storage.Integer = (result > 0 );
						break;
					case GE:
						temp->storage.Integer = (result >= 0 );
						break;
					case NE:
						temp->storage.Integer = (result != 0 );
					}
					dispose(aexpr);
					dispose(right);
					aexpr = temp;
	
				}
			} 
			else 
			{//numeric
				if( aexpr->type == TYPE_STRING )
				{
					dispose(right);
					dispose(aexpr);
					dispose(ret);
					return Raise(TYPE_MISMATCH); 
				}
				else
				{
					Data* temp = Data_new(TYPE_INTEGER,(Storage)(short)0,1); 
					int result ;
					if( aexpr->type == TYPE_INTEGER )
					{
						aexpr->type = TYPE_REAL;
						aexpr->storage.Real = aexpr->storage.Integer;
					}
					if( right->type == TYPE_INTEGER )
					{
						right->type = TYPE_REAL;
						right->storage.Real = right->storage.Integer;
					}
					double t = aexpr->storage.Real - right->storage.Real;
					if( t < 0 ) result = -1;
					if( t == 0 ) result = 0;
					if( t > 0 ) result = 1;
					switch(flag)
					{
					case EQ:
						temp->storage.Integer = (result == 0);
						break;
					case L:
						temp->storage.Integer = (result < 0 );
						break;
					case LE:
						temp->storage.Integer = (result <= 0 );
						break;
					case G:
						temp->storage.Integer = (result > 0 );
						break;
					case GE:
						temp->storage.Integer = (result >= 0 );
						break;
					case NE:
						temp->storage.Integer = (result != 0 );
					}
					dispose(aexpr);
					dispose(right);
					aexpr = temp;
				}
			}	
		}
		if( aexpr->type == TYPE_ERROR )
		{
			dispose(ret);
			return aexpr; 
		}
		if( neg )
		{/*one or more NOT parsed*/
			if( aexpr->type == TYPE_STRING ) /*NOT "illegal"*/
			{
				dispose(aexpr);
				dispose(ret);
				return Raise(TYPE_MISMATCH );
			}
			if( aexpr->type == TYPE_REAL )
			{
				aexpr->type = TYPE_INTEGER;
				aexpr->storage.Integer = (int)(aexpr->storage.Real+.001);	
			}
			if( aexpr->storage.Integer )
				aexpr->storage.Integer = 1;
			else
				aexpr->storage.Integer = 0;
			if( neg < 0 )
				aexpr->storage.Integer = !aexpr->storage.Integer;
		}
		if( !ret )
		{/* first expression */
			ret = aexpr;
		}
		else
		{
			ret = temp_var(ret);
			if( aexpr->type == TYPE_STRING )
			{
				dispose(aexpr);
				dispose(ret);
				return Raise(TYPE_MISMATCH);
			}
			if( aexpr->type == TYPE_REAL )
			{
				aexpr->type = TYPE_INTEGER;
				aexpr->storage.Integer = (int)(aexpr->storage.Real+.001);
			}
			if( ret->type == TYPE_REAL )
			{
				ret->type = TYPE_INTEGER;
				ret->storage.Integer = (int)(ret->storage.Real+.001);
			}
			if( op == 'A' )
				ret->storage.Integer &= aexpr->storage.Integer;
			else
				ret->storage.Integer |= aexpr->storage.Integer;
			dispose(aexpr);
		}
	}
	return ret;
}
#ifdef EXPRESSION_TEST
#include "init.h"
#include "memory.h"
char testcode[10000];
//"STR$(RND(2)+VAL(MID$(STR$(SIN(1)+COS(1)),3,2)) + 2^.5)+\"FUCK\"";
int main()
{
	init();
	*memoryP(CUR_S) = 2;
	memoryV();
	/*/put_char('H');
	put_char('E');
	put_char('L');
	put_char('L');
	put_char('O');
	rect( 30 , 30 , 10 , 10 , 1 , 1 );
	ellipse( 35 , 35 , 15 , 15 , 0 , 1 );*/
	while(1)
	{
		gets(testcode);
		char *p =testcode;
		Data* data = parse_expression(&p);
		if( data->type == TYPE_ERROR )
			printf(" %s ERROR IN %d\n", err_str[data->storage.error.id] , data->storage.error.line_no);
		if( data->type == TYPE_REAL )
			printf(" REAL(%lf)\n",data->storage.Real );
		if( data->type == TYPE_INTEGER )
			printf(" INTEGER(%d)\n",data->storage.Integer );
		if( data->type == TYPE_STRING )
			printf(" STRING(%s)\n",data->storage.String );
		dispose(data);
	}	
	while( inkey() != 'q' );
	return 0;
}
#endif
