#include "log.h"

void putLog(const char *pFileName, const char *pLog)
{
	// format
	char Buffer[8192] = { 0 };
	_ASSERTE(strlen(pLog) <= 8000);
	SYSTEMTIME T;
	::GetLocalTime(&T);
	_snprintf_s(Buffer, ARRAY_SIZE(Buffer), _TRUNCATE, "(%d) %02d:%02d:%02d.%03d : %s", ::GetCurrentThreadId(),
		T.wHour, T.wMinute, T.wSecond, T.wMilliseconds,
		pLog);
	// output
	OutputDebugStringA(Buffer);
	//printf_s("%s\n", Buffer);
}
void OutputDebugInfo(const char *pFileName, const char *pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	char Buffer[1024] = { 0 };
	int Ret = _vsnprintf_s(Buffer, 1024, _TRUNCATE, pFormat, ArgList); //  Format 后字符串最大长度为 1023
	//int Ret = _vsnprintf(Buffer, 1024, /*_TRUNCATE,*/ pFormat, ArgList);
	if (Ret < 0)
		Buffer[1023] = '\0';

	va_end(ArgList);

	putLog(pFileName, Buffer);
}

void OutputDebugInfo(const char *pFileName, const wchar_t *pFormat, ...)
{
	DWORD dwLastError = GetLastError();
	va_list ArgList;
	va_start(ArgList, pFormat);

	wchar_t Buffer[1024];
	int WideLen = _vsnwprintf_s(Buffer, 1024, _TRUNCATE, pFormat, ArgList);
	if (WideLen < 0)
	{
		Buffer[1023] = 0;
		WideLen = (int) wcslen(Buffer);
	}
	va_end(ArgList);

	char Buffer2[2048];
	int Length = WideCharToMultiByte(CP_ACP, 0, Buffer, WideLen, NULL, 0, NULL, NULL);
	_ASSERTE(Length < sizeof(Buffer2));
	WideCharToMultiByte(CP_ACP, 0, Buffer, WideLen, Buffer2, Length, NULL, NULL);
	Buffer2[Length] = 0;

	putLog(pFileName, Buffer2);

	SetLastError(dwLastError);
}

void OutputDebugHex(const char *pFileName, const char *pHeader, const unsigned char *pData, unsigned int Len)
{

	DWORD dwLastError = GetLastError();
	const int LineLen = 16;
	char ResultBuffer[MAX_HEX_LEN * 3 + MAX_HEX_LEN / LineLen * 2 + 300];

	if (pData == NULL)
		return;

	if (Len > MAX_HEX_LEN)
	{
		_snprintf_s(ResultBuffer, ARRAY_SIZE(ResultBuffer), _TRUNCATE, "%s Len=%d (RealLen=%d, too long)\r\n", pHeader, MAX_HEX_LEN, Len);
		Len = MAX_HEX_LEN;
	}
	else
	{
		_snprintf_s(ResultBuffer, ARRAY_SIZE(ResultBuffer), _TRUNCATE, "%s Len=%d\r\n", pHeader, Len);
	}

	char *pBuf = ResultBuffer + strlen(ResultBuffer);
	for (unsigned int i = 0; i < Len; i++)
	{
		_snprintf_s(pBuf, 4, _TRUNCATE, " %02X", (BYTE) pData[i]);
		pBuf += 3;
		if (((i + 1) % LineLen) == 0 && i != Len - 1)
		{
			strncpy_s(pBuf, 3, "\r\n", 2);
			pBuf += 2;
		}
	}
	*pBuf = 0;
	putLog(pFileName, ResultBuffer);

	SetLastError(dwLastError);
}