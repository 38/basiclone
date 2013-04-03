/******************************************
	statement.h: Statement parser
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __IMP_STATEMENT
#include "statement.h"
/*TODO: while/wend fileIO */
typedef struct
{
	Data* ctl_var;
	Data* endv;
	Data* increment;
	int line_no;
	char* start_p;
	char* end_s;
} For; /*struct for a for loop*/
int goto_line_no = -1; /*set when the program requests to jump to a specified line,
			 the program parser will response the request and set
			 the variable to -1 when the request has been processed*/
char* start_pos = NULL;
char* end_s     = NULL;
char* cur_ends  = NULL;
Data* data_store[65536];   /* the storage of DATA statement */
int   data_ptr;		   /* pointer of READ statement*/
int   data_count;	   /* number of data*/ 
int   prog_stack[65536];   /* call stack*/
char* prog_ends [65536];
char* prog_line [65536];
For   for_list[65536];
int   for_ptr;
int   ps_ptr;
int   delay;
int   debug;
int  str_equ( char**p , char* q)
{
	int i;
	for( i=0 ; *q ; q++ , i++)
		if( p[0][i] != *q )
			return 0;
	if( p[0][i] == ':' || p[0][i] == ' ' || p[0][i] == '\t' || p[0][i] == '\n' || p[0][i] == '\r' || p[0][i]  == 0 || p[0][i] == '#')
		return i;
	else
		return 0;
}
void draw_string(char* str)
{
	int i;
	if(debug==0)
	{
		int len = strlen(str);
		for( i = 0 ; i < len ; i ++ )
			put_char(str[i]);
	}
	else
		for( i = 0 ; i < strlen(str) ; i ++ )
			if( str[i] != '\r' )
				putchar(str[i]);
			else
				putchar('\n');
}
char* get_str()
{
	char* _i_buffer = (char*) malloc(101);
	char ch;
	int len = 0;
	while( (ch = inkey()) != 13 )
	{
		if( ch == 8 )
		{//backspace
			if( len )
			{
				memoryP(0);
				ram[CUR_X]--;
				if(ram[CUR_X] == 255 )
					ram[CUR_Y] -- , ram[CUR_X] = TEXT_W - 1;
				ram[ TEXT_BUF + TEXT_W * ram[CUR_Y] + ram[CUR_X] ] = ' ';
				memoryV();
				_i_buffer[--len] = 0;
			}
		}
		else
		{
			if( len >= 100 ) continue;
			put_char(ch);
			_i_buffer[len++] = ch;
			//if( cur >= len ) len ++;
		}
		//printf("%d\n",ch);
	}
	put_char(13);
	_i_buffer[len] = 0;
	return _i_buffer;
}
Data* parse_PRINT(char** p)
{
	int fn = -1;	
	if( **p == '#' )
	{
		(*p)++;	
		Data* temp = parse_expression(p);
		if( temp->type == TYPE_ERROR )
		{
			return temp;
		}
		if( temp->type == TYPE_STRING )
		{
			dispose(temp);
			return Raise(TYPE_MISMATCH);
		}
		if( temp->type == TYPE_REAL )
			fn = (int)(temp->storage.Real + .001 );
		else if( temp->type == TYPE_INTEGER )
			fn = temp->storage.Integer;
		parse_ws(p);
		dispose(temp);
		if( **p != ',' )
			return Raise(SYNTAX);
		else (*p)++;
		if( fn<0 || fn>255 || !file_list[fn].used )
			return Raise(FILE_NUMBER);
		if( file_list[fn].mode != FILE_MODE_OUTPUT )
			return Raise(FILE_MODE);
	}
	while(1)
	{
		if( **p != ',' && **p != ';' && **p != ':' && **p != 0 )
		{
			Data* val = parse_expression(p);
			if( val->type == TYPE_ERROR )
			{
				dispose(val);
				return val;			
			}
			if( val->type == TYPE_STRING )
			{
				char buffer[1024];
				sprintf( buffer , "%s" , val->storage.String );
				if( fn == -1 )draw_string(buffer);
				else fputs( buffer , file_list[fn].fp );
			}
			if( val->type == TYPE_REAL )
			{
				char buffer[1024];
				sprintf( buffer , "%lg" ,val->storage.Real );
				if( fn == -1 )draw_string(buffer);
				else fputs( buffer , file_list[fn].fp );
			}
			if( val->type == TYPE_INTEGER )
			{
				char buffer[1024];
				sprintf( buffer , "%d" , val->storage.Integer );
				if( fn == -1 )draw_string(buffer);
				else fputs( buffer , file_list[fn].fp );
			}
			if (fn != -1 ) fputc( '\n' , file_list[fn].fp );
			dispose(val);
			parse_ws(p);
		}
		if( **p == 0 || **p == ':' || str_equ(p,"ELSE"))
		{
			if( fn == -1 )
			{
				if(!debug) put_char(13); else putchar(10);
			}
			break;
 		}
		else if( **p == ';' ); 
		else if( **p == ',' )
		{if(fn==-1){if(!debug) put_char(13); else putchar(10);}}
		(*p)++;
		parse_ws(p);
		if( **p == 0 || **p == ':' || str_equ(p,"ELSE"))
			break;

	}
	if( **p == ';' ) (*p)++;
	return NULL;
} 
Data* parse_INPUT(char** p)
{
	int fn = -1;
	if( **p == '#' )
	{
		(*p)++;	
		Data* temp = parse_expression(p);
		if( temp->type == TYPE_ERROR )
		{
			return temp;
		}
		if( temp->type == TYPE_STRING )
		{
			dispose(temp);
			return Raise(TYPE_MISMATCH);
		}
		if( temp->type == TYPE_REAL )
			fn = (int)(temp->storage.Real + .001 );
		else if( temp->type == TYPE_INTEGER )
			fn = temp->storage.Integer;
		parse_ws(p);
		dispose(temp);
		if( **p != ',' )
			return Raise(SYNTAX);
		else (*p)++;
		if( fn<0 || fn>255 || !file_list[fn].used )
			return Raise(FILE_NUMBER);
		if( file_list[fn].mode != FILE_MODE_INPUT )
			return Raise(FILE_MODE);
	}
	while(1)
	{
		Data* prompt;
		Data* data;
		if(**p == '"')
		{
			/*Prompt*/
			prompt = parse_string(p);
			parse_ws(p);
			if( **p != ';' )
			{
				dispose(prompt);
				return Raise(SYNTAX);
			}
			if( prompt->type == TYPE_ERROR )
				return prompt;
			(*p)++;
		}
		else
		{
			prompt= Data_new(TYPE_STRING,(Storage)(char*)malloc(2),1);
			prompt->storage.String[0] = '?';
			prompt->storage.String[1] = 0;
		}
		data = parse_expression(p);
		if( data->disposable )
		{
			dispose(data);
			dispose(prompt);
			return Raise(ILLEGAL_DIRECT);
		}
		else
		{//INPUT
			char* p;
			if( fn == -1 ) draw_string( prompt->storage.String);
			if( fn == -1 ) p = get_str();
			else
			{
				char* _i_buffer;
				p = fgets( _i_buffer = (char*)malloc(255) , 255 , file_list[fn].fp);
				if( p == NULL )
				{
					dispose(data);
					dispose(prompt);
					return Raise(FILE_READ);
				}
				p[strlen(p)-1]=0;
			}
			if( data->type == TYPE_STRING )
			{
				if(data->storage.String != init_value[TYPE_STRING].String)
					free(data->storage.String);
				data->storage.String = p;
			}
			if( data->type == TYPE_INTEGER )
			{
				int val;
				sscanf( p , "%d" , &val);
				data->storage.Integer = val;
				free(p);
			}
			if( data->type == TYPE_REAL )
			{
				double val;
				sscanf( p , "%lf" , &val );
				data->storage.Real = val;
				free(p);
			}
		}
		dispose(data);
		dispose(prompt);
		parse_ws(p);
		if( **p == 0 || **p == ':') break;
		if( str_equ( p , "ELSE" )  )
			break;
		if( **p != ',')
	 	{
			return Raise(SYNTAX);
		}
		else (*p)++;
	} 
	return NULL;	
}
Data* parse_LOCATE(char** p)
{
	Data* p1 , *p2;
	p1 = parse_expression(p);
	if( p1->type == TYPE_ERROR)
		return p1;
	if( p1->type == TYPE_STRING)
	{
		dispose(p1);
		return Raise(TYPE_MISMATCH);
	}
	parse_ws(p);
	if( **p != ',' )
	{
		dispose(p1);
		return Raise(SYNTAX);
	}
	else (*p)++;
	parse_ws(p);
	p2=parse_expression(p);
	if( p2->type == TYPE_ERROR)
	{
		dispose(p1);
		return p2;
	}
	if( p2->type == TYPE_STRING )
	{
		dispose(p1);
		dispose(p2);
		return Raise(TYPE_MISMATCH);
	}

	int x,y;
	if( p1->type == TYPE_REAL )
		y = (int)(p1->storage.Real+.001);
	if( p2->type == TYPE_REAL )
		x = (int)(p2->storage.Real+.001);
	dispose(p1);
	dispose(p2);
	if( x < 1 || x > 20 || y < 1 || y > 5)
	{
		return Raise(ILLEGAL_QUANTITY);
	}
	else
	{
		memoryP(0);
		ram[CUR_X]  = x-1;
		ram[CUR_Y]  = y-1;
		memoryV();
	}
	return NULL;
}
Data* parse_GOTO(char** p)
 {
	int line = -1;
	while( **p >= '0' && **p <= '9' )
	{
		if( line == -1 ) line = 0;
		line = line*10 + **p - '0';
		(*p)++;
	}
	if( line >= 0 ) 
	{
		goto_line_no = line;
		start_pos = NULL;
		end_s = NULL;
		return NULL;
	}
	else
		return Raise(SYNTAX);
}
Data* parse_GOSUB(char** p)
 {
	int line = -1;
	while( **p >= '0' && **p <= '9' )
	{ 
		if( line == -1 ) line = 0;
		line = line*10 + **p - '0';
		(*p)++;
	}
	if( line >= 0 ) 
 	{
		if( ps_ptr >= 65535 )
		       return Raise(OUT_OF_MEMORY);	
		prog_stack[ps_ptr++] = line_no;
		parse_ws(p);
		if( **p != ':' || (cur_ends && str_equ(p,cur_ends)) )
 		{
			(*p) += strlen(*p);
		}
		else
			(*p) ++;
		prog_line[ps_ptr-1] = *p;
		prog_ends[ps_ptr-1] = cur_ends;
		goto_line_no = line;
		start_pos = NULL;
		return NULL;
	}
	else
		return Raise(SYNTAX);
}
Data* parse_RETURN(char **p)
{
	if( ps_ptr == 0 )
		return Raise(RETURN_WITHOUT_GOSUB);
	else
		ps_ptr--;
	goto_line_no = prog_stack[ps_ptr];
	start_pos    = prog_line[ps_ptr];
	end_s 	     = prog_ends[ps_ptr];
	return NULL;
}
Data* parse_LET(char** p) /*[LET] var = expr*/
{
	Data* var = parse_aexpr(p);
	Data* val = NULL;
	if( var->type == TYPE_ERROR )
		return var;
	if( var->disposable )
	{
		dispose(var);
		return Raise(SYNTAX);
	}
	parse_ws(p);
	if( **p != '=' )
	{
		dispose(var);
		return Raise(SYNTAX);
	}
	else (*p)++;
	parse_ws(p);
	val = parse_expression(p);
	if( var->type == TYPE_STRING )
	{
		char* buf;
		if( val->type != TYPE_STRING )
		{
			dispose(var);
			dispose(val);
			return Raise(TYPE_MISMATCH);
		}
		buf = (char*)malloc(strlen(val->storage.String)+1);
		strcpy( buf , val->storage.String);
		if( var->storage.String != init_value[TYPE_STRING].String )
			free(var->storage.String);
		var->storage.String = buf;
	}
	if( var->type == TYPE_REAL )
	{
		double v;
		if( val->type == TYPE_STRING )
		{
			dispose(val);
			dispose(var);
			return Raise(TYPE_MISMATCH);
		}
		if( val->type == TYPE_INTEGER )
			v = val->storage.Integer;
		else 
			v= val->storage.Real;
		var->storage.Real = v; 
	}
	if( var->type == TYPE_INTEGER )
	{
		int v;
		if( val->type == TYPE_STRING )
		{
			dispose(val);
			dispose(var);
			return Raise(TYPE_MISMATCH);
		}
		if( val->type == TYPE_REAL )
			v = val->storage.Real;
		else
			v = val->storage.Integer;
		var->storage.Integer = v;

	}
	dispose(val);
	return NULL;
}
Data* parse_DATA(char** p)
{
	while(1)
	{
		Data* ret = parse_expression(p);
		if( ret->type == TYPE_ERROR )
			return ret;
		dispose(ret);
		parse_ws(p);
		if( **p == ':' || **p == 0 || (str_equ( p , "ELSE")  ) )
			break;
		if( **p != ',' )
			return Raise(SYNTAX);
		else
			(*p)++;
	}
	return NULL;
}
Data* parse_READ(char** p)
{
	while(1)
	{
		Data* ret = parse_aexpr(p);
		if( ret->type == TYPE_ERROR )
			return ret;
		if( ret->disposable )
		{
			dispose(ret);
			return Raise(SYNTAX);
		}
		if( data_ptr >= data_count )
		{
			dispose(ret);
			return Raise(OUT_OF_DATA);
		}
		if( ret->type == TYPE_STRING )
		{
			char* buf;
			if( data_store[data_ptr]->type  != TYPE_STRING )
			{
				dispose(ret);
				return Raise(TYPE_MISMATCH);
			}
			buf = (char*)malloc( strlen(data_store[data_ptr]->storage.String) + 1 );
			strcpy( buf , data_store[data_ptr]->storage.String);
			if( ret->storage.String != init_value[TYPE_STRING].String)
				free( ret->storage.String);
			ret->storage.String = buf;
		}
		if( ret->type == TYPE_INTEGER )
		{
			int val;
			if( data_store[data_ptr] -> type == TYPE_STRING )
			{
				dispose(ret);
				return Raise(TYPE_MISMATCH);
			}
			if( data_store[data_ptr]->type == TYPE_INTEGER )
				val = data_store[data_ptr]->storage.Integer;
			else
				val = data_store[data_ptr]->storage.Real;
			ret->storage.Integer = val;				
		}
		if( ret->type == TYPE_REAL )
		{
			double val;
			if( data_store[data_ptr]->type == TYPE_STRING )
			{
				dispose(ret);
				return Raise(TYPE_MISMATCH);
			}
			if( data_store[data_ptr]->type == TYPE_INTEGER )
				val = data_store[data_ptr]->storage.Integer;
			else
				val = data_store[data_ptr]->storage.Real;
			ret->storage.Real = val;
		}
		data_ptr++;
		if( **p == 0 || **p == ':' || ( str_equ(p,"ELSE")  ) )
			break;
		if( **p != ',' )
			return Raise(SYNTAX);
		else
			(*p)++;
	}
	return NULL;
}
Data* parse_line(char**,char*);
Data* parse_IF(char** p)
{
	Data* cond = parse_expression(p);
	int val;
	if( cond->type == TYPE_ERROR )
		return cond;
	if( cond->type == TYPE_STRING )
	{
		dispose(cond);
		return Raise(TYPE_MISMATCH);
	}
	if( cond->type == TYPE_REAL )
		val = (cond->storage.Real!=0);
	else
		val = (cond->storage.Integer!=0);
	dispose(cond);
	parse_ws(p);
	if( !str_equ(p,"THEN") )
		return Raise(SYNTAX);
	(*p)+=4;
	if( val )
	{
		parse_ws(p);
		if( **p >= '0' && **p <= '9' )
			return parse_GOTO(p);
		Data * ret = parse_line(p,"ELSE");
		while(**p)(*p)++;
		return ret;
	}
	else
	{
		int IF_count = 0;
		int flag = 0;
		int first = 1;
		while(**p)
		{
			while( **p && ((!first && **p != ' ' && **p != '\t' && **p != ':') || flag)  )
			{
				if( **p == '"' ) flag = !flag;
				(*p)++;
			}
			first = 0;
			(*p)++;
			parse_ws(p);
			if(!**p) break;
			if(str_equ( p , "IF"))
				IF_count++;
			if(str_equ( p , "ELSE"))
				IF_count--;
			if( IF_count == -1 )
				break;
			//parse_ws(p);
			
		}
		if( IF_count != -1 )
			return NULL;
		else
		{
			(*p)+=5;
			if( **p >= '0' && **p <= '9' )
				return parse_GOTO(p);
			return parse_line(p,NULL);
		}
	}
}
Data* parse_BOX(char** p)
{
	int parm[6];
	int i;
	for(i = 0 ; i < 6 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i < 4)
	       return Raise(ILLEGAL_QUANTITY);
	if( i < 5 )
		parm[4] = 0;
	if( i < 6 )
		parm[5] = 1;
	rect( parm[0],parm[1],parm[2]-parm[0],parm[3]-parm[1],parm[4],parm[5]);
	return NULL;	
}
Data* parse_CIRCLE(char** p)
{
	int parm[5];
	int i;
	for(i = 0 ; i < 5 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i < 3)
	       return Raise(ILLEGAL_QUANTITY);
	if( i < 4 )
		parm[3] = 0;
	if( i < 5 )
		parm[4] = 1;
	ellipse( parm[0],parm[1],parm[2],parm[2],parm[3],parm[4]);
	return NULL;	
}
Data* parse_ELLIPSE(char** p)
{
	int parm[6];
	int i;
	for(i = 0 ; i < 6 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i < 4)
	       return Raise(ILLEGAL_QUANTITY);
	if( i < 5 )
		parm[4] = 0;
	if( i < 6 )
		parm[5] = 1;
	ellipse( parm[0],parm[1],parm[2],parm[3],parm[4],parm[5]);
	return NULL;	
}
Data* parse_LINE(char** p)
{
	int parm[5];
	int i;
	for(i = 0 ; i < 5 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i < 4)
	       return Raise(ILLEGAL_QUANTITY);
	if( i < 5 )
		parm[4] = 1;
	line( parm[0],parm[1],parm[2]-parm[0],parm[3]-parm[1],parm[4]);
	return NULL;	
}
Data* parse_DRAW(char** p)
{
	int parm[3];
	int i;
	for(i = 0 ; i < 3 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i < 2)
	       return Raise(ILLEGAL_QUANTITY);
	if( i < 3 )
		parm[2] = 1;
	pixel( parm[0],parm[1],parm[2]);
	return NULL;	
}
Data* parse_SWAP( char** p )
{
	Data* l , *r;
	l = parse_expression(p);
	if( l->type == TYPE_ERROR )
		return l;
	if( l->disposable )
	{
		dispose(l);
		return Raise(ILLEGAL_QUANTITY);
	}
	if( **p != ',' )
	{
		dispose(l);
		return Raise(SYNTAX);
	}
	else (*p)++;
	r = parse_expression(p);
	if( r->type == TYPE_ERROR )
	{
		dispose(l);
		return r;
	}
	if( r->disposable )
	{
		dispose(l);
		dispose(r);
		return Raise(ILLEGAL_QUANTITY);
	}
	if( r->type != l->type )
	{
		dispose(l);
		dispose(r);
		return Raise(TYPE_MISMATCH);
	}
	Storage temp = l->storage;
	l->storage=r->storage;
	r->storage=temp;
	return NULL;
} 
Data* parse_DIM(char** p)
{
	while(**p)
	{
		char* id = parse_id(p);
		if( id == NULL || !strlen(id) ) return NULL;
		parse_ws(p);
		if( **p == '(' ) /*an array declearation*/
		{
			int i;
			Subscript* sub = parse_arg(p);
			if( sub == NULL ) 
				return Raise(SYNTAX);
			if( sub->dimensions == 0 )
			{
				dispose_arg(sub);
				return Raise(SYNTAX);	
			}
			if( sub->dimension[sub->dimensions-1]->type == TYPE_ERROR )
			{
				Data* ret = sub->dimension[sub->dimensions-1];
				sub->dimensions--;
				dispose_arg(sub);
				return ret;			
			}
			for( i = 0 ; i < sub->dimensions ; i ++ )
				if( sub->dimension[i]->type == TYPE_STRING )
				{
					dispose_arg(sub);
					return Raise(ILLEGAL_QUANTITY);
				}
			reg_arr( id , sub );
			dispose_arg(sub);
		}
		else /*variable*/
			reg_var(id);
		if( **p != ',' ) break;
		else (*p)++;
	}
	return NULL;
}
Data* parse_POKE(char** p)
{
	int parm[3];
	int i;
	for(i = 0 ; i < 3 ; i ++ )
	{
		Data* ret = parse_expression(p);
		if(ret->type == TYPE_ERROR)
			return ret;
		if(ret->type == TYPE_STRING)
		{
			dispose(ret);
			return Raise(TYPE_MISMATCH);
		}
		if(ret->type == TYPE_REAL )
			parm[i] = (int)(ret->storage.Real+.01);
		else
			parm[i] = ret->storage.Integer;
		parse_ws(p);
		if(**p!=',')
			break;
		else
			(*p)++;
		dispose(ret);
	}
	i++;
	if( i != 2 )
 		return Raise(ILLEGAL_QUANTITY);
	*memoryP(parm[0])=parm[1];
	memoryV();
	return NULL;
}
Data* parse_FOR(char** p)
{
	if(for_ptr>65535) return Raise(OUT_OF_MEMORY);
	Data* var = parse_aexpr( p );
	Data* ini;
	Data* end;
	if( var->type == TYPE_ERROR )
		return var;
	if( var->type == TYPE_STRING)
	{
		dispose(var);
		return Raise(TYPE_MISMATCH);
	}
	if( var->disposable )
	{
		dispose(var);
		return Raise(SYNTAX);
	}
	parse_ws(p);
	if(**p!='=')
	{
		dispose(var);
		return Raise(SYNTAX);
	}
	else
		(*p)++;
	ini = parse_expression(p);
	if( var->type == TYPE_ERROR )
	{
		dispose(var);
		return ini;
	}
	if( var->type == TYPE_STRING )
	{
		dispose(var);
		dispose(ini);
		return Raise(TYPE_MISMATCH);
	}
	parse_ws(p);
	if( !str_equ(p,"TO") )
	{
		dispose(var);
		dispose(ini);
		return Raise(SYNTAX);
	}
	else
		(*p)+=2;
	parse_ws(p);
	end = parse_expression(p);
	if( end->type == TYPE_ERROR )
	{
		dispose(var);
		dispose(ini);
		return end;
	}
	if( end->type == TYPE_STRING )
	{
		dispose(var);
		dispose(ini);
		dispose(end);
		return Raise(TYPE_MISMATCH);
	}
	parse_ws(p);
	if(str_equ(p,"STEP"))
	{
		(*p)+=4;
		parse_ws(p);
		Data* inc = parse_expression(p);
		if( inc->type == TYPE_ERROR )
		{
			dispose(var);
			dispose(ini);
			dispose(end);
			return inc;
		}
		if( inc->type == TYPE_STRING )
		{
			dispose(var);
			dispose(ini);
			dispose(end);
			dispose(inc);
			return Raise(TYPE_MISMATCH);
		}
		for_list[for_ptr].increment = inc;
		if( inc->type == TYPE_INTEGER )
			inc->storage.Real = inc->storage.Integer;
	}
	else
		for_list[for_ptr].increment = NULL;
	for_list[for_ptr].ctl_var = var;
	double val;
	if( ini->type == TYPE_INTEGER )
		val = ini->storage.Integer;
	else
		val = ini->storage.Real;
	if( var->type == TYPE_INTEGER )
		var->storage.Integer = val;
	else
		var->storage.Real = val;
	for_list[for_ptr].endv = end;
        for_list[for_ptr].line_no = line_no;
	parse_ws(p);
	char* tp = *p;
	if( *tp != ':' || (cur_ends && str_equ(&tp,cur_ends)) )
 	{
		(tp) += strlen(tp);
	}
	else
		(tp) ++;
	for_list[for_ptr].start_p = tp;
	for_list[for_ptr].end_s = cur_ends;
	for_ptr++;
	dispose(ini);
	return NULL;
} 
Data* parse_NEXT(char** p)
{
	if( for_ptr == 0 )
		return Raise(NEXT_WITHOUT_FOR);
	For* cur = for_list + for_ptr - 1;
	Data* var = cur->ctl_var;
	double val;
	double end;
	double inc;
	if(**p==0 || **p == ':' || str_equ(p,"ELSE"));
	else dispose(parse_expression(p));
	if( var->type == TYPE_INTEGER )
		val = var->storage.Integer;
	else
		val = var->storage.Real; 
	if( cur->endv->type == TYPE_INTEGER )
		end = cur->endv->storage.Integer;
	else
		end = cur->endv->storage.Real;

	if( cur->increment )
		val += (inc = cur->increment->storage.Real);
	else
		val += (inc = 1);
	if( var->type == TYPE_INTEGER )
		var->storage.Integer = val;
	else
		var->storage.Real = val;
	if( !(( val > end && inc > 0 ) || ( val < end && inc < 0 )) )
	{
		goto_line_no = cur->line_no;
		start_pos = cur->start_p;
		end_s = cur->end_s;
	}
	else
	{
		dispose(cur->endv);
		dispose(cur->ctl_var);
		dispose(cur->increment);
		for_ptr--;
	}
	return NULL;
}
Data* parse_ON(char** p)
{
	Data* val = parse_expression(p);
	int v;
	int jmp_list[1000];
	int jmp_c=0;
	if( val->type == TYPE_ERROR )
		return val;
	if( val->type == TYPE_STRING )
	{
		dispose(val);
		return Raise(TYPE_MISMATCH);
	}
	if( val->type == TYPE_REAL )
		v = (int)(val->storage.Real+.001);
	else
		v = val->storage.Integer;
	dispose(val);
	parse_ws(p);
	if( !str_equ(p,"GOTO") )
		return Raise(SYNTAX);
	else
		(*p)+=4;
	parse_ws(p);
	while(1)
	{
		jmp_list[jmp_c] = 0;
		while( **p>='0' && **p<='9' )
		{
			jmp_list[jmp_c] = jmp_list[jmp_c] * 10 + **p - '0';
			(*p)++;
		}
		jmp_c++;
		parse_ws(p);
		if( **p == 0 || **p == ':' || str_equ(p,"ELSE") )
			break;
		if( **p != ',' )
			return Raise(SYNTAX);
		else
			(*p)++;
		parse_ws(p);
	}
	if( v > jmp_c )
		return Raise(ILLEGAL_QUANTITY);
	goto_line_no = jmp_list[v-1];
	start_pos = NULL;
	end_s = NULL;
	return NULL;
}
Data* parse_OPEN( char** p )
{
	int mode = FILE_MODE_INPUT;
	int len  = 32;
	int fn   = -1;
	int ret;
	Data* file_name = parse_expression(p);
	if( file_name->type == TYPE_ERROR )
		return file_name;
	if( file_name->type != TYPE_STRING )
	{
		dispose(file_name);
		return Raise(TYPE_MISMATCH);
	}
	parse_ws(p);
	if( ret = str_equ( p , "FOR" ) )
	{
		(*p)+=ret;
		parse_ws(p);
		if( ret = str_equ( p , "INPUT" ) )
			mode = FILE_MODE_INPUT;
		else if( ret = str_equ( p , "OUTPUT" ))
			mode = FILE_MODE_OUTPUT;
		else if( ret = str_equ( p , "APPEND"))
			mode = FILE_MODE_APPEND;
		else if( ret = str_equ( p , "RANDOM"))
			mode = FILE_MODE_RANDOM;
		else
		{
			dispose(file_name);
			return Raise(SYNTAX);
		}
		(*p)+=ret;
	}
	parse_ws(p);
	if( p[0][0] == 'A' && p[0][1] == 'S' )
	{
		(*p)+=2;
		parse_ws(p);
		if( **p == '#' )
			(*p)++;
		Data* temp = parse_expression(p);
		if( temp->type == TYPE_ERROR )
		{
			dispose(file_name);
			return temp;
		}
		if( temp->type == TYPE_STRING )
		{
			dispose(file_name);
			dispose(temp);
			return Raise(TYPE_MISMATCH);
		}
		if( temp->type == TYPE_REAL )
			fn = (int)(temp->storage.Real+.001);
		else if( temp->type == TYPE_INTEGER )
			fn = temp->storage.Integer;
		dispose(temp);
	}
	else
	{
		dispose(file_name);
		return Raise(SYNTAX);
	}
	parse_ws(p);
	if( p[0][0] == 'L' && p[0][1] == 'E' && p[0][2] == 'N' )
	{
		(*p)+=3;
		parse_ws(p);
		if( p[0][0] != '=' )
		{
			dispose(file_name);
			return Raise(SYNTAX);
		}
		Data* temp = parse_expression(p);
		if( temp->type == TYPE_ERROR )
		{
			dispose(file_name);
			return temp;
		}
		if( temp->type == TYPE_STRING )
		{
			dispose(file_name);
			dispose(temp);
			return Raise(TYPE_MISMATCH);
		}
		if( temp->type == TYPE_REAL )
			len = (int)(temp->storage.Real+.001);
		else if( temp->type == TYPE_INTEGER )
			len = temp->storage.Integer;
		dispose(temp);
	}
	ret = open_file( file_name->storage.String , mode , len , fn );
	dispose(file_name);
	if( ret != 0 )
		return Raise(ret);
	return NULL;
}
Data* parse_CLOSE( char** p )
{
	int fn;
	if( **p == '#' )
		(*p)++;
	Data* ret = parse_expression(p);
	if( ret->type == TYPE_ERROR )
		return ret;
	if( ret->type == TYPE_STRING )
	{
		dispose(ret);
		return Raise(TYPE_MISMATCH);
	}
	if( ret->type == TYPE_REAL )
		fn = (int)(ret->storage.Real + .001);
	if( ret->type == TYPE_INTEGER )
		fn = ret->storage.Integer;
	int res = close_file(fn);
	if( res )
		return Raise(res);
	return NULL;
}
Data* parse_statement( char** p)
{ 
	int len;
	parse_ws(p);
	if( len = str_equ(p,"PRINT") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_PRINT(p);
	}
	if( len = str_equ(p,"END") )
	{
		(*p)+=len;
		parse_ws(p);
		return Raise(SUCESS);
	}
	if( len = str_equ(p,"CLS") )
	{
		int i;
		(*p)+=len;
		parse_ws(p);
		memoryP(0);
		for( i = DISP_BUF ; i < DISP_BUF + DISP_H * DISP_W ; i ++ )
			ram[i] = 0;
		for( i = TEXT_BUF ; i < TEXT_BUF + TEXT_W * TEXT_H ; i ++ )
			ram[i] = 0;
		ram[CUR_X] = 0;
		ram[CUR_Y] = 0;	
		memoryV();
		return NULL;
	}
	if( len = str_equ(p,"GRAPH") )
	{
		(*p)+=len;
		parse_ws(p);
		*memoryP(CUR_S) = 2;
		memoryV();
		return NULL;
	}
	if( len = str_equ(p,"TEXT") )
	{
		(*p)+=len;
		parse_ws(p);
		*memoryP(CUR_S) =0;
		memoryV();
		return NULL;
	}
	if( len = str_equ(p,"INPUT") )
	{
		(*p)+=len;
		parse_ws(p);
		Data* ret = parse_INPUT(p);
		return ret;
	}
	if( len = str_equ(p,"LOCATE") )
	{
		(*p)+=len;
		parse_ws(p);
		Data* ret  = parse_LOCATE(p);
		return ret;
 	}
	if( len = str_equ(p,"GOTO") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_GOTO(p);
	}
	if( len = str_equ(p,"DATA") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_DATA(p); /*skip*/
	}	
	if( len = str_equ(p,"LET") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_LET(p);
	}
	if( len = str_equ(p,"READ") )
	{
		(*p)+=len;
		parse_ws(p);
		Data* ret = parse_READ(p);
		return ret;
	}
	if( len = str_equ(p,"RESTORE"))
	{
		(*p)+=len;
		parse_ws(p);
		data_ptr = 0;
		return NULL;
	}
	if( len = str_equ(p,"IF") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_IF(p);
	}
	if( len = str_equ(p,"BOX"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_BOX(p);
	}
	if( len = str_equ(p,"CIRCLE"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_CIRCLE(p);
	}
	if( len = str_equ(p,"ELLIPSE"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_ELLIPSE(p);
	}
	if( len = str_equ(p,"LINE"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_LINE(p);
	}
	if( len = str_equ(p,"DRAW"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_DRAW(p);
	}
	if( len = str_equ(p,"REM"))
	{
		(*p)+=len;
		parse_ws(p);
		while(**p) (*p)++;
		return NULL;
	}
	if( len = str_equ(p,"SWAP"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_SWAP(p);
	}
	if( len = str_equ(p,"DIM"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_DIM(p);
	}
	if( len = str_equ(p,"POKE"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_POKE(p);
	}
	if( len = str_equ(p,"GOSUB"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_GOSUB(p);
	}
	if( len = str_equ(p,"RETURN"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_RETURN(p);
	}
	if( len = str_equ(p,"FOR"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_FOR(p);
	}
	if( len = str_equ(p,"NEXT"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_NEXT(p);
	}
	if( len = str_equ(p,"CLEAR"))
	{
		(*p)+=len;
		parse_ws(p);

		return NULL;
	}
	if( len = str_equ(p,"__DELAY"))
	{
		(*p)+=len;
		parse_ws(p);
		delay = -1;
		while( **p >= '0' && **p <= '9' )
		{
			if( delay == -1 ) delay = 0;
			delay = delay*10 + **p - '0';
			(*p)++;
		}
		return NULL;
	}
	if( len = str_equ(p,"__DEBUG") )
	{
		(*p)+=len;
		parse_ws(p);
		debug = 1;
		Data* ret = parse_PRINT(p);
		debug = 0;
		return ret;
	}
	if( len = str_equ(p,"__EXECUTE") )
	{
		(*p)+=len;
		parse_ws(p);
		Data* line = parse_expression(p);
		if( line->type == TYPE_ERROR )
			return line;
		if( line->type != TYPE_STRING )
		{
			dispose(line);
			return Raise(TYPE_MISMATCH);
		}
		char* tp = line->storage.String;
		if( tp == NULL || strlen(tp) == 0 ) 
			return NULL;
		parse_line(&tp,"NULL");
		return NULL;
	}
	if( len = str_equ(p,"__RESET_ERR") )
	{
		(*p)+=len;
		parse_ws(p);
		parse_expression(p);
		Last_Error.id = 0;
		return NULL;
	}
	if( len = str_equ(p,"ON") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_ON(p);
	}
	if( len = str_equ(p,"OPEN") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_OPEN(p);
	}
	if( len = str_equ(p,"CLOSE"))
	{
		(*p)+=len;
		parse_ws(p);
		return parse_CLOSE(p);
	}
	if( len = str_equ(p,"WRITE") )
	{
		(*p)+=len;
		parse_ws(p);
		return parse_PRINT(p);
	}
	/*TODO: FIELD GET PUT */
	return parse_LET(p);
} 
Data* parse_line(char** p , char* end_sign)
{
       cur_ends = end_sign;	
	while(1)
	{
		Data* data = parse_statement(p);
		parse_ws(p);
		if( data )
			return data;
		if(goto_line_no!=-1) /*a jump requst*/
			return NULL; /*abort the parse and back trace to handle the request*/
		if(**p == 0 )  break;
		if(**p == ':')	/*there is anther statement*/
			(*p)++;	/*parse it*/
		else	      	/*no following statement*/
		{	      	/*an end_sign expected*/
			int len;
			if( end_sign && ( len = str_equ( p , end_sign ) ) )
			{
				(*p) += len;
			        parse_ws(p);
				break;	
			}
			else
				return Raise(SYNTAX);
		}
	}
	cur_ends = NULL;
	return NULL;
}
Data* parse_data(char** program)
{
	for( line_no = 0 ; line_no < 65536 ; line_no ++ )
		if( program[line_no] != NULL )
		{
			char* p = program[line_no];
			int bos = 1;
			int in_str = 0;
			for(;*p;p++)
			{
				if( p[0] == '"' )
					in_str = !in_str;
				if( p[0] == 'D' && p[1] == 'A' && p[2] == 'T' && p[3] == 'A' && bos & !in_str )
				{
					int first = 1;
					p+=4;
					for(;*p;)
					{
						parse_ws(&p);
						if( *p == 0 || (*p != ',' && !first )) break;
						if(!first) p++;
						parse_ws(&p);
						if( (data_store[data_count++] = parse_expression(&p))->type == TYPE_ERROR)
							return data_store[data_count-1];
						//printf("%d\n",data_store[data_count-1]->type);
						first = 0;
					}
				}
				if( *p == ':')
					bos = 1;
				else if( *p != ' ' && *p != '\t' )
					bos = 0;
				if(!*p) break;
			}
		}
	return NULL;
}
int line_count;
void  output_error(Data* error)
{
	char buf[1024];
	if(error->storage.error.id==0)
	{
		printf("Program terminated normally ,%d lines executed\n",line_count);
		return;
	}
	sprintf( buf , "\r?%s ERROR IN %d\r" , err_str[error->storage.error.id] , error->storage.error.line_no );
	draw_string(buf);
}
Data* parse_program(char **program) 
{
/*	eg: a program	
		1 PRINT "HELLO WORLD"
		2 END
	should be stored in following form:  
		program = { NULL , "PRINT ... " , "END" , ...}
*/
	Data* ret;
	if( ret = parse_data(program) )
	{
		output_error(ret);
		return ret;
	}
	line_no = 0;
	char *p = NULL ,*es = NULL;
	while(1)
	{
		Data* ret;
		while(!program[line_no] && line_no < 65536) line_no++;
		if( line_no >= 65536 ) return Raise(SUCESS);
		line_count++;
		if( !p )p = program[line_no];
		parse_ws(&p);
		//printf("%d\n",line_no);
		if( *p && (ret = parse_line(&p,es)) != NULL )
		{/*error occured*/
			
			//if( ret->storage.error.id != SUCESS ) output_error(ret);
			return ret;
		}
		es = NULL;
		p = NULL;
		if(goto_line_no!=-1)
		{/*a goto request*/
			if( !program[goto_line_no] ) 
				return Raise(UNDEF_STATEMENT);
			line_no = goto_line_no;
			p = start_pos;
			es = end_s;
			goto_line_no = -1;

		}
		else
			line_no++;
		if( delay )
			usleep(delay);
	}
	return NULL;
}
#ifdef STATEMENT_TEST
char* test[65536];
char *p;
//FILE* fp = fopen("test.bas","r");
int main()
{
	init();
	int i = 0;
	/*test[0] = "INPUT A,B";
	test[2] = "PRINT A";
	test[3] = "IF A=B THEN GOTO 0 ELSE PRINT A;B";
	test[4] = "END";*/
	while( !feof(stdin) )
	{
		int line = 0;
		char* temp = (char*)malloc(1024);
		gets(temp);
		int flag = 0;
		for( p = temp ; *p ; p ++ , flag = 1 )
			if( *p >= '0' && *p <= '9' )
				line = line * 10 + *p - '0';
			else
				break;
		parse_ws(&p);
		if(flag) test[line] = p;
	}
	output_error(parse_program(test));
	return 0;
}
#endif
