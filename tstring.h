#pragma once
#include <string>
#include <tchar.h>

///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#ifndef tstring
#define tstring std::wstring
#endif
#else
#ifndef tstring
#define tstring std::string
#endif
#endif

#ifdef _UNICODE
#ifndef TString
#define TString std::wstring
#endif
#else
#ifndef TString
#define TString std::string
#endif
#endif