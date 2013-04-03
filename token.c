/******************************************
	token.c: implementation of Variable , function & array
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __TOKEN_IMP
#include "token.h"
Error Error_new(int id )
{
	Error ret = { id , line_no };
	Last_Error = ret;
	return ret;
}
Data* Data_new( int type , Storage data , int disposable)
{
	Data* ret = (Data*)malloc( sizeof(Data) );
	ret->type = type;
	ret->storage = data;
	ret->disposable = disposable;
	return ret;
}
int get_id_type( char * id )
{
	int len = strlen(id);
	if( id[len-1] == '%' )
		return TYPE_INTEGER;
	if( id[len-1] == '$' )
		return TYPE_STRING;
	return TYPE_REAL;
}
Error reg_var( char* id )
{
	int i = 0 ;
	for( i = 0 ; i < var_count ; i ++ )
		if( strcmp( id , varlist[i].name ) == 0 )
			return Error_new(SUCESS);
	if( var_count > 65535 )
		return Error_new(OUT_OF_MEMORY); /*throw an out-of-memory error*/
	
	strcpy( varlist[var_count++].name , id );
	
	return Error_new(SUCESS);			
}
Data* get_var( char* id )
{
	int i = 0 , type = get_id_type(id);
	for( i = 0 ; i < var_count ; i ++ )
		if( strcmp( id , varlist[i].name ) == 0 )
		{
			if( varlist[i].data == NULL )
				varlist[i].data = Data_new( type , init_value[type] , 0);
			return varlist[i].data;
		}
	if( reg_var( id ).id  == 0 ) /*register a new variable */
	{
		if( varlist[var_count-1].data == NULL )
			varlist[var_count-1].data = Data_new( type , init_value[type] , 0);
		return varlist[var_count - 1].data;
	}
	else 
		return NULL;
}

Error reg_foo( char* id , Data* (*foo)( Subscript* arg ) )
{
	int i = 0 ;
	for( i = 0 ; i < fun_count ; i ++ )
		if( strcmp( id , funlist[i].name ) == 0 )
			return Error_new(SUCESS);
	if( fun_count > 65535 )
		return Error_new(OUT_OF_MEMORY); /*throw an out-of-memory error*/
	strcpy( funlist[fun_count++].name , id );
	funlist[fun_count-1].is_function = 1;
	funlist[fun_count-1].f.foo = foo;
	return Error_new(SUCESS);	
}

Error reg_arr( char* id , Subscript * size )
{
	int i = 0 ;
	for( i = 0 ; i < fun_count ; i ++ )
		if( strcmp( id , funlist[i].name ) == 0 )
			return Error_new(REDIMED_ARRAY);
	if( fun_count > 65535 )
		return Error_new(OUT_OF_MEMORY); /*throw an out-of-memory error*/
	strcpy( funlist[fun_count++].name , id );
	funlist[fun_count-1].is_function = 0;
	funlist[fun_count-1].f.arr.type = get_id_type(id);
	if( size )
	{
		int sz = 1;
		int i;
		funlist[fun_count-1].f.arr.dim.dimensions = size->dimensions;
		for( i = 0 ; i < size->dimensions ; i ++ )
		{
			Data* cur_sub;
			if( size->dimension[i]->type == TYPE_REAL )
				cur_sub = Data_new(TYPE_REAL,(Storage)(short)(size->dimension[i]->storage.Real+.001),1);
			else
				cur_sub = Data_new(TYPE_REAL,(Storage)size->dimension[i]->storage.Integer,1);
			sz *= (cur_sub->storage.Integer+1);
			funlist[fun_count-1].f.arr.dim.dimension[i] = cur_sub;
		}
		funlist[fun_count-1].f.arr.storage = (Data**)malloc( sz * sizeof(Data*) );
		for( i = 0 ; i < sz ; i ++ )
		{
			funlist[fun_count-1].f.arr.storage[i] = NULL;
		}	
	}
	else
	{
		funlist[fun_count-1].f.arr.dim.dimensions = 1;
		funlist[fun_count-1].f.arr.dim.dimension[0] = Data_new( TYPE_INTEGER , (Storage)(short)10 , 0);
		funlist[fun_count-1].f.arr.storage = (Data**)malloc( 11 * sizeof(Data*) );
		for( i = 0 ; i < 11 ; i ++ )
		{
			funlist[fun_count-1].f.arr.storage[i] = NULL;
		}
	}	
	return Error_new(SUCESS);	
}
Function* get_fun( char* id )
{
	int i = 0 ;
	for( i = 0 ; i < fun_count ; i ++ )
		if( strcmp( id , funlist[i].name ) == 0 )
			return funlist + i;
	if( reg_arr( id , NULL ).id  == 0 ) /*register a new variable */
		return funlist + fun_count - 1;
	else 
		return NULL;
}
Data* get_arr_element( Array* arr , Subscript* sub )
{
	int offset = 0, i , type = arr->type;
	if( arr->dim.dimensions != sub->dimensions )
		return Raise(BAD_SUBSCRIPT);
	for( i = 0 ; i < sub->dimensions ; i ++ )
		if( sub->dimension[i]->type == TYPE_INTEGER ||
		    sub->dimension[i]->type == TYPE_REAL )
		{
			int val;
			if( sub->dimension[i]->type == TYPE_INTEGER )
				val = sub->dimension[i]->storage.Integer;
			else
				val = (int)sub->dimension[i]->storage.Real;
			if( val > arr->dim.dimension[i]->storage.Integer )
				return Data_new( TYPE_ERROR , (Storage)Error_new(ILLEGAL_QUANTITY) , 1);
			offset = offset * arr->dim.dimension[i]->storage.Integer + val;		
		}
		else
			return Raise(TYPE_MISMATCH);
	
	if( arr->storage[ offset] == NULL )
		arr->storage[ offset ] = Data_new( type , init_value[type] , 0); 
	return arr->storage[ offset ];
}
void token_init()
{
	init_value[TYPE_STRING] = (Storage)"";
	init_value[TYPE_REAL] = (Storage)0.0;
	init_value[TYPE_INTEGER] = (Storage)(short)0;
	init_value[TYPE_ERROR] = (Storage)Error_new(SUCESS);
}
Data* temp_var( Data* var )
{
	if(var)
	{
		if( var->disposable ) return var;
		Data* ret = (Data*)malloc(sizeof(Data));
		ret->type = var->type;
		ret->disposable = 1;
		if( ret->type == TYPE_STRING )
		{
			ret->storage.String = (char*)malloc(strlen(var->storage.String)+1);
			strcpy( ret->storage.String , var->storage.String );
		}
		else
			ret->storage = var->storage;
		return ret;
	}
	else
		return NULL;
}
#ifdef TOKEN_TEST
#include "init.h"
#include "io.h"
#include "memory.h"
int main()
{
	uchar ch;
	init();
	*memoryP( CUR_S ) = 0;
	memoryV();
	while( (ch = inkey())!= 'q' )
	{
		Subscript arg = { 2 , Data_new(TYPE_REAL , (Storage)4.0)};
		printf("%s\n", err_str[get_arr_element( &(get_fun("A")->f.arr) , &arg)->storage.error.id] );
		//rect( 0 , 0 , 160 , 80 , 1 , 1 );
		put_char(ch);
	}
	//sleep(10);
	return 0;
}
#endif
