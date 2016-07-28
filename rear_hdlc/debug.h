#ifdef DEBUG
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>

#define printf MyOutputDebugString

void MyOutputDebugString(char *format, ...)
{
	char outString[256] = { 0, };

	va_list args;
	va_start(args, format);

	vsprintf(outString, format, args);

	va_end(args);

	OutputDebugString(outString);
}
#endif#pragma once
