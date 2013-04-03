/******************************************
	token.c: implementation of Variable , function & array
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#ifndef __HEADER_TOKEN
#define __HEADER_TOKEN
#include "common.h"


#ifdef  __TOKEN_IMP
#define TOKEN_VARIABLE
#else
#define TOKEN_VARIABLE extern
#endif 

#define SUCESS 			0
#define NEXT_WITHOUT_FOR 	1
#define SYNTAX 			2
#define OUT_OF_DATA 		3
#define ILLEGAL_QUANTITY 	4
#define OVERFLOW 		5
#define OUT_OF_MEMORY 		6
#define UNDEF_STATEMENT 	7
#define BAD_SUBSCRIPT 		8
#define REDIMED_ARRAY 		9
#define DIV_BY_ZERO 		10
#define ILLEGAL_DIRECT 		11
#define TYPE_MISMATCH 		12
#define STRING_TOO_LONG 	13
#define FORMULA_TOO_COMPLEX 	14
#define CANT_CONT 		15
#define UNDEF_FUNCTION 		16
#define WEND_WITHOUT_WHILE 	17
#define RETURN_WITHOUT_GOSUB 	18
#define FILE_NUMBER		19
#define FILE_MODE		20
#define FILE_NOT_EXIST		21
#define FILE_READ		22
#define FILE_WRITE		23
#define FILE_CLOSE		24
#define FILE_OPEN		25
TOKEN_VARIABLE char* err_str[] 
#ifdef __TOKEN_IMP
={
	"SUCCESS",
	"NEXT WITHOUT FOR",
	"SYNTAX",
	"OUT OF DATA",
	"ILLEGAL QUANTITY",
	"OVERFLOW",
	"OUT OF MEMROY",
	"UNDEFIN'D STATEMENT",
	"BAD SUBSCRIPT",
	"REDIM'D ARRAY",
	"DIV'D BY ZERO",
	"ILLEGAL DIRECT",
	"TYPE MISMATCH",
	"STRING TOO LONG",
	"FORMULA TOO COMPLEX",
	"CAN'T CONT",
	"UNDEFIN'D FUNCTION",
	"WEND WITHOUT WHILE",
	"RETURN WITHOUT GOSUB",
	"FILE NUMBER",
	"FILE MODE",
	"FILE NOT EXIST",
	"FILE READ",
	"FILE WRITE",
	"FILE CLOSE",
	"FILE OPEN"
}
#endif
;

typedef struct 
{
	int id;
	int line_no;
} Error;
#define TYPE_STRING  0
#define TYPE_REAL    1
#define TYPE_INTEGER 2
#define TYPE_ERROR   3
typedef union
{
	char* String;
	double Real;
	short int Integer;
	Error error;
} Storage;
TOKEN_VARIABLE Storage init_value[4];
typedef struct
{
	int type;
	int disposable;
	Storage storage;
} Data;

typedef struct
{
	char name[32];
	Data* data; 
} Variable;
typedef struct
{
	int dimensions;
	Data* dimension[256];
} Subscript;
typedef struct
{
	int type;
	Data** storage;
	Subscript dim;
} Array;

typedef struct
{
	char name[32];
	int is_function; /* false , when it is a array */
	union
	{
		Array arr;
		Data* (*foo)( Subscript* arg );
	} f;
} Function;



TOKEN_VARIABLE Error Last_Error;
TOKEN_VARIABLE int line_no;

Error Error_new(int id );
Data* Data_new( int type , Storage data , int disposable);

TOKEN_VARIABLE Variable varlist[65536];
TOKEN_VARIABLE Function funlist[65536];
TOKEN_VARIABLE int var_count , fun_count;


Error	  reg_var( char* id );  /*register a new variable*/ 
Data*     get_var( char* id );  /*get variable . if it's not exist , register it */

Error     reg_foo( char* id , Data* (*foo)( Subscript* arg ) );
Error	  reg_arr( char* id , Subscript * size );
Function* get_fun( char* id );
Data*	  get_arr_element( Array* arr , Subscript* sub );

int 	  get_id_type( char *id );

Data* temp_var( Data* var );
#define Raise(error) Data_new(TYPE_ERROR,(Storage)Error_new(error),1)
#endif
