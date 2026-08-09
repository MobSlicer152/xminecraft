#define NO_STRICT
#define _NO_CRT_STDIO_INLINE

#include <print>
#include <windows.h>
#include "../xjvm/xjvm.h"

extern "C" int _vsnprintf(_Out_writes_opt_(_BufferCount) _Post_maybez_ char* const _Buffer, _In_ size_t const _BufferCount, _In_z_ _Printf_format_string_ char const* const _Format, va_list _ArgList)
{
	return _vsnprintf_s_l(_Buffer, _BufferCount, _BufferCount, _Format, nullptr, _ArgList);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::println("usage: {} <classfile>", argv[0]);
		return ERROR_INVALID_PARAMETER;
	}

	XJVM::g_msgCallback = (XJVM::MessageCallback)puts;

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

	for (size_t i = 0; i < test.GetInterfaceCount(); i++)
	{
		std::println("interface {} is {}", i, test.GetInterface(i));
	}

	return ERROR_SUCCESS;
}
 