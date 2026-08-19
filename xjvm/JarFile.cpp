#include "stdafx.h"

using namespace XJVM;

#define GetArchive() ((mz_zip_archive*)m_archive)

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

JarFile::~JarFile()
{
	if (m_archive)
	{
		delete GetArchive();
	}
}

bool JarFile::IsValid() const
{
	return m_valid;
}

std::optional<ClassFile> JarFile::GetClass(const std::string_view name) const
{
	auto nameStr = std::string(name);
	if (m_classes.contains(nameStr))
	{
		auto index = m_classes.at(nameStr);
		// extract the class
		size_t size = 0;
		auto data = mz_zip_reader_extract_to_heap(GetArchive(), index, &size, 0);
		if (!data)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(GetArchive()));
			Message("Failed to read file %s: %s", nameStr.c_str(), err);
			free(data);
			return {};
		}

		// parse it
		ClassFile classFile(std::span((uint8_t*)data, size));
		free(data);
		if (!classFile.IsValid())
		{
			Message("Failed to parse class %s", nameStr.c_str());
			return {};
		}

		return std::make_optional<ClassFile>(std::move(classFile));
	}

	return {};
}

void JarFile::Parse(std::span<const uint8_t> data)
{
	m_valid = true;

	// parse the archive
	m_archive = new mz_zip_archive;
	if (!mz_zip_reader_init_mem(GetArchive(), data.data(), data.size(), 0))
	{
		auto err = mz_zip_get_error_string(mz_zip_get_last_error(GetArchive()));
		Message("Failed to parse jar file: %s", err);
		m_valid = false;
		return;
	}

	// go through all the files
	auto count = mz_zip_reader_get_num_files(GetArchive());
	for (size_t i = 0; i < count; i++)
	{
		// get the filename size
		size_t size = mz_zip_reader_get_filename(GetArchive(), i, nullptr, 0);
		if (size < 1)
		{
			auto err = mz_zip_get_error_string(mz_zip_get_last_error(GetArchive()));
			Message("Failed to get name for jar entry %zu: %s", i, err);
			continue;
		}

		// read the filename (size is for NUL terminated, std::string automatically includes that)
		auto name = std::string(size - 1, 0);
		mz_zip_reader_get_filename(GetArchive(), i, name.data(), size);

		// skip if not a class, manifests aren't relevant yet
		if (!name.ends_with(".class"))
		{
			continue;
		}

		// store that name as one that's in this jar
		m_classes[name] = i;
	}
}
