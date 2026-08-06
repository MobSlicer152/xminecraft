// Class file format

#pragma once

#include "jvmdef.h"
#include "classfile_structs.h"

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
	/// Parse a class file
	/// </summary>
	/// <param name="classData">The buffer to parse the class data from</param>
	ClassFile(std::span<const JByte> classData);
	
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
	static constexpr u4 CLASSFILE_MAGIC = 0xCAFEBABE;

	bool m_valid;

	u4 m_magic;
	u2 m_majorVersion;
	u2 m_minorVersion;
	u2 m_constantCount;
	ConstantInfo* m_constantPool;
	ClassAccessFlags m_accessFlags;
	u2 m_thisClass;
	u2 m_superClass;
	u2 m_interfaceCount;
	u2* m_interfaces;
	u2 m_fieldCount;
	FieldInfo* m_fields;
	u2 m_methodCount;
	MethodInfo* m_methods;
	u2 m_attributeCount;

	// combined data for nested variably sized arrays
	// (java class files are poorly designed)

	u1* m_constantData;
	size_t m_constantDataSize;
	u1* m_fieldData;
	size_t m_fieldDataSize;
	u1* m_attributeData;
	size_t m_attributesSize;
	u1* m_methodData;
	size_t m_methodDataSize;

	size_t ParseConstantPool();
	size_t ParseConstant(size_t offset = SIZE_MAX);
};

} // namespace XJVM
