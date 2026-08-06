/// Detail structs for class files

#pragma once

#include "jvmdef.h"

namespace XJVM
{

// types for parsing classes

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;

// 4.4 The Constant Pool

/// <summary>
/// Constant pool info type
/// </summary>
enum class ConstantType : u1
{
	CONSTANT_Class = 7,
	CONSTANT_Fieldref = 9,
	CONSTANT_Methodref = 10,
	CONSTANT_InterfaceMethodref = 11,
	CONSTANT_String = 8,
	CONSTANT_Integer = 3,
	CONSTANT_Float = 4,
	CONSTANT_Long = 5,
	CONSTANT_Double = 6,
	CONSTANT_NameAndType = 12,
	CONSTANT_Utf8 = 1,
	CONSTANT_Unknown = 0,
};

struct ConstantInfo
{
	ConstantType tag;
	u4 infoOffset;
};

/// <summary>
/// Access flags for classes
/// </summary>
enum class ClassAccessFlags : u2
{
	ACC_UNKNOWN = 0x0000,
	ACC_PUBLIC = 0x0001,
	ACC_FINAL = 0x0010,
	ACC_SUPER = 0x0020,
	ACC_INTERFACE = 0x0200,
	ACC_ABSTRACT = 0x0400,
};

/// <summary>
/// Access flags for fields
/// </summary>
enum class FieldAccessFlags : u2
{
	ACC_UNKNOWN = 0x0000,
	ACC_PUBLIC = 0x0001,
	ACC_PRIVATE = 0x0002,
	ACC_PROTECTED = 0x0004,
	ACC_STATIC = 0x0008,
	ACC_FINAL = 0x0010,
	ACC_VOLATILE = 0x0040,
	ACC_TRANSIENT = 0x0080,
};

/// <summary>
/// Access flags for methods
/// </summary>
enum class MethodAccessFlags : u2
{
	ACC_UNKNOWN = 0x0000,
	ACC_PUBLIC = 0x0001,
	ACC_PRIVATE = 0x0002,
	ACC_PROTECTED = 0x0004,
	ACC_STATIC = 0x0008,
	ACC_FINAL = 0x0010,
	ACC_SYNCHRONIZED = 0x0020,
	ACC_NATIVE = 0x0100,
	ACC_ABSTRACT = 0x0200,
	ACC_STRICT = 0x0400
};

// 4.7 Attributes

/// <summary>
/// Attribute information
/// </summary>
struct AttributeInfo
{
	u2 attributeNameIndex;
	u4 attributeLength;
	u4 infoOffset;
};

// 4.5 Fields

/// <summary>
/// Field information
/// </summary>
struct FieldInfo
{
	u2 accessFlags;
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
	u2 accessFlags;
	u2 nameIndex;
	u2 descriptorIndex;
	u2 attributeCount;
	u4 attributeOffset;
};

}
