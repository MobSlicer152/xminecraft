/// Yes, as a matter of fact, I did hand type these from the spec. This is because I am not smart.

#pragma once

#include "jvmdef.h"

namespace XJVM
{
/// <summary>
/// JVM bytecode instructions
/// </summary>
namespace Opcode
{
struct OpcodeInfo
{
	uint8_t code;
	std::string_view name;
};

#define XJVM_OPCODE(name, val) static constexpr OpcodeInfo name = {val, #name};

#include "opcodes.inc"

} // namespace Opcode

} // namespace XJVM
