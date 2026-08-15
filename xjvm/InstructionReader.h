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
	ArrayRef = 2,	// JReference
	ObjectRef = 3,	// JReference
	Immediate4 = 4, // JInt or JFloat
	Immediate8 = 8, // JLong or JDouble
};

/// <summary>
/// Represents an operand on the operand stack
/// </summary>
struct Operand
{
	OperandType type;
	union {
		uint32_t indexVal; // LocalIndex
		JReference refVal; // ArrayRef or ObjectRef
		JInt intVal;	   // Immediate4
		JLong longVal;	   // Immediate8
		JFloat floatVal;   // Immediate4
		JDouble doubleVal; // Immediate8
	};
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
	/// <param name="offset">The offset into the bytecode the instruction is at</param>
	/// <param name="opcode">The opcode of the instruction</param>
	/// <param name="operands">The current operand stack (managed by reader)</param>
	/// <param name="data">Any bytes in the instruction after the opcode</param>
	/// <returns>Whether the instruction was processed successfully</returns>
	virtual bool ProcessInstruction(uint32_t offset, Opcode opcode, const std::stack<Operand>& operands,
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
	/// <param name="offset">The offset of the instruction</param>
	/// <param name="operands">The current operand stack</param>
	/// <param name="processor">Instruction processor to call</param>
	/// <returns>The length of the instruction, or UINT32_MAX on failure</returns>
	uint32_t VisitInstruction(uint32_t offset, std::stack<Operand>& operands, IInstructionProcessor* processor);
};

} // namespace XJVM
