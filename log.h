#pragma once
#include "stdafx.h"
#include <ctype.h>
#define MAX_HEX_LEN     2048
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))
#endif
void putLog(const char *pFileName, const char *pLog);
void OutputDebugInfo(const char *pFileName, const char *pFormat, ...);
void OutputDebugInfo(const char *pFileName, const wchar_t *pFormat, ...);
void OutputDebugHex(const char *pFileName, const char *pHeader, const unsigned char *pData, unsigned int Len);

#if _MSC_VER >= 1300
    #define __NOOP       __noop  
#else
    #define __NOOP       ((void)0)
#endif

#define _ENABLE_LOG

#ifdef _ENABLE_LOG
    #define SHOW_LOG(format, ...)         OutputDebugInfo(__FILE__ , format, __VA_ARGS__)
	
    #define SHOW_LOG_HEX(header, data, len)    OutputDebugHex(__FILE__, header, data, len)
#else
    #define SHOW_LOG        __NOOP 
    #define SHOW_LOG_HEX         __NOOP
#endif
