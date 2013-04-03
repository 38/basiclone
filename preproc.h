#ifndef __HEADER__PREPROC
#define __HEADER__PREPROC
/*source code format*/
#include "common.h"
#define SOURCE_FORMAT_TXT 0
#define SOURCE_FORMAT_BAS 1
#define SOURCE_FORMAT_UNR -1
int   get_source_format(FILE* fp);
char* get_line(FILE* fp , int format ,int* line);
#endif
