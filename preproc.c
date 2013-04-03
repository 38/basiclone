#include "preproc.h"
#include "codeset.h"
#include "common.h"
#include "expression.h"
char* text[128] = 
{
	"END",
	"FOR",
	"NEXT",
	"DATA",
	"INPUT",
	"DEL",
	"DIM",
	"READ",
	"SWAP",
	"TRACE",
	"NOTRACE",
	"POP",
	"LET",
	"GOTO",
	"RUN",
	"IF",
	"RESTORE",
	"GOSUB",
	"RETURN",
	"REM",
	"STOP",
	"ON",
	"DEF",
	"POKE",
	"PRINT",
	"CONT",
	"LIST",
	"CLEAR",
	"NEW",
	"TEXT",
	"GRAPH",
	"SYSTEM",
	"NORMAL",
	"INVERSE",
	"FLASH",
	"PLAY",
	"BEEP",
	"INKEY$",
	"LOAD",
	"SAVE",
	"KILL",
	"FILES",
	"OPEN",
	"CLOSE",
	"WRITE",
	"FIELD",
	"GET",
	"PUT",
	"LSET",
	"RSET",
	"AUTO",
	"LOCATE",
	"DRAW",
	"LINE",
	"BOX",
	"CIRCLE",
	"ELLIPSE",
	"CLS",
	"EDIT",
	"WHILE",
	"WEND",
	"CALL",
	"RENAME",
	"COPY",
	"TAB",
	"TO",
	"FN",
	"SPC",
	"THEN",
	"ELSE",
	"AT",
	"NOT",
	"STEP",
	"+",
	"-",
	"*",
	"/",
	"^",
	"AND",
	"OR",
	">",
	"=",
	"<",
	"SGN",
	"INT",
	"ABS",
	"POS",
	"SQR",
	"RND",
	"LOG",
	"EXP",
	"COS",
	"SIN",
	"TAN",
	"ATN",
	"PEEK",
	"LEN",
	"STR$",
	"VAL",
	"ASC",
	"MKS$",
	"MKI$",
	"CVS$",
	"CVI$",
	"LOF",
	"EOF",
	"CHR$",
	"LEFT$",
	"RIGHT$",
	"MID$",
};
typedef struct
{
	unsigned short next_line_no;
	unsigned short this_line_no;
} line_header;
int get_source_format(FILE* fp)
{
	int ret = -1;
	if(fp == NULL ) return SOURCE_FORMAT_UNR;
	if(fp == stdin ) return SOURCE_FORMAT_TXT;
	fseek( fp , 0 , SEEK_SET );
	if( fgetc(fp) == 0 )
		ret = SOURCE_FORMAT_BAS;
	else
		ret = SOURCE_FORMAT_TXT;
	if( ret == SOURCE_FORMAT_TXT ) fseek( fp , 0 , SEEK_SET );
	return ret;
}
char buf[1024];
char* get_line(FILE* fp , int format ,int* line)
{
	char* p;
	char* temp = (char*)malloc(1024);
	int flag = 0;
	*line = 0;
	if( fp == NULL || feof(fp) )
	{
		free(temp);
		return NULL;
	}
	if( format == SOURCE_FORMAT_TXT )
	{
		fgets(buf,1024,fp);
		code_convert("utf8","gbk",buf,strlen(buf),temp,1024);
		if(temp[0] == '#')
		{
			free(temp);
			return get_line( fp , format , line );
		}
		while( temp[strlen(temp)-1] == '\n' || temp[strlen(temp)-1] == '\r' )
			temp[strlen(temp)-1] = 0;
		for( p = temp ; *p ; p ++ , flag = 1 )
			if( *p >= '0' && *p <= '9' )
				(*line) = (*line) * 10 + *p - '0';
			else
				break;
		parse_ws(&p);
	}
	else
	{
		int  cf = 0;
		line_header h;
		fread( &h , 1 , sizeof(line_header) , fp );
		if( h.next_line_no == 0 )
		{
			free(temp);
			return NULL;
		}
		h.next_line_no -= 0x7000;
		//printf("%d %d\n", h.this_line_no , h.next_line_no );
		p = temp;
		*p = 0;
		while(1)
		{
			int ch = fgetc(fp);
			int ind;
			if( ch == EOF ) break;
			if( ch == 0 ) break;
			if( ch == 0x1f ) cf = 1;
			if( (ch & 0x80) && !cf )
			{	
				if( text[ind = (ch & 0x7f) ] )
				{
					int len = strlen(text[ ind ]);
					int i;
					for(i = 0 ; i < len ; i ++ )
					{
						*(p++) = text[ind][i];
						*p=0;
					}
					if( p[-1]>='A' && p[-1] <='Z' )
					{
						*(p++) = ' ';
						*p = 0;
					}
				}
			}
			else
			{
				if( cf != 1 )
				{
					*(p++) = ch;
					*p=0;
				}
				if( cf ) cf++;
				if( cf == 4 ) cf = 0;
			}
		}
		p = temp;
		*line = h.this_line_no;
		flag = 1;	
	}
	if(!flag)
	{
		free(temp);
		return get_line( fp , format , line );
	}
	else 
		return p;
}
