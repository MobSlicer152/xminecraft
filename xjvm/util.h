/// <summary>
/// General utilities
/// </summary>

#pragma once

#include "jvmdef.h"

namespace XJVM
{

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

#ifdef _DEBUG
#define DbgMessage Message
#else
#define DbgMessage
#endif

/// <summary>
/// Abort and write a message if condition is false
/// </summary>
#define XJVM_ASSERT(cond, ...)                                     \
	if (!(cond))                                                   \
	{                                                              \
		XJVM::Message("Assertion " #cond " failed: " __VA_ARGS__); \
		abort();                                                   \
	}

/// <summary>
/// True if E is an enum that derives from an integral type
/// </summary>
template <typename E> static constexpr bool IsIntegralEnum = std::is_enum_v<E> && std::is_integral_v<std::underlying_type_t<E>>;

/// <summary>
/// Bitwise or for enums
/// </summary>
template <typename E>
static constexpr E operator|(E a, E b)
	requires(IsIntegralEnum<E>)
{
	return (E)(std::to_underlying(a) | std::to_underlying(b));
}

/// <summary>
/// Bitwise and for enums
/// </summary>
template <typename E>
static constexpr E operator&(E a, E b)
	requires(IsIntegralEnum<E>)
{
	return (E)(std::to_underlying(a) & std::to_underlying(b));
}

/// <summary>
/// Reverses the bytes in a value if the system endian is not big endian
/// </summary>
/// <typeparam name="T">The integral type of the value</typeparam>
/// <param name="x">The value to convert</param>
/// <returns>The reversed value</returns>
template <std::integral T> static constexpr T SwapEndian(T x)
{
	if constexpr (std::endian::native == std::endian::big)
	{
		return x;
	}

	return std::byteswap(x);
}

/// <summary>
/// Read the value at the offset
/// </summary>
/// <typeparam name="T">The type of value to read</typeparam>
/// <param name="data">Data to read from</param>
/// <param name="offset">Current offset into the data</param>
/// <returns>The value read</returns>
template <std::integral T> T ReadValueAt(std::span<const uint8_t> data, size_t offset)
{
	XJVM_ASSERT((offset + sizeof(T)) < data.size(), "tried to read past end of class file data");
	return SwapEndian(*(const T*)(data.data() + offset));
}

/// <summary>
/// Read the value at the offset and advance by its size
/// </summary>
/// <typeparam name="T">The type of value to read</typeparam>
/// <param name="data">Data to read from</param>
/// <param name="offset">Current offset into the data</param>
/// <returns>The value read</returns>
template <std::integral T> T ReadNextValue(std::span<const uint8_t> data, size_t& offset)
{
	auto value = ReadValueAt<T>(data, offset);
	offset += sizeof(T);
	return value;
}

/// <summary>
/// Read the bytes at the offset and advance by the size
/// </summary>
/// <param name="data">Data to read from</param>
/// <param name="offset">Current offset into the data</param>
/// <param name="dest">Where to copy the bytes to</param>
void ReadBytes(std::span<const u1> data, size_t& offset, std::span<u1> dest);

/// <summary>
/// Read the array at the offset and advance by its size
/// </summary>
/// <typeparam name="T">Element type of the array</typeparam>
/// <param name="data">Data to read from</param>
/// <param name="offset">Current offset into the data</param>
/// <param name="dest">Where to copy the array to</param>
template <typename T> void ReadArray(std::span<const uint8_t> data, size_t& offset, std::span<T> dest)
{
	ReadBytes(data, offset, std::span<u1>((uint8_t*)dest.data(), dest.size_bytes()));
}

/// <summary>
/// Get the size of the given file
/// </summary>
/// <param name="file">The file to get the size of</param>
/// <returns>The size of the file</returns>
size_t FileSize(FILE* file);

} // namespace XJVM
