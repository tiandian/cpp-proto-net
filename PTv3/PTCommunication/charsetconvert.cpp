#include "StdAfx.h"
#include "charsetconvert.h"

wchar_t * UTF8ToUnicode( const char* str )
{
	int textlen = 0;
	wchar_t * result = NULL;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str, -1, NULL, 0 );
	
	result = new wchar_t[textlen + 1];
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return  result;
}