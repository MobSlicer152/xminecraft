/// Instruction reading

#pragma once

#include "jvmdef.h"
#include "opcodes.h"

namespace XJVM
{

/// <summary>
/// Operand stack value type
/// </summary>
enum class OperandType
{
	Unknown = 0,
	LocalIndex = 1, // uint32_t
	Reference = 2,	// JReference
	Immediate4 = 3, // JInt or JFloat
	Immediate8 = 4, // JLong or JDouble
};

/// <summary>
/// Represents an operand on the operand stack
/// </summary>
struct Operand
{
	OperandType type;
	union {
		uint32_t indexVal; // LocalIndex
		JReference refVal; // Reference
		JInt intVal;	   // Immediate4
		JLong longVal;	   // Immediate8
		JFloat floatVal;   // Immediate4
		JDouble doubleVal; // Immediate8
	};
};

enum class InstructionFlags
{
	None = 0 << 0,
	Wide = 1 << 0,
};

/// <summary>
/// Interface for instruction processors to implement
/// </summary>
class IInstructionProcessor
{
  public:
	virtual ~IInstructionProcessor() = default;

	/// <summary>
	/// Begin processing a chunk of bytecode
	/// TODO: add method info
	/// </summary>
	/// <param name="bytecode">The bytecode to process</param>
	virtual void BeginProcessing(std::span<const uint8_t> bytecode) = 0;

	/// <summary>
	/// Process the instruction at the given offset
	/// </summary>
	/// <param name="offset">The offset into the bytecode the instruction is at, may be modified if needed</param>
	/// <param name="opcode">The opcode of the instruction</param>
	/// <param name="flags">Flags for this instruction</param>
	/// <param name="data">Any bytes in the instruction after the opcode</param>
	/// <returns>Whether the instruction was processed successfully</returns>
	virtual bool ProcessInstruction(uint32_t& offset, Opcode opcode, InstructionFlags flags,
									std::span<const uint8_t> data = {}) = 0;
};

/// <summary>
/// JVM bytecode instruction reader
/// </summary>
class InstructionReader
{
  public:
	/// <summary>
	/// Read bytecode
	/// </summary>
	/// <param name="bytecode">The bytecode to read</param>
	InstructionReader(std::span<const uint8_t> bytecode);

	/// <summary>
	/// Destructor
	/// </summary>
	~InstructionReader() = default;

	/// <summary>
	/// Parse the class
	/// </summary>
	/// <param name="processor">Instruction processor to call</param>
	/// <returns>The number of bytes parsed successfully</returns>
	uint32_t Parse(IInstructionProcessor* processor = nullptr);

  private:
	std::span<const uint8_t> m_bytecode;

	/// <summary>
	/// Visit an instruction
	/// </summary>
	/// <param name="offset">The offset of the current instruction</param>
	/// <param name="processor">Instruction processor to call</param>
	/// <returns>The length of the instruction, or UINT32_MAX on failure</returns>
	uint32_t VisitInstruction(uint32_t& offset, IInstructionProcessor* processor);
};

} // namespace XJVM
