/// <summary>
/// Endianness utilities
/// </summary>

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
	else
	{
		return Byteswap(x);
	}
}

} // namespace XJVM
