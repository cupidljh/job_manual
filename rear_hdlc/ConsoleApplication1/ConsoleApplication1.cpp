// ConsoleApplication1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <stdarg.h>

//
//void DebugString(int i, char* c)
//{
//	char buf[256];
//	sprintf(buf, "%d %s", i, c);
//
//	OutputDebugStringA(buf);
//}
//
//int main(void)
//{
//	int a = 10;
//	char s[] = "yoyyoy";
//
//	DebugString(a, s);
//
//	return 0;
//}


void MyOutputDebugString(char *format, ...)
{
	char outString[256] = { 0, };

	va_list args;
	va_start(args, format);

	vsprintf(outString, format, args);

	va_end(args);

	OutputDebugString(outString);
}

int main(void)
{
	MyOutputDebugString(("가변인자 테스트"));
	//MyOutputDebugString("%d는 16진수로 %X", 10, 10);
	//MyOutputDebugString("%s 는 %s다", "바나나", "길");

	return 0;
}
//
//void MyVarFunc(char* szFormat, ...)
//{
//	char szBuf[1024] = { 0, };
//
//	va_list lpStart;
//	va_start(lpStart, szFormat);
//
//	vsprintf(szBuf, szFormat, lpStart);
//
//	printf(szBuf);
//
//	va_end(lpStart);
//}
//
//int main()
//{
//	int a = 5, b = 4;
//	MyVarFunc("%d + %d = %d\n", a, b, a + b);
//	return 0;
//}