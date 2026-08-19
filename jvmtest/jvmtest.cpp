#define NO_STRICT
#define _NO_CRT_STDIO_INLINE
#define XJVM_ENABLE_INSTRUCTION_PRINTER

#include "../xjvm/xjvm.h"
#include <print>
#include <windows.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::println("usage: {} <classfile>", argv[0]);
		return ERROR_INVALID_PARAMETER;
	}

	XJVM::g_msgCallback = (XJVM::MessageCallback)puts;

	XJVM::JarFile jar(argv[1]);
	if (!jar.IsValid())
	{
		std::println("failed to parse jar {}", argv[1]);
		return ERROR_INVALID_DATA;
	}

	for (const auto& name : jar.GetClasses())
	{
		auto classFile = jar.GetClass(name);
		std::println("class name is \"{}\"", classFile->GetString(classFile->GetThisClass()));
		std::println("superclass name is \"{}\"", classFile->GetString(classFile->GetSuperClass()));
		std::println("class version is {}.{}", classFile->GetMajorVersion(), classFile->GetMinorVersion());

		for (uint16_t i = 1; i < classFile->GetConstantPoolSize(); i++)
		{
			auto constant = classFile->GetConstant(i);
			// std::println("constant {} has type {} and string \"{}\"", i, (XJVM::u1)constant.tag, test.GetString(constant));
		}

		for (size_t i = 0; i < classFile->GetInterfaceCount(); i++)
		{
			std::println("interface {} is {}", i, classFile->GetInterface(i));
		}

		auto methods = classFile->GetMethods();
		for (const auto& method : methods)
		{
			std::println("{} -> {}", method.GetName(), method.GetDescriptor());
			auto code = classFile->GetCode(method);
			XJVM::InstructionReader reader(code);
			XJVM::PrintInstructionProcessor proc;
			if (reader.Parse(&proc) < code.size())
			{
				std::println("FAILED TO PARSE METHOD {}", method.GetName());
			}
		}
	}

	return ERROR_SUCCESS;
}
