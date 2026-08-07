/// Detail structs for class files

#pragma once

#include "jvmdef.h"

namespace XJVM
{

// types for parsing classes

using u1 = uint8_t;
using u2 = uint16_t;
using u4 = uint32_t;

// 4.4 The Constant Pool

/// <summary>
/// Constant pool info type
/// </summary>
enum class ConstantType : u1
{
	Class = 7,
	Fieldref = 9,
	Methodref = 10,
	InterfaceMethodref = 11,
	String = 8,
	Integer = 3,
	Float = 4,
	Long = 5,
	Double = 6,
	NameAndType = 12,
	Utf8 = 1,
	Unknown = 0,
};

struct ConstantInfo
{
	ConstantType tag;
	union {
		// 4.4.1 The CONSTANT_Class_info Structure
		struct
		{
			u2 nameIndex;
		} classInfo;
		
		// 4.4.2 The CONSTANT_Fieldref_info, CONSTANT_Methodref_info, and CONSTANT_InterfaceMethodref_info Structures
		struct
		{
			u2 classIndex;
			u2 nameAndTypeIndex;
		} referenceInfo;
		
		// 4.4.3 The CONSTANT_String_info Structure
		struct
		{
			u2 stringIndex;
		} stringInfo;

		// 4.4.4 The CONSTANT_Integer_info and CONSTANT_Float_info Structures
		struct
		{
			u4 bytes;
		} scalar32Info;

		// 4.4.5 The CONSTANT_Long_info and CONSTANT_Double_info Structures
		struct
		{
			u4 highBytes;
			u4 lowBytes;
		} scalar64Info;

		// 4.4.6 The CONSTANT_NameAndType_info Structure
		struct
		{
			u2 nameIndex;
			u2 descriptorIndex;
		} nameAndTypeInfo;

		// 4.4.7 The CONSTANT_Utf8_info Structure
		struct
		{
			u2 length;
			u4 bytesOffset;
		} utf8Info;
	};
};

/// <summary>
/// Access flags for classes
/// </summary>
enum class ClassAccessFlags : u2
{
	UNKNOWN = 0x0000,
	PUBLIC = 0x0001,
	FINAL = 0x0010,
	SUPER = 0x0020,
	INTERFACE = 0x0200,
	ABSTRACT = 0x0400,
};

/// <summary>
/// Access flags for fields
/// </summary>
enum class FieldAccessFlags : u2
{
	UNKNOWN = 0x0000,
	PUBLIC = 0x0001,
	PRIVATE = 0x0002,
	PROTECTED = 0x0004,
	STATIC = 0x0008,
	FINAL = 0x0010,
	VOLATILE = 0x0040,
	TRANSIENT = 0x0080,
};

/// <summary>
/// Access flags for methods
/// </summary>
enum class MethodAccessFlags : u2
{
	UNKNOWN = 0x0000,
	PUBLIC = 0x0001,
	PRIVATE = 0x0002,
	PROTECTED = 0x0004,
	STATIC = 0x0008,
	FINAL = 0x0010,
	SYNCHRONIZED = 0x0020,
	NATIVE = 0x0100,
	ABSTRACT = 0x0200,
	STRICT = 0x0400,
};

// 4.7 Attributes

/// <summary>
/// Attribute information
/// </summary>
struct AttributeInfo
{
	u2 attributeNameIndex;
	u4 attributeLength;
	//union {
	//};
};

// 4.5 Fields

/// <summary>
/// Field information
/// </summary>
struct FieldInfo
{
	FieldAccessFlags accessFlags;
	u2 nameIndex;
	u2 descriptorIndex;
	u2 attributeCount;
	u4 attributeOffset;
};

// 4.6 Methods

/// <summary>
/// Method information
/// </summary>
struct MethodInfo
{
	MethodAccessFlags accessFlags;
	u2 nameIndex;
	u2 descriptorIndex;
	u2 attributeCount;
	u4 attributeOffset;
};

} // namespace XJVM
