/// Yes, as a matter of fact, I did hand type these from the spec. This is because I am not smart.
/// TODO: some of this needs refactoring to actually be useful, but the values are enough for now.

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
	std::string_view name;
	uint8_t size;
};

/// <summary>
/// JVM bytecode instructions
/// </summary>
namespace Opcodes
{

#define XJVM_OPCODE(name, val, ...) static constexpr Opcode name = (val);
#include "opcodes.inc"
#undef XJVM_OPCODE

} // namespace Opcodes

/// <summary>
/// Opcode info table
/// NOTE: opcodes.inc MUST stay sorted by value for this to be correct
/// </summary>
static constexpr OpcodeInfo OPCODE_INFO[] = {
#define XJVM_OPCODE(name, val, size) {#name, size},
#include "opcodes.inc"
#undef XJVM_OPCODE
};

} // namespace XJVM
