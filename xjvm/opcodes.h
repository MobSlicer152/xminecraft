/// Yes, as a matter of fact, I did hand type these from the spec. This is because I am not smart.

#pragma once

#include "jvmdef.h"

namespace XJVM
{

/// <summary>
/// An opcode
/// </summary>
using Opcode = uint8_t;

/// <summary>
/// Information about an opcode.
/// </summary>
struct OpcodeInfo
{
	Opcode code;
	std::string_view name;
};

/// <summary>
/// JVM bytecode instructions
/// </summary>
namespace Opcodes
{

#define XJVM_OPCODE(name, val)                                \
	static constexpr Opcode name = (val);                     \
	static constexpr OpcodeInfo name##_INFO = {name, #name};

#include "opcodes.inc"

} // namespace Opcode

} // namespace XJVM
