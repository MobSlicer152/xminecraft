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
		printf("class has incorrect magic 0x%08X\n", m_magic);
		m_valid = false;
		return;
	}

	// read and check version
	m_minorVersion = ReadNextValue<u2>(data, offset);
	m_majorVersion = ReadNextValue<u2>(data, offset);
	if (m_majorVersion != MAJOR_VERSION)
	{
		printf("class version %hu.%hu does not match supported version %hu\n", m_majorVersion, m_minorVersion, MAJOR_VERSION);
		m_valid = false;
		return;
	}

	// read constant count and parse the pool
	auto constantPoolSize = ReadNextValue<u2>(data, offset);
	ParseConstantPool(data.subspan(offset), offset, constantPoolSize);
	if (!m_valid)
	{
		return;
	}
}

void ClassFile::ParseConstantPool(std::span<const u1> data, u4& offset, u2 poolSize)
{
	if (poolSize > 0)
	{
		u2 i = 0;
		while (i < poolSize - 1)
		{
			ParseConstant(data, offset, i);
		}
	}
}

XJVM::u4 ClassFile::ReadString(std::span<const u1> data, size_t& offset, u2 length)
{
	assert((offset + length) < data.size(), "tried to read past end of class file data");
	auto strOffset = m_stringData.size();
	m_stringData.resize(m_stringData.size() + length);
	memcpy(&m_stringData[strOffset], &data[offset], length);
	offset += length;
	return strOffset;
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
		printf("utf-8 constant %u has value %.*s\n", index, info.utf8Info.length, &m_stringData[info.utf8Info.bytesOffset]);
		break;
	}
	default: {
		printf("constant %u has unknown tag %u\n", index, info.tag);
		break;
	}
	}

	// add the entry
	m_constantPool[index] = info;
	index += entryCount;
}

ClassFile::ClassFile(const char* fileName)
{
	auto f = fopen(fileName, "rb");
	if (!f)
	{
		printf("failed to open class file %s\n", fileName);
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
