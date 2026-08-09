/// Class file parsing API

#pragma once

#include <map>
#include <string_view>
#include <vector>

#include "classfile_structs.h"
#include "endian.h"
#include "jvmdef.h"

namespace XJVM
{

// 4.1 The ClassFile Structure

/// <summary>
/// Class file parser
/// </summary>
class ClassFile
{
  public:
	/// <summary>
	/// Parse a class file from disk
	/// </summary>
	/// <param name="fileName">The path to the class file to parse</param>
	ClassFile(const char* fileName);

	/// <summary>
	/// Parse a class file in memory
	/// </summary>
	/// <param name="classData">The buffer to parse the class data from</param>
	ClassFile(std::span<const u1> classData);

	/// <summary>
	/// Clean up the class file
	/// </summary>
	~ClassFile() = default;

	/// <summary>
	/// Whether the class was successfully parsed
	/// </summary>
	bool IsValid() const
	{
		return m_valid;
	}

	/// <summary>
	/// Get the major version of the class file
	/// </summary>
	/// <returns>The major version of the class file</returns>
	u2 GetMajorVersion() const
	{
		return m_majorVersion;
	}

	/// <summary>
	/// Get the minor version of the class file
	/// </summary>
	/// <returns>The minor version of the class file</returns>
	u2 GetMinorVersion() const
	{
		return m_minorVersion;
	}

	/// <summary>
	/// Get a constant
	/// </summary>
	const ConstantInfo& GetConstant(u2 index) const
	{
		if (m_constantPool.contains(index))
		{
			return m_constantPool.at(index);
		}

		return CONSTANT_PAD_ENTRY;
	}

	/// <summary>
	/// Get the number of constants in the constant pool
	/// </summary>
	/// <returns>The number of constants</returns>
	u2 GetConstantPoolSize() const
	{
		return m_constantPoolSize;
	}

	/// <summary>
	/// Get the offset of the string for a constant
	/// </summary>
	/// <param name="constant">The constant to read from</param>
	/// <param name="getDescriptor>Get the descriptor instead of the name of a reference</param>
	/// <returns>The string pointed to, or nothing if not a string constant</returns>
	ConstOffsetStringView GetStringOffset(const ConstantInfo& constant, bool getDescriptor = false) const;

	/// <summary>
	/// Get the offset of a string for a constant
	/// </summary>
	/// <param name="constant">The index of the constant to read from</param>
	/// <param name="getDescriptor>Get the descriptor instead of the name of a reference</param>
	/// <returns>The string pointed to, or nothing if not a string constant</returns>
	ConstOffsetStringView GetStringOffset(u2 index, bool getDescriptor = false) const
	{
		return GetStringOffset(GetConstant(index), getDescriptor);
	}

	/// <summary>
	/// Get a string for a constant
	/// </summary>
	/// <param name="constant">The constant to read from</param>
	/// <param name="getDescriptor>Get the descriptor instead of the name of a reference</param>
	/// <returns>The string pointed to, or nothing if not a string constant</returns>
	const std::string_view GetString(const ConstantInfo& constant, bool getDescriptor = false) const;

	/// <summary>
	/// Get a string for a constant
	/// </summary>
	/// <param name="constant">The index of the constant to read from</param>
	/// <param name="getDescriptor>Get the descriptor instead of the name of a reference</param>
	/// <returns>The string pointed to, or nothing if not a string constant</returns>
	const std::string_view GetString(u2 index, bool getDescriptor = false) const
	{
		return GetString(GetConstant(index), getDescriptor);
	}

	/// <summary>
	/// Get the class's access flags
	/// </summary>
	/// <returns>The access flags</returns>
	ClassAccessFlags GetAccessFlags() const
	{
		return m_accessFlags;
	}

	/// <summary>
	/// Get this class's constant info
	/// </summary>
	/// <returns>The constant</returns>
	const ConstantInfo& GetThisClass() const
	{
		return GetConstant(m_thisClass);
	}

	/// <summary>
	/// Get this class's super class's constant info
	/// </summary>
	/// <returns>The constant</returns>
	const ConstantInfo& GetSuperClass() const
	{
		return GetConstant(m_superClass);
	}

	/// <summary>
	/// Get the name of the given interface
	/// </summary>
	/// <param name="index">The index in the interface array to get</param>
	/// <returns>The string pointed to by that interface, if any</returns>
	const std::string_view GetInterface(size_t index) const
	{
		if (index < m_interfaces.size())
		{
			return GetString(m_interfaces[index]);
		}

		return {};
	}

	/// <summary>
	/// Get the number of interfaces this class implements
	/// </summary>
	/// <returns>The number of interfaces</returns>
	size_t GetInterfaceCount() const
	{
		return m_interfaces.size();
	}

  private:
	static constexpr u4 MAGIC = 0xCAFEBABE;
	static constexpr u2 MAJOR_VERSION = 50; // Java SE 6

	static constexpr ConstantInfo CONSTANT_PAD_ENTRY = {};

	bool m_valid = false;

	u4 m_magic = MAGIC;
	u2 m_minorVersion = 0;
	u2 m_majorVersion = MAJOR_VERSION;
	u2 m_constantPoolSize = 0;
	std::map<u2, ConstantInfo> m_constantPool;
	ClassAccessFlags m_accessFlags = ClassAccessFlags::UNKNOWN;
	u2 m_thisClass = 0;
	u2 m_superClass = 0;
	std::vector<u2> m_interfaces;
	std::vector<MemberInfo> m_fields;
	std::vector<MemberInfo> m_methods;
	std::vector<AttributeInfo> m_attributes;

	// all UTF-8 constants pooled together
	std::vector<u1> m_stringData;

	// attribute data
	std::vector<u1> m_attributeData;

	// code (if present)
	std::vector<u1> m_code;

	/// <summary>
	/// Read the value at the offset
	/// </summary>
	/// <typeparam name="T">The type of value to read</typeparam>
	/// <param name="data">Class file data</param>
	/// <param name="offset">Current offset into the class</param>
	/// <returns>The value read</returns>
	template <std::integral T> T ReadValueAt(std::span<const u1> data, size_t offset)
	{
		XJVM_ASSERT((offset + sizeof(T)) < data.size(), "tried to read past end of class file data");
		return SwapEndian(*(const T*)(data.data() + offset));
	}

	/// <summary>
	/// Read the value at the offset and advance by its size
	/// </summary>
	/// <typeparam name="T">The type of value to read</typeparam>
	/// <param name="data">Class file data</param>
	/// <param name="offset">Current offset into the class</param>
	/// <returns>The value read</returns>
	template <std::integral T>
	T ReadNextValue(std::span<const u1> data, size_t& offset)
	{
		auto value = ReadValueAt<T>(data, offset);
		offset += sizeof(T);
		return value;
	}

	/// <summary>
	/// Read the bytes at the offset and advance by the size
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">Current offset into the class</param>
	/// <param name="dest">Where to copy the bytes to</param>
	void ReadBytes(std::span<const u1> data, size_t& offset, std::span<u1> dest);

	template <typename T>
	void ReadArray(std::span<const u1> data, size_t& offset, std::span<T> dest)
	{
		ReadBytes(data, offset, std::span<u1>((u1*)dest.data(), dest.size_bytes()));
	}

	/// <summary>
	/// Read a string and add it to the string buffer
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">Current offset into the class</param>
	/// <param name="length">The length of the string</param>
	/// <returns>The offset of the string in the string buffer</returns>
	u4 ReadString(std::span<const u1> data, size_t& offset, u2 length);

	/// <summary>
	/// Parse a class file
	/// </summary>
	/// <param name="data">Class file data</param>
	void Parse(std::span<const u1> data);
	
	/// <summary>
	/// Parses the constant pool
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the file</param>
	void ParseConstantPool(std::span<const u1> data, u4& offset);

	/// <summary>
	/// Parses a constant from the constant pool
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the file</param>
	/// <param name="index">The index of the current entry, incremented before return</param>
	void ParseConstant(std::span<const u1> data, u4& offset, u2& index);

	/// <summary>
	/// Read the interface indices
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	void ParseInterfaces(std::span<const u1> data, u4& offset);

	/// <summary>
	/// Parse fields and methods
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	void ParseMembers(std::span<const u1> data, u4& offset);

	/// <summary>
	/// Parse a field or method
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	/// <param name="info">The member to store the data in</param>
	/// <param name="type">The type of member this is</param>
	void ParseMember(std::span<const u1> data, u4& offset, MemberInfo& info, MemberType type);

	/// <summary>
	/// Parse an attribute
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	/// <returns>The subset of m_attributes where the attributes are</returns>
	OffsetSpan<const AttributeInfo> ParseAttributes(std::span<const u1> data, u4& offset);

	/// <summary>
	/// Parse a field or method
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	/// <param name="info">The member to store the data in</param>
	/// <param name="type">The type of member this is</param>
	OffsetSpan<const u1> ParseAttribute(std::span<const u1> data, u4& offset, const std::string_view name);
};

} // namespace XJVM
