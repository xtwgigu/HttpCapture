// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <string>
#include <httpserv.h>

#ifdef _UNICODE
typedef std::wstring TString;
#else
typedef std::string TString;
#endif




// TODO: reference additional headers your program requires here
