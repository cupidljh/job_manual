#define DEBUG

#ifdef DEBUG
#include <Windows.h>
#include <stdarg.h>

#define printf CustomTrace

void CustomTrace(char *format, ...)
{
	char buf[1024];
	va_list marker;

	va_start(marker, format);

	vsprintf(buf, format, marker);

	OutputDebugString(buf);
}

#endif

#include <stdio.h>

int main(void)
{
	int a = 10;
	char *s = "i like";

	
	
	printf(&s);
	

	return 0;
}