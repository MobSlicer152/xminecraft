#include "stdafx.h"

using namespace XJVM;

MessageCallback XJVM::g_msgCallback;

void XJVM::ReadBytes(std::span<const u1> data, size_t& offset, std::span<u1> dest)
{
	XJVM_ASSERT(dest.size() < data.size() - offset, "tried to read past end of class file data");
	memcpy(dest.data(), &data[offset], dest.size());
	offset += dest.size();
}
