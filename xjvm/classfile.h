// Class file format

#pragma once

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
	/// Free all the allocated structures in this class
	/// </summary>
	~ClassFile();

	/// <summary>
	/// Whether the class was successfully parsed
	/// </summary>
	bool IsValid() const
	{
		return m_valid;
	}

  private:
	static constexpr u4 MAGIC = 0xCAFEBABE;
	static constexpr u2 MAJOR_VERSION = 68; // Java SE 6

	bool m_valid = false;

	u4 m_magic = MAGIC;
	u2 m_majorVersion = MAJOR_VERSION;
	u2 m_minorVersion = 0;
	u2 m_constantCount = 0;
	ConstantInfo* m_constantPool = nullptr;
	ClassAccessFlags m_accessFlags = ClassAccessFlags::ACC_UNKNOWN;
	u2 m_thisClass = 0;
	u2 m_superClass = 0;
	u2 m_interfaceCount = 0;
	u2* m_interfaces = nullptr;
	u2 m_fieldCount = 0;
	FieldInfo* m_fields = nullptr;
	u2 m_methodCount = 0;
	MethodInfo* m_methods = nullptr;
	u2 m_attributeCount = 0;
	AttributeInfo* m_attributes = nullptr;

	// combined data for nested variably sized arrays (java class files are a bad format to parse)
	u1* m_data = nullptr;
	size_t m_dataSize = 0;

	std::span<u1> m_constantData;
	std::span<u1> m_fieldData;
	std::span<u1> m_methodData;
	std::span<u1> m_attributeData;

	template <std::integral T>
	T ReadValue(std::span<const u1> data, size_t offset)
	{
		return NativeEndian(*(const T*)(data.data() + offset));
	}

	/// <summary>
	/// Parse a class file
	/// </summary>
	/// <param name="data">Class file data</param>
	void Parse(std::span<const u1> data);
	
	/// <summary>
	/// Scans the size needed for m_data
	/// </summary>
	/// <param name="data">Class file data</param>
	void ScanDataSize(std::span<const u1> data);

	/// <summary>
	/// Parses basic fields from the header
	/// </summary>
	/// <param name="data">Class file data</param>
	void ReadHeader(std::span<const u1> data);

	/// <summary>
	/// Parses the constant pool if m_data is allocated, otherwise adds the size needed to m_dataSize.
	/// </summary>
	void ParseConstantPool(std::span<const u1> data);

	/// <summary>
	/// Parses a constant from the constant pool
	/// </summary>
	/// <param name="index>The index of the constant to parse</param>
	/// <param name="offset">
	/// The offset into m_constantData to write the result to, or UINT32_MAX to indicate size calculation only
	/// </param>
	/// <returns>The size of this constant</returns>
	u4 ParseConstant(u2 index, u4 offset = UINT32_MAX);
};

} // namespace XJVM
