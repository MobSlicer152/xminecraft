#include "stdafx.h"

using namespace XJVM;

JarFile::JarFile(const char* fileName)
{
	// open the file
	auto f = fopen(fileName, "rb");
	if (!f)
	{
		Message("failed to open class file %s", fileName);
		return;
	}

	// read the whole thing
	auto data = std::vector<u1>(FileSize(f));
	fread(data.data(), 1, data.size(), f);
	fclose(f);

	// parse it
	Parse(data);

	DbgMessage("Loaded %zu classes from %s", m_classes.size(), fileName);
}

JarFile::JarFile(std::span<const uint8_t> data)
{
	Parse(data);
}

bool JarFile::IsValid() const
{
	return m_valid;
}

const ClassFile* JarFile::GetClass(const std::string_view name) const
{
	return nullptr;
}

const std::unordered_map<std::string_view, ClassFile>& JarFile::GetClasses() const
{
	return m_classes;
}

void JarFile::Parse(std::span<const uint8_t> data)
{
	m_valid = true;

	mz_zip_archive archive = {};
	if (!mz_zip_reader_init_mem(&archive, data.data(), data.size(), 0))
	{
		auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
		Message("Failed to parse jar file: %s", err);
		m_valid = false;
		return;
	}

	auto count = mz_zip_reader_get_num_files(&archive);
	for (size_t i = 0; i < count; i++)
	{
		size_t size = mz_zip_reader_get_filename(&archive, i, nullptr, 0);
		if (size < 1)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
			Message("Failed to get name for jar entry %zu: %s", i, err);
			continue;
		}

		auto name = std::string(size - 1, 0);
		mz_zip_reader_get_filename(&archive, i, name.data(), name.size() + 1);

		if (!name.ends_with(".class"))
		{
			continue;
		}

		size = 0;
		auto data = mz_zip_reader_extract_to_heap(&archive, i, &size, 0);
		if (!data)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
			Message("Failed to read file %s: %s", name.c_str(), err);
			free(data);
			continue;
		}

		auto classFile = ClassFile(std::span((uint8_t*)data, size));
		free(data);
		if (!classFile.IsValid())
		{
			Message("Failed to parse class %s", name.c_str());
			continue;
		}

		m_classes[classFile.GetString(classFile.GetThisClass())] = classFile;
	}
}
