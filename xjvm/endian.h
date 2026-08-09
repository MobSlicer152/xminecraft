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
	requires(sizeof(T) <= 8)
{
	return std::byteswap(x);
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

	return Byteswap(x);
}

} // namespace XJVM
