/// Instruction reading

#pragma once

#ifdef XJVM_ENABLE_INSTRUCTION_PRINTER
#include <algorithm>
#include <format>
#include <print>
#include <string>
#endif
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

/// <summary>
/// Flags for an instruction
/// </summary>
enum class InstructionFlags : uint8_t
{
	None = 0 << 0,
	Wide = 1 << 0,
};

/// <summary>
/// A parsed instruction, references data from the bytecode it was parsed from
/// </summary>
struct Instruction
{
	uint32_t offset;
	Opcode opcode;
	InstructionFlags flags;
	std::span<const uint8_t> operands;

	/// <summary>
	/// Get the instruction's operand
	/// </summary>
	/// <param name="offset">Optional offset into operand data</param>
	/// <returns>The value</returns>
	template <typename T> T GetOperand(uint16_t offset = 0)
	{
		XJVM_ASSERT(offset + sizeof(T) <= operands.size());
		return *(T*)&operands[offset];
	}
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
	/// <returns>Whether the instruction was processed successfully</returns>
	virtual bool ProcessInstruction(const Instruction& instruction) = 0;
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

	/// <summary>
	/// Run a processor on already parsed instructions
	/// </summary>
	/// <param name="processor">Instruction processor to call</param>
	void Scan(IInstructionProcessor* processor) const;

	/// <summary>
	/// Get the parsed instructions
	/// </summary>
	/// <returns>The instructions</returns>
	const std::vector<Instruction>& GetInstructions() const;

  private:
	std::span<const uint8_t> m_bytecode;
	std::vector<Instruction> m_instructions;

	/// <summary>
	/// Visit an instruction
	/// </summary>
	/// <param name="offset">The offset of the current instruction</param>
	/// <param name="processor">Instruction processor to call</param>
	/// <returns>The length of the instruction, or UINT32_MAX on failure</returns>
	uint32_t VisitInstruction(uint32_t offset, IInstructionProcessor* processor);
};

#ifdef XJVM_ENABLE_INSTRUCTION_PRINTER
class PrintInstructionProcessor: public IInstructionProcessor
{
	// Inherited via IInstructionProcessor
	void BeginProcessing(std::span<const uint8_t> bytecode) override
	{
	}

	bool ProcessInstruction(const Instruction& instruction) override
	{
		std::string dataStr;
		std::for_each(instruction.operands.begin(), instruction.operands.end(),
					  [&](const auto val) { dataStr = std::format("{}{}{:02X}", dataStr, dataStr.empty() ? "" : " ", val); });

		std::println("{:08X}  {:02X} {:08b}\t{} <{}>", instruction.offset, instruction.opcode,
					 std::to_underlying(instruction.flags),
					 OPCODE_INFO[instruction.opcode].name,
					 dataStr);
		return true;
	}
};
#endif

} // namespace XJVM
