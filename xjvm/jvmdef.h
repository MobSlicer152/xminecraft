/// <summary>
/// Common definitions for the whole JVM
/// </summary>

#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <limits>
#include <span>
#include <string_view>

namespace XJVM
{

// 3.3 Primitive Types and Values

// 3.3.1 Integral Types and Values

typedef int8_t JByte;
typedef int16_t JShort;
typedef int32_t JInt;
typedef int64_t JLong;

static constexpr JByte JBYTE_MIN = INT8_MIN;
static constexpr JByte JBYTE_MAX = INT8_MAX;
static constexpr JShort JSHORT_MIN = INT16_MIN;
static constexpr JShort JSHORT_MAX = INT16_MAX;
static constexpr JInt JINT_MIN = INT32_MIN;
static constexpr JInt JINT_MAX = INT32_MAX;
static constexpr JLong JLONG_MIN = INT64_MIN;
static constexpr JLong JLONG_MAX = INT64_MAX;

// 3.3.2 Floating-Point Types and Values

typedef float JFloat;
typedef double JDouble;

static constexpr JFloat JFLOAT_MIN = FLT_MIN;
static constexpr JFloat JFLOAT_MAX = FLT_MAX;
static constexpr JFloat JFLOAT_EPSILON = FLT_EPSILON;

static constexpr JDouble JDOUBLE_MIN = DBL_MIN;
static constexpr JDouble JDOUBLE_MAX = DBL_MAX;
static constexpr JDouble JDOUBLE_EPISLON = DBL_EPSILON;

// 3.3.3 The returnAddress Type

typedef void* JReturnAddress;

// 3.3.4 The boolean Type

typedef int JBoolean;

/// <summary>
/// The type for the message callback
/// </summary>
typedef void (*MessageCallback)(const char* message);

/// <summary>
/// User overridable message function
/// </summary>
extern MessageCallback g_msgCallback;

/// <summary>
/// Call the message callback
/// </summary>
/// <param name="msg">Format string to use</param>
/// <param name="">Additional arguments for format string</param>
static constexpr void Message(_Printf_format_string_ const char* msg, ...)
{
	if (g_msgCallback)
	{
		va_list args;
		char buf[256] = {};
		va_start(args, msg);
#pragma warning(suppress : 4996)
		_vsnprintf(buf, _countof(buf), msg, args);
		g_msgCallback(buf);
		va_end(args);
	}
}

/// <summary>
/// Abort and write a message if condition is false
/// </summary>
#define XJVM_ASSERT(cond, ...)                                     \
	if (!(cond))                                                   \
	{                                                              \
		XJVM::Message("Assertion " #cond " failed: " __VA_ARGS__); \
		abort();                                                   \
	}

} // namespace XJVM
