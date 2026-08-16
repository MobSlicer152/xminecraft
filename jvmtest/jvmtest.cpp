#define NO_STRICT
#define _NO_CRT_STDIO_INLINE

#include "../xjvm/xjvm.h"
#include <print>
#include <windows.h>

class PrintInstructionProcessor: public XJVM::IInstructionProcessor
{
	// Inherited via IInstructionProcessor
	void BeginProcessing(std::span<const uint8_t> bytecode) override
	{
	}

	bool ProcessInstruction(uint32_t& offset, XJVM::Opcode opcode, XJVM::InstructionFlags flags,
							std::span<const uint8_t> data) override
	{
		std::string dataStr;
		std::for_each(data.begin(), data.end(),
					  [&](const auto val) { dataStr = std::format("{}{}{:02X}", dataStr, dataStr.empty() ? "" : " ", val); });

		std::println("{} -> {} 0x{:02X} {} <{}>", offset, XJVM::OPCODE_INFO[opcode].name, opcode, std::to_underlying(flags),
					 dataStr);
		return true;
	}
};

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

	for (const auto& [name, classFile] : jar.GetClasses())
	{
		std::println("class name is \"{}\"", classFile.GetString(classFile.GetThisClass()));
		std::println("superclass name is \"{}\"", classFile.GetString(classFile.GetSuperClass()));
		std::println("class version is {}.{}", classFile.GetMajorVersion(), classFile.GetMinorVersion());

		for (uint16_t i = 1; i < classFile.GetConstantPoolSize(); i++)
		{
			auto constant = classFile.GetConstant(i);
			// std::println("constant {} has type {} and string \"{}\"", i, (XJVM::u1)constant.tag, test.GetString(constant));
		}

		for (size_t i = 0; i < classFile.GetInterfaceCount(); i++)
		{
			std::println("interface {} is {}", i, classFile.GetInterface(i));
		}

		auto methods = classFile.GetMethods();
		for (const auto& method : methods)
		{
			std::println("{} -> {}", method.GetName(), method.GetDescriptor());
			auto code = classFile.GetCode(method);
			XJVM::InstructionReader reader(code);
			PrintInstructionProcessor proc;
			if (reader.Parse(&proc) < code.size())
			{
				std::println("FAILED TO PARSE METHOD {}", method.GetName());
			}
		}
	}

	return ERROR_SUCCESS;
}
