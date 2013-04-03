#include "preproc.h"
char* code[65536];
char* p;
FILE* fp;
int main( int argc , char** argv )
{
	int line;
	if(argc == 1 )
		fp = stdin;
	else
		fp = fopen( argv[1] , "rb");
	int format = get_source_format(fp);
	while(p = get_line( fp , format , &line))
		printf("%d %s\n",line,p);
	return 0;
}
