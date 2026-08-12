/// <summary>
/// General utilities
/// </summary>

#pragma once

#include "jvmdef.h"

namespace XJVM
{

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
