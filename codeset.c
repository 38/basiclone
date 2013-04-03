#define CODESET_IMP
#include "common.h"
#include "codeset.h"
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen) 
{ 
	iconv_t cd; 
	char * in  = inbuf;
	char * out = outbuf;
	char **pin = &in; 
	char **pout = &out; 
	cd = iconv_open(to_charset,from_charset); 
	if (cd==0) return -1; 
	memset(outbuf,0,outlen); 
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1; 
	iconv_close(cd); 
	return 0; 
}
