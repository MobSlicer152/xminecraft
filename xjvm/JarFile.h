/// Jar file parsing

#pragma once

#include "ClassFile.h"
#include "jvmdef.h"
#include <optional>
#include <unordered_map>

namespace XJVM
{

/// <summary>
/// Jar file parser
/// </summary>
class JarFile
{
  public:
	JarFile() = default;
	~JarFile() = default;

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
	const ClassFile* GetClass(const std::string_view name) const;

	/// <summary>
	/// Get the classes in this jar
	/// </summary>
	/// <returns>The classes mapped by name</returns>
	const std::unordered_map<std::string_view, ClassFile>& GetClasses() const;

  private:
	bool m_valid = false;
	std::unordered_map<std::string_view, ClassFile> m_classes;

	/// <summary>
	/// Parse the jar file
	/// </summary>
	/// <param name="data">Jar file data</param>
	void Parse(std::span<const uint8_t> data);
};

} // namespace XJVM
