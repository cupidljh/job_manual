#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>

#define printf MyOutputDebugString   //printf를 Debug 시에 바꿔준다.

void MyOutputDebugString(char *format, ...) // print할 파라미터 받아오기
{
	char outString[256] = { 0, };

	va_list args;
	va_start(args, format);

	vsprintf(outString, format, args);  //파라미터들을 버퍼에 넣기

	va_end(args);

	OutputDebugString(outString);  // Debug 창에 출력
}
#endif

#endif
