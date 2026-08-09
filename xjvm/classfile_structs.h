/// Detail structs for class files

#pragma once

#include "hash.h"
#include "jvmdef.h"
#include "offspan.h"

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
		/// <summary>
		/// 4.4.1 The CONSTANT_Class_info Structure
		/// <summary>
		struct
		{
			u2 nameIndex;
		} classInfo;
		
		/// <summary>
		/// 4.4.2 The CONSTANT_Fieldref_info, CONSTANT_Methodref_info, and CONSTANT_InterfaceMethodref_info Structures
		/// </summary>
		struct
		{
			u2 classIndex;
			u2 nameAndTypeIndex;
		} referenceInfo;
		
		/// <summary>
		/// 4.4.3 The CONSTANT_String_info Structure
		/// </summary>
		struct
		{
			u2 stringIndex;
		} stringInfo;

		/// <summary>
		/// 4.4.4 The CONSTANT_Integer_info and CONSTANT_Float_info Structures
		/// </summary>
		struct
		{
			u4 bytes;
		} scalar32Info;

		/// <summary>
		/// 4.4.5 The CONSTANT_Long_info and CONSTANT_Double_info Structures
		/// </summary>
		struct
		{
			u4 highBytes;
			u4 lowBytes;
		} scalar64Info;

		/// <summary>
		/// 4.4.6 The CONSTANT_NameAndType_info Structure
		/// </summary>
		struct
		{
			u2 nameIndex;
			u2 descriptorIndex;
		} nameAndTypeInfo;

		/// <summary>
		/// 4.4.7 The CONSTANT_Utf8_info Structure
		/// </summary>
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
/// Access flags for members
/// </summary>
enum class MemberAccessFlags : u2
{
	UNKNOWN = 0x0000,
	PUBLIC = 0x0001,
	PRIVATE = 0x0002,
	PROTECTED = 0x0004,
	STATIC = 0x0008,
	FINAL = 0x0010,
	SYNCHRONIZED = 0x0020,
	TRANSIENT = 0x0080,
	VOLATILE = 0x0040,
	NATIVE = 0x0100,
	ABSTRACT = 0x0200,
	STRICT = 0x0400,
};

enum class AttributeType : uint64_t
{
#define XJVM_MAKE_ATTRIBUTE(x) x = FNV(#x)
	XJVM_MAKE_ATTRIBUTE(ConstantValue),
	XJVM_MAKE_ATTRIBUTE(Code),
	XJVM_MAKE_ATTRIBUTE(Exceptions),
	XJVM_MAKE_ATTRIBUTE(InnerClasses),
	XJVM_MAKE_ATTRIBUTE(Synthetic),
	XJVM_MAKE_ATTRIBUTE(SourceFile),
	XJVM_MAKE_ATTRIBUTE(LineNumberTable),
	XJVM_MAKE_ATTRIBUTE(LocalVariableTable),
	XJVM_MAKE_ATTRIBUTE(Deprecated),
#undef XJVM_MAKE_ATTRIBUTE
};

/// <summary>
/// 4.7 Attributes
/// </summary>
struct AttributeInfo
{
	ConstOffsetStringView name;
	OffsetSpan<const u1> info;
};

/// <summary>
/// 4.7.2 The ConstantValue Attribute
/// </summary>
struct ConstantValueAttribute
{
	u2 constantValueIndex;
};

/// <summary>
/// Exception data for code attributes
/// </summary>
struct CodeExceptionData
{
	u2 startPc;
	u2 endPc;
	u2 handlerPc;
	u2 catchType;
};

/// <summary>
/// 4.7.3 The Code Attribute
/// </summary>
struct CodeAttribute
{
	u2 maxStack;
	u2 maxLocals;
	OffsetSpan<const u1> code;
	OffsetSpan<const AttributeInfo> attributes;
	u2 exceptionCount;
	CodeExceptionData exceptions[0];
};

/// <summary>
/// 4.7.4 The Exceptions Attribute
/// </summary>
struct ExceptionsAttribute
{
	u2 exceptionCount;
	u2 exceptionIndexTable[0];
};

/// <summary>
/// Class data for inner classes attribute
/// </summary>
struct InnerClassData
{
	u2 innerClassInfoIndex;
	u2 outerClassInfoIndex;
	u2 innerNameIndex;
	ClassAccessFlags innerAccessFlags;
};

/// <summary>
/// 4.7.5 The InnerClasses Attribute
/// </summary>
struct InnerClassesAttribute
{
	u2 classCount;
	InnerClassData classes[0];
};

struct SyntheticAttribute
{
};

enum class MemberType
{
	Field = 0,
	Method = 1
};

/// <summary>
/// 4.5 Fields
/// 4.6 Methods
/// </summary>
struct MemberInfo
{
	MemberType type;
	MemberAccessFlags accessFlags;
	ConstOffsetStringView name;
	ConstOffsetStringView descriptor;
	OffsetSpan<const AttributeInfo> attributes;
};

} // namespace XJVM
