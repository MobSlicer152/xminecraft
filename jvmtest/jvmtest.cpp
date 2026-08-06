#include <cstdio>
#include <windows.h>
#include "../xjvm/classfile.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: %s <classfile>\n", argv[0]);
		return ERROR_INVALID_PARAMETER;
	}

	XJVM::ClassFile test(argv[1]);
	if (!test.IsValid())
	{
		printf("failed to parse class %s\n", argv[1]);
		return ERROR_INVALID_DATA;
	}

	return ERROR_SUCCESS;
}
