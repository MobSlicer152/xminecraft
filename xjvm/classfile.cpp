#include "stdafx.h"

using namespace XJVM;

void ClassFile::Parse(std::span<const u1> data)
{
	// the file is valid until proven otherwise
	m_valid = true;
	size_t offset = 0;

	// read and check magic
	m_magic = ReadNextValue<u4>(data, offset);
	if (m_magic != MAGIC)
	{
		Message("class has incorrect magic 0x%08X\n", m_magic);
		m_valid = false;
		return;
	}

	// read and check version
	m_minorVersion = ReadNextValue<u2>(data, offset);
	m_majorVersion = ReadNextValue<u2>(data, offset);
	if (m_majorVersion != MAJOR_VERSION)
	{
		Message("class version %hu.%hu does not match supported version %hu\n", m_majorVersion, m_minorVersion, MAJOR_VERSION);
		m_valid = false;
		return;
	}

	// read the constant pool
	ParseConstantPool(data, offset);

	// read access flags and class information
	m_accessFlags = (ClassAccessFlags)ReadNextValue<u2>(data, offset);
	m_thisClass = ReadNextValue<u2>(data, offset);
	m_superClass = ReadNextValue<u2>(data, offset);

	// read interfaces
	ParseInterfaces(data, offset);

	// read fields and methods
	ParseMembers(data, offset);

	// read class attributes
	ParseAttributes(data, offset);
}

void ClassFile::ParseConstantPool(std::span<const u1> data, u4& offset)
{
	u2 i = 1;
	m_constantPoolSize = ReadNextValue<u2>(data, offset);
	while (i < m_constantPoolSize)
	{
		ParseConstant(data, offset, i);
	}
}

void ClassFile::ReadBytes(std::span<const u1> data, size_t& offset, std::span<u1> dest)
{
	XJVM_ASSERT((offset + dest.size()) < data.size(), "tried to read past end of class file data");
	memcpy(dest.data(), &data[offset], dest.size());
	offset += dest.size();
}

u4 ClassFile::ReadString(std::span<const u1> data, size_t& offset, u2 length)
{
	XJVM_ASSERT((offset + length) < data.size(), "tried to read past end of class file data");
	auto strOffset = m_stringData.size();
	if (length > 0)
	{
		m_stringData.resize(m_stringData.size() + length);
		memcpy(&m_stringData[strOffset], &data[offset], length);
		offset += length;
	}
	return strOffset;
}

ConstOffsetStringView ClassFile::GetStringOffset(const ConstantInfo& constant, bool getDescriptor /*= false*/) const
{
	u2 index;
	switch (constant.tag)
	{
	// for a utf8 value, just load it
	case ConstantType::Utf8: {
		return ConstOffsetStringView(constant.utf8Info.bytesOffset, constant.utf8Info.length);
	}
	// for a string, get its utf8 value and look that up
	case ConstantType::String: {
		index = constant.stringInfo.stringIndex;
		break;
	}
	// for a class, get its name
	case ConstantType::Class: {
		index = constant.classInfo.nameIndex;
		break;
	}
	// for a reference, get its name-and-type
	case ConstantType::Fieldref:
	case ConstantType::Methodref:
	case ConstantType::InterfaceMethodref: {
		index = constant.referenceInfo.nameAndTypeIndex;
		break;
	}
	// for a name and type, get the name
	case ConstantType::NameAndType: {
		index = getDescriptor ? constant.nameAndTypeInfo.descriptorIndex : constant.nameAndTypeInfo.nameIndex;
		break;
	}
	}

	// if this wasn't a utf8, recurse
	if (m_constantPool.contains(index))
	{
		const auto& utf8Const = m_constantPool.at(index);
		return GetStringOffset(utf8Const);
	}

	return {};
}

const std::string_view ClassFile::GetString(const ConstantInfo& constant, bool getDescriptor) const
{
	return GetStringOffset(constant, getDescriptor).StringView((const void*)m_stringData.data());
}

void ClassFile::ParseConstant(std::span<const u1> data, u4& offset, u2& index)
{
	// 2 for some types
	auto entryCount = 1;

	// parse the constant
	ConstantInfo info = {};
	info.tag = (ConstantType)ReadNextValue<u1>(data, offset);
	switch (info.tag)
	{
	case ConstantType::Class: {
		info.classInfo.nameIndex = ReadNextValue<u2>(data, offset);
		break;
	}
	case ConstantType::Fieldref:
	case ConstantType::Methodref:
	case ConstantType::InterfaceMethodref: {
		info.referenceInfo.classIndex = ReadNextValue<u2>(data, offset);
		info.referenceInfo.nameAndTypeIndex = ReadNextValue<u2>(data, offset);
		break;
	}
	case ConstantType::String: {
		info.stringInfo.stringIndex = ReadNextValue<u2>(data, offset);
		break;
	}
	case ConstantType::Integer:
	case ConstantType::Float: {
		info.scalar32Info.bytes = ReadNextValue<u4>(data, offset);
		break;
	}
	case ConstantType::Long:
	case ConstantType::Double: {
		info.scalar64Info.highBytes = ReadNextValue<u4>(data, offset);
		info.scalar64Info.lowBytes = ReadNextValue<u4>(data, offset);
		entryCount = 2;
		break;
	}
	case ConstantType::NameAndType: {
		info.nameAndTypeInfo.nameIndex = ReadNextValue<u2>(data, offset);
		info.nameAndTypeInfo.descriptorIndex = ReadNextValue<u2>(data, offset);
		break;
	}
	case ConstantType::Utf8: {
		info.utf8Info.length = ReadNextValue<u2>(data, offset);
		info.utf8Info.bytesOffset = ReadString(data, offset, info.utf8Info.length);
		break;
	}
	default: {
		Message("constant %u has unknown tag %u\n", index, info.tag);
		break;
	}
	}

	// add the entry
	m_constantPool[index] = info;
	index += entryCount;
}

void ClassFile::ParseInterfaces(std::span<const u1> data, u4& offset)
{
	m_interfaces.resize(ReadNextValue<u2>(data, offset));
	for (size_t i = 0; i < m_interfaces.size(); i++)
	{
		m_interfaces[i] = ReadNextValue<u2>(data, offset);
	}
}

void XJVM::ClassFile::ParseMembers(std::span<const u1> data, u4& offset)
{
	// read fields
	m_fields.resize(ReadNextValue<u2>(data, offset));
	for (auto& field : m_fields)
	{
		ParseMember(data, offset, field, MemberType::Field);
	}

	// read methods
	m_methods.resize(ReadNextValue<u2>(data, offset));
	for (auto& method : m_methods)
	{
		ParseMember(data, offset, method, MemberType::Method);
	}
}

void XJVM::ClassFile::ParseMember(std::span<const u1> data, u4& offset, MemberInfo& info, MemberType type)
{
	info.type = type;
	info.accessFlags = (MemberAccessFlags)ReadNextValue<u2>(data, offset);
	info.name = GetStringOffset(ReadNextValue<u2>(data, offset));		// read the name index and get it
	info.descriptor = GetStringOffset(ReadNextValue<u2>(data, offset)); // read the descriptor index and get it
	info.attributes = ParseAttributes(data, offset);
}

OffsetSpan<const AttributeInfo> XJVM::ClassFile::ParseAttributes(std::span<const u1> data, u4& offset)
{
	// attributes_count
	auto span = OffsetSpan<AttributeInfo>(m_attributes.size(), ReadNextValue<u2>(data, offset));
	if (span.Size() == UINT16_MAX)
	{
		return OffsetSpan<AttributeInfo>(m_attributes.size(), 0);
	}
	m_attributes.resize(m_attributes.size() + span.Size());

	for (auto& attrib : span.View(m_attributes.data()))
	{
		attrib.name = GetStringOffset(ReadNextValue<u2>(data, offset));
		attrib.info = ParseAttribute(data, offset, attrib.name.StringView(m_stringData.data()));
	}

	return span;
}

OffsetSpan<const u1> ClassFile::ParseAttribute(std::span<const u1> data, u4& offset, const std::string_view name)
{
	// read the attribute length
	auto length = ReadNextValue<u4>(data, offset);
	// output span that gets returned
	OffsetSpan<u1> result;

	// handles extending the attribute data buffer and setting up the result span
	auto extendData = [&](size_t size) {
		result = OffsetSpan<u1>(m_attributeData.size(), size);
		m_attributeData.resize(m_attributeData.size() + size);
		return result.View(m_attributeData.data());
	};

	// parse stuff
	switch ((AttributeType)FNV(name))
	{
	case AttributeType::ConstantValue: {
		auto view = extendData(sizeof(ConstantValueAttribute));
		auto& constant = *(ConstantValueAttribute*)view.data();
		constant.constantValueIndex = ReadNextValue<u2>(data, offset);
		break;
	}
	case AttributeType::Code: {
		// get basic stuff into the struct
		CodeAttribute code = {};
		code.maxStack = ReadNextValue<u2>(data, offset);
		code.maxLocals = ReadNextValue<u2>(data, offset);

		// read the size of the code and extend the code buffer for it
		auto codeLength = ReadNextValue<u4>(data, offset);
		auto codeOffset = m_code.size();
		m_code.resize(m_code.size() + codeLength);

		// read the code
		auto codeSpan = OffsetSpan<u1>(codeOffset, codeLength);
		auto codeView = codeSpan.View(m_code.data());
		ReadBytes(data, offset, codeView);
		code.code = codeSpan;

		// read number of exceptions
		code.exceptionCount = ReadNextValue<u2>(data, offset);

		// now that the size is known, extend the data and copy the struct in
		auto view = extendData(sizeof(CodeAttribute) + code.exceptionCount * sizeof(CodeExceptionData));
		memcpy(view.data(), &code, sizeof(CodeAttribute));

		// read the exceptions
		auto& exceptions = ((CodeAttribute*)view.data())->exceptions;
		for (u2 i = 0; i < code.exceptionCount; i++)
		{
			exceptions[i].startPc = ReadNextValue<u2>(data, offset);
			exceptions[i].endPc = ReadNextValue<u2>(data, offset);
			exceptions[i].handlerPc = ReadNextValue<u2>(data, offset);
			exceptions[i].catchType = ReadNextValue<u2>(data, offset);
		}

		// read attributes
		code.attributes = ParseAttributes(data, offset);

		break;
	}
	case AttributeType::Exceptions: {
		// read the number of exceptions
		auto count = ReadNextValue<u2>(data, offset);

		// extend the attribute data
		auto view = extendData(sizeof(ExceptionsAttribute) + count * sizeof(u2));

		// read the array of exceptions
		auto& exceptions = *(ExceptionsAttribute*)view.data();
		exceptions.exceptionCount = count;
		ReadArray(data, offset, std::span(exceptions.exceptionIndexTable, exceptions.exceptionCount));

		break;
	}
	default: {
		Message("unknown attribute %.*s\n", name.size(), name.data());
		[[fallthrough]];
	}
	case AttributeType::Synthetic:
	case AttributeType::SourceFile:
	case AttributeType::LineNumberTable:
	case AttributeType::LocalVariableTable:
	case AttributeType::Deprecated: {
		// skip the data
		offset += length;
		break;
	}
	}

	return result;
}

ClassFile::ClassFile(const char* fileName)
{
	auto f = fopen(fileName, "rb");
	if (!f)
	{
		Message("failed to open class file %s\n", fileName);
		return;
	}

	fseek(f, 0, SEEK_END);
	auto size = ftell(f);
	fseek(f, 0, SEEK_SET);

	auto data = std::vector<u1>(size);
	fread(data.data(), 1, data.size(), f);
	fclose(f);

	Parse(data);
}

ClassFile::ClassFile(std::span<const u1> classData)
{
	Parse(classData);
}
