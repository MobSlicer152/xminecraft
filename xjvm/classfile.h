/// Class file parsing API

#pragma once

#include <map>
#include <string_view>
#include <vector>

#include "class_structs.h"
#include "jvmdef.h"
#include "util.h"

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
	/// Empty default constructed class
	/// </summary>
	ClassFile() = default;

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
	bool IsValid() const;

	/// <summary>
	/// Get the major version of the class file
	/// </summary>
	/// <returns>The major version of the class file</returns>
	u2 GetMajorVersion() const;

	/// <summary>
	/// Get the minor version of the class file
	/// </summary>
	/// <returns>The minor version of the class file</returns>
	u2 GetMinorVersion() const;

	/// <summary>
	/// Get a constant
	/// </summary>
	const ConstantInfo& GetConstant(u2 index) const;

	/// <summary>
	/// Get the number of constants in the constant pool
	/// </summary>
	/// <returns>The number of constants</returns>
	u2 GetConstantPoolSize() const;

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
	ConstOffsetStringView GetStringOffset(u2 index, bool getDescriptor = false) const;

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
	const std::string_view GetString(u2 index, bool getDescriptor = false) const;

	/// <summary>
	/// Get a string from an offset view
	/// </summary>
	/// <param name="view">The view to get</param>
	/// <returns>The string view</returns>
	const std::string_view GetString(ConstOffsetStringView view) const;

	/// <summary>
	/// Get the class's access flags
	/// </summary>
	/// <returns>The access flags</returns>
	ClassAccessFlags GetAccessFlags() const;

	/// <summary>
	/// Get this class's constant info
	/// </summary>
	/// <returns>The constant</returns>
	const ConstantInfo& GetThisClass() const;

	/// <summary>
	/// Get this class's super class's constant info
	/// </summary>
	/// <returns>The constant</returns>
	const ConstantInfo& GetSuperClass() const;

	/// <summary>
	/// Get the name of the given interface
	/// </summary>
	/// <param name="index">The index in the interface array to get</param>
	/// <returns>The string pointed to by that interface, if any</returns>
	const std::string_view GetInterface(size_t index) const;

	/// <summary>
	/// Get the number of interfaces this class implements
	/// </summary>
	/// <returns>The number of interfaces</returns>
	size_t GetInterfaceCount() const;

	/// <summary>
	/// Allows access to the parsed information about an attribute
	/// </summary>
	class AttributeView
	{
	  public:
		AttributeView() = default;
		~AttributeView() = default;

		/// <summary>
		/// Get the name of this attribute
		/// </summary>
		/// <returns>The name</returns>
		const std::string_view GetName() const;

		/// <summary>
		/// Get the hash of this attribute's name
		/// </summary>
		/// <returns>The hash</returns>
		uint64_t GetNameHash() const;

		/// <summary>
		/// Get the hash of this attribute's name as an AttributeType
		/// </summary>
		/// <returns>The name hash cast to an AttributeType, may not be a valid enum member</returns>
		AttributeType GetType() const;

		/// <summary>
		/// Get this attribute's info as the given type
		/// </summary>
		/// <typeparam name="T">The type to get the attribute info as</typeparam>
		/// <returns>A reference to the info as the given type</returns>
		template <typename T> const T& GetInfo() const
		{
			return *(const T*)m_info;
		}

	  private:
		std::string_view m_name;
		uint64_t m_nameHash;
		const void* m_info;

		AttributeView(const AttributeInfo& info, const std::vector<u1>& stringData,
				   const std::vector<u1>& attributeData);

		friend class ClassFile;
		friend class MemberView;
	};

	/// <summary>
	/// Allows access to the parsed information about a member of the class
	/// </summary>
	class MemberView
	{
	  public:
		MemberView() = default;
		~MemberView() = default;

		MemberType GetType() const;

		/// <summary>
		/// Get the access flags of this member
		/// </summary>
		/// <returns>The access flags</returns>
		MemberAccessFlags GetAccessFlags() const;

		/// <summary>
		/// Get the name of this member
		/// </summary>
		/// <returns>The name</returns>
		const std::string_view GetName() const;

		/// <summary>
		/// Get the descriptor for this member
		/// </summary>
		/// <returns>The descriptor</returns>
		const std::string_view GetDescriptor() const;

		/// <summary>
		/// Get the attributes of this member
		/// </summary>
		/// <returns>The attributes</returns>
		std::span<const AttributeView> GetAttributes() const;

	  private:
		MemberType m_type;
		MemberAccessFlags m_accessFlags;
		std::string_view m_name;
		std::string_view m_descriptor;
		std::vector<AttributeView> m_attributes;

		MemberView(const MemberInfo& info, const std::vector<u1>& stringData, const std::vector<AttributeInfo>& attributes,
				   const std::vector<u1>& attributeData);

		friend class ClassFile;
	};

	/// <summary>
	/// Get the field at the given index
	/// </summary>
	/// <param name="index">The index of the field</param>
	/// <param name="member">Where to store the field info</param>
	void GetField(size_t index, MemberView& member) const;

	/// <summary>
	/// Get the number of fields in the class
	/// </summary>
	/// <returns>The number of fields</returns>
	size_t GetFieldCount() const;

	/// <summary>
	/// Get a new vector of views of all the fields
	/// </summary>
	/// <returns>A vector of views of all the fields</returns>
	std::vector<MemberView> GetFields() const;

	/// <summary>
	/// Get a method in the class
	/// </summary>
	/// <param name="index">The index of the method</param>
	/// <param name="member">Where to store the method info</param>
	void GetMethod(size_t index, MemberView& member) const;

	/// <summary>
	/// Get the number of methods in the class
	/// </summary>
	/// <returns>The number of methods</returns>
	size_t GetMethodCount() const;

	/// <summary>
	/// Get a new vector of views of all the methods
	/// </summary>
	/// <returns>A vector of views of all the methods</returns>
	std::vector<MemberView> GetMethods() const;

	/// <summary>
	/// Get an attribute
	/// </summary>
	/// <param name="index">The index of the attribute</param>
	/// <param name="attribute">Where to store the attribute info</param>
	void GetAttribute(size_t index, AttributeView& attribute) const;

	/// <summary>
	/// Get a view of an attribute
	/// </summary>
	/// <param name="info">The attribute info to get a view of</param>
	/// <param name="attribute">Where to store the attribute info</param>
	void GetAttribute(AttributeInfo& info, AttributeView& attribute) const;

	/// <summary>
	/// Get the number of attributes in the class
	/// </summary>
	/// <returns>The number of attributes</returns>
	size_t GetAttributeCount() const;

	/// <summary>
	/// Get a new vector of views of all the attributes
	/// </summary>
	/// <returns>The vector of attributes</returns>
	std::vector<AttributeView> GetAttributes() const;

	/// <summary>
	/// Get code from a code attribute
	/// </summary>
	/// <param name="codeAttrib">The code attribute</param>
	/// <returns>A span of the code</returns>
	std::span<const u1> GetCode(CodeAttribute& codeAttrib) const;

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
