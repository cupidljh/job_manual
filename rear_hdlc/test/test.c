// test.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
#define DEBUG

#ifdef DEBUG
#include <Windows.h>

void _OutputDebugString(LPCTSTR pszStr, ...)
{
#ifdef DEBUG   
	TCHAR szMsg[256];
	va_list args;
	va_start(args, pszStr);
	_vstprintf_s(szMsg, 256, pszStr, args);
	OutputDebugString(szMsg);
#endif   
}

#define printf _OutputDebugString
#endif

#include "stdafx.h"
#include <stdio.h>

int main()
{
	printf("babay\n");

    return 0;
}

