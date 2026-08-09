/// Hashing utilities

#pragma once

#include "jvmdef.h"

namespace XJVM
{

#define FNV_BODY()                                        \
	constexpr uint64_t OFFSET_BASIS = 0xcbf29ce484222325; \
	constexpr uint64_t PRIME = 0x00000100000001B3;        \
                                                          \
	uint64_t hash = OFFSET_BASIS;                         \
	for (size_t i = 0; i < size; i++)                     \
	{                                                     \
		hash ^= (uint8_t)bytes[i];                        \
		hash *= PRIME;                                    \
	}                                                     \
                                                          \
	return hash;

template <typename T> static constexpr uint64_t FNV(std::span<const T> data)
{
	auto bytes = std::as_bytes(data);
	auto size = bytes.size();
	FNV_BODY()
}

static constexpr uint64_t FNV(const std::string_view str)
{
	return FNV(std::span(str));
}

template <typename T, size_t N>
static consteval uint64_t FNV(const T (&bytes)[N], size_t size = N)
	requires(sizeof(T) == 1)
{
	FNV_BODY()
}

template <size_t N>
static consteval uint64_t FNV(const char (&str)[N])
	requires(N > 1)
{
	return FNV<char, N>(str, N - 1);
}

} // namespace XJVM
