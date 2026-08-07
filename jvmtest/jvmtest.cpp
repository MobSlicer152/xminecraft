#define NO_STRICT
#include <print>
#include <windows.h>
#include "../xjvm/classfile.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::println("usage: {} <classfile>", argv[0]);
		return ERROR_INVALID_PARAMETER;
	}

	XJVM::ClassFile test(argv[1]);
	if (!test.IsValid())
	{
		std::println("failed to parse class {}", argv[1]);
		return ERROR_INVALID_DATA;
	}

	std::println("class name is \"{}\"", test.GetString(test.GetThisClass()));
	std::println("superclass name is \"{}\"", test.GetString(test.GetSuperClass()));
	std::println("class version is {}.{}", test.GetMajorVersion(), test.GetMinorVersion());

	for (uint16_t i = 1; i < test.GetConstantPoolSize(); i++)
	{
		auto constant = test.GetConstant(i);
		std::println("constant {} has type {} and string \"{}\"", i, (XJVM::u1)constant.tag, test.GetString(constant));
	}

	return ERROR_SUCCESS;
}
