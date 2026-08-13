#pragma once

#define NO_STRICT
#define _MSVC_STL_USE_ABORT_AS_DOOM_FUNCTION
#define _NO_CRT_STDIO_INLINE
// fopen is literally fine
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4002, justification : "adding messages to CRT asserts")

#include "xjvm.h"

#include "miniz.h"
#include <string>
