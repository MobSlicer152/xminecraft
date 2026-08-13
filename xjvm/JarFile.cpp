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
	if (m_classes.contains(name))
	{
		return &m_classes.at(name);
	}

	return nullptr;
}

const std::unordered_map<std::string_view, ClassFile>& JarFile::GetClasses() const
{
	return m_classes;
}

void JarFile::Parse(std::span<const uint8_t> data)
{
	m_valid = true;

	// parse the archive
	mz_zip_archive archive = {};
	if (!mz_zip_reader_init_mem(&archive, data.data(), data.size(), 0))
	{
		auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
		Message("Failed to parse jar file: %s", err);
		m_valid = false;
		return;
	}

	// go through all the files
	auto count = mz_zip_reader_get_num_files(&archive);
	for (size_t i = 0; i < count; i++)
	{
		// get the filename size
		size_t size = mz_zip_reader_get_filename(&archive, i, nullptr, 0);
		if (size < 1)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
			Message("Failed to get name for jar entry %zu: %s", i, err);
			continue;
		}

		// read the filename (size is for NUL terminated, std::string automatically includes that)
		auto name = std::string(size - 1, 0);
		mz_zip_reader_get_filename(&archive, i, name.data(), size);

		// skip if not a class, manifests aren't relevant yet
		if (!name.ends_with(".class"))
		{
			continue;
		}

		// extract the class
		size = 0;
		auto data = mz_zip_reader_extract_to_heap(&archive, i, &size, 0);
		if (!data)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(&archive));
			Message("Failed to read file %s: %s", name.c_str(), err);
			free(data);
			continue;
		}

		// parse it
		auto classFile = ClassFile(std::span((uint8_t*)data, size));
		free(data);
		if (!classFile.IsValid())
		{
			Message("Failed to parse class %s", name.c_str());
			continue;
		}

		// class was valid, store it by its name
		m_classes[classFile.GetString(classFile.GetThisClass())] = classFile;
	}
}
