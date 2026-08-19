/// Information about JVM bytecode instructions

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
	uint8_t wideSize;
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
#define XJVM_OPCODE(name, val, size, ...) {#name, size, __VA_ARGS__},
#include "opcodes.inc"
#undef XJVM_OPCODE
};

} // namespace XJVM
