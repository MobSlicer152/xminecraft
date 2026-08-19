/// Jar file parsing

#pragma once

#include "ClassFile.h"
#include "jvmdef.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <ranges>

namespace XJVM
{

/// <summary>
/// Jar file parser
/// </summary>
class JarFile
{
  public:
	JarFile() = default;
	~JarFile();

	/// <summary>
	/// Parse a jar from disk
	/// </summary>
	/// <param name="fileName">The name of the jar file</param>
	JarFile(const char* fileName);

	/// <summary>
	/// Parse a jar in memory
	/// </summary>
	/// <param name="data">Jar file data</param>
	JarFile(std::span<const uint8_t> data);

	/// <summary>
	/// Whether the jar was successfully parsed
	/// </summary>
	bool IsValid() const;

	/// <summary>
	/// Get a class by name
	/// </summary>
	/// <param name="name">The name of the class</param>
	/// <returns>The class, if present. Otherwise nullptr.</returns>
	std::optional<ClassFile> GetClass(const std::string_view name) const;

	/// <summary>
	/// Get the class names in this jar
	/// </summary>
	/// <returns>The class names</returns>
	const auto GetClasses() const
	{
		return std::views::keys(m_classes);
	}

  private:
	bool m_valid = false;
	void* m_archive = nullptr;
	std::unordered_map<std::string, int> m_classes;

	/// <summary>
	/// Parse the jar file
	/// </summary>
	/// <param name="data">Jar file data</param>
	void Parse(std::span<const uint8_t> data);
};

} // namespace XJVM
