#ifndef __HEADER__CODESET
#define __HEADER__CODESET
#include <iconv.h>
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen);
#endif
