/// Endianness utilities

#pragma once

#include "jvmdef.h"

namespace XJVM
{

/// <summary>
/// Reverse the bytes in a value
/// </summary>
/// <typeparam name="T">The integral type of the value</typeparam>
/// <param name="x">The value to reverse</param>
/// <returns>The value with its bytes reversed</returns>
template <std::integral T> static constexpr T Byteswap(T x)
{
	constexpr auto size = sizeof(T);
	static_assert(size <= 8, "T is not byteswappable");
	if constexpr (size == 1)
	{
		return x;
	}
	else if constexpr (size == 2)
	{
		return std::_Byteswap_ushort(x);
	}
	else if constexpr (size == 4)
	{
		return std::_Byteswap_ulong(x);
	}
	else if constexpr (size == 8)
	{
		return std::_Byteswap_uint64(x);
	}
}

/// <summary>
/// Convert a native endian value to big endian
/// </summary>
/// <typeparam name="T">The integral type of the value</typeparam>
/// <param name="x">The native endian value to convert to big endian</param>
/// <returns>The value in big endian</returns>
template <std::integral T> static constexpr T BigEndian(T x)
{
	if constexpr (std::endian::native == std::endian::big)
	{
		return x;
	}
	else
	{
		return Byteswap(x);
	}
}

/// <summary>
/// Convert a big endian value to native endian
/// </summary>
/// <typeparam name="T">The integral type of the value</typeparam>
/// <param name="x">The big endian value to convert to native endian</param>
/// <returns>The value in native endian</returns>
template <std::integral T> static constexpr T NativeEndian(T x)
{
	if constexpr (std::endian::native == std::endian::big)
	{
		return x;
	}
	else
	{
		return Byteswap(x);
	}
}

} // namespace XJVM
