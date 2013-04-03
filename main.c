#include "statement.h"
#include "preproc.h"
char* code[65536];
char* p;
FILE* fp;
extern char * font_path;
int main( int argc , char** argv )
{
	font_path = "font.ttf";
	init();
	int line;
	if(argc == 1 )
		fp = stdin;
	else
		fp = fopen( argv[1] , "rb");
	int format = get_source_format(fp);
	char* p;
	Data* data;
	while(p = get_line( fp , format , &line))
		code[line] = p;
	output_error(data = parse_program(code));
	dispose(data);
	inkey();
	return 0;
}
