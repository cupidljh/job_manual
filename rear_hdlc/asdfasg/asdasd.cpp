#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

void MyOutputDebugString(char *format, ...)
{
	char outString[256];

	va_list args;
	va_start(args, format);

	vsprintf(outString, format, args);

	va_end(args);

	OutputDebugString((LPCWSTR)outString);
}

int main(void)
{
	MyOutputDebugString("가변인자 테스트");
	MyOutputDebugString("%d는 16진수로 %X", 10, 10);
	MyOutputDebugString("%s 는 %s다", "바나나", "길");

	return 0;
}


void DebugString(int i, char* c)
{
	char buf[256];
	int len;
	len = sprintf(buf, c);
	sprintf(buf + len, "%d" ,i);

	puts(buf);
}

int main(void)
{
	int a = 10;
	char s[] = "yoyyoy";

	DebugString(a, s);

	return 0;
}

