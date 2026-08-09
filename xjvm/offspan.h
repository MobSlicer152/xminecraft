/// Helper class to make spans that stay valid when a container is resized

#pragma once

#include <cstdint>
#include <type_traits>

namespace XJVM
{

/// <summary>
/// Like std::span, but just stores an offset and is therefore stable when the container
/// it refers to is resized.
/// </summary>
/// <typeparam name="T">Element type</typeparam>
template <class T> class OffsetSpan
{
  public:
	/// <summary>
	/// Create a new offset span
	/// </summary>
	/// <param name="offset">The offset into the container the data is at</param>
	/// <param name="count">The number of elements in the data</param>
	constexpr OffsetSpan(size_t offset = 0, size_t count = 0) : m_offset(offset), m_count(count)
	{
	}

	/// <summary>
	/// Get a const std::span
	/// </summary>
	/// <param name="base">The base of the container</param>
	/// <returns>A span of const T</returns>
	constexpr std::span<const T> View(const void* base) const
	{
		return RawView(base);
	}

	/// <summary>
	/// Get a const string view
	/// </summary>
	/// <param name="base">The base of the container</param>
	/// <returns>A const string view</returns>
	constexpr const std::string_view StringView(const void* base) const
		requires(std::is_same_v<std::remove_const_t<T>, char>)
	{
		auto span = RawView(base);
		return std::string_view(span.data(), span.size());
	}

	/// <summary>
	/// Get a span
	/// </summary>
	/// <param name="base">The base of the data</param>
	/// <returns>A span</returns>
	constexpr std::span<T> View(void* base)
	{
		return RawView(base);
	}

	/// <summary>
	/// Get a string view
	/// </summary>
	/// <param name="base">The base of the container</param>
	/// <returns>A string view</returns>
	constexpr std::string_view StringView(void* base)
		requires(std::is_same_v<T, char>)
	{
		auto span = RawView(base);
		return std::string_view(span.data(), span.size());
	}

	/// <summary>
	/// Get the size of the span
	/// </summary>
	/// <returns>The size of the span</returns>
	constexpr std::size_t Size() const
	{
		return m_count;
	}

	/// <summary>
	/// Convert to span of const T
	/// </summary>
	constexpr operator OffsetSpan<const T>()
	{
		return OffsetSpan<const T>(m_offset, m_count);
	}

  private:
	size_t m_offset;
	size_t m_count;

	// evil because it takes the const off, but it gets added back in the const
	// versions, and base isn't const otherwise so it's okay
	constexpr std::span<T> RawView(const void* base) const
	{
		auto bytes = (std::byte*)base;
		return std::span<T>((T*)&bytes[m_offset], m_count);
	}
};

using OffsetStringView = OffsetSpan<char>;
using ConstOffsetStringView = OffsetSpan<const char>;

} // namespace XJVM
