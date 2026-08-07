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

  private:
	static constexpr u4 MAGIC = 0xCAFEBABE;
	static constexpr u2 MAJOR_VERSION = 50; // Java SE 6

	static constexpr ConstantInfo CONSTANT_PAD_ENTRY = {};

	bool m_valid = false;

	u4 m_magic = MAGIC;
	u2 m_minorVersion = 0;
	u2 m_majorVersion = MAJOR_VERSION;
	u2 m_constantPoolSize;
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

	// attribute data (contains both info field and any nested variably sized arrays)
	std::vector<u1> m_attributeData;

	/// <summary>
	/// Read the value at the offset
	/// </summary>
	/// <typeparam name="T">The type of value to read</typeparam>
	/// <param name="data">Class file data</param>
	/// <param name="offset">Current offset into the class</param>
	/// <returns>The value read</returns>
	template <std::integral T> T ReadValueAt(std::span<const u1> data, size_t offset)
	{
		assert((offset + sizeof(T)) < data.size(), "tried to read past end of class file data");
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
	void ParseMember(std::span<const u1> data, u4& offset);

	/// <summary>
	/// Parse an attribute
	/// </summary>
	/// <param name="data">Class file data</param>
	/// <param name="offset">The current offset into the class</param>
	void ParseAttribute(std::span<const u1> data, u4& offset);
};

} // namespace XJVM
