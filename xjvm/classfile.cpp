#include <any>
#include <cstdio>
#include <vector>

#include "classfile.h"

using namespace XJVM;

void ClassFile::Parse(std::span<const u1> data)
{
	// initially, the file is valid until proven otherwise
	m_valid = true;

	// read and check the header
	ReadHeader(data);
	if (!m_valid)
	{
		return;
	}

	// do an initial parse of the data to figure out how much space is needed
	ScanDataSize(data);
	if (!m_valid)
	{
		return;
	}

	// parse the data fully now
	ParseConstantPool(data);
}

void ClassFile::ScanDataSize(std::span<const u1> data)
{
	ParseConstantPool(data);
}

void ClassFile::ParseConstantPool(std::span<const u1> data)
{
}

u4 ClassFile::ParseConstant(u2 index, u4 offset /*= UINT32_MAX*/)
{
	return 0;
}

void ClassFile::ReadHeader(std::span<const u1> data)
{
	m_magic = ReadValue<u4>(data, 0);
	if (m_magic != MAGIC)
	{
		m_valid = false;
		return;
	}
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

ClassFile::~ClassFile()
{
	// NOTE: do not put anything that has a destructor that matters in here
	void* ptrs[] = {m_constantPool, m_fields, m_methods, m_attributes, m_data};
	for (auto ptr : ptrs)
	{
		if (ptr)
		{
			delete[] ptr;
		}
	}
}
