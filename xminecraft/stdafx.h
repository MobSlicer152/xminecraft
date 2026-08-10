// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NO_STRICT
#define _MSVC_STL_USE_ABORT_AS_DOOM_FUNCTION
#define _NO_CRT_STDIO_INLINE
#define _CRT_SECURE_NO_WARNINGS

// modern MSVC declares these in an STL header, and that conflicts with the XTL ones
#define _InterlockedCompareExchange _InterlockedCompareExchange_XBOX
#define _InterlockedDecrement		_InterlockedDecrement_XBOX
#define _InterlockedExchange		_InterlockedExchange_XBOX
#define _InterlockedExchangeAdd		_InterlockedExchangeAdd_XBOX
#define _InterlockedIncrement		_InterlockedIncrement_XBOX

#include <stdio.h>
// clang-format off
#include <xtl.h>
#include <xgraphics.h>
// clang-format on

#undef _InterlockedCompareExchange
#undef _InterlockedDecrement
#undef _InterlockedExchange
#undef _InterlockedExchangeAdd
#undef _InterlockedIncrement

#include "../xjvm/xjvm.h"

extern "C" void NTAPI DbgPrint(const char* message, ...);

// TODO: reference additional headers your program requires here
