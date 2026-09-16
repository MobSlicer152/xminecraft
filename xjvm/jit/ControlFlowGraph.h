#pragma once

#include "../InstructionReader.h"
#include "SSA.h"
#include <algorithm>
#include <map>
#include <set>
#include <vector>

namespace XJVM::JIT
{

/// <summary>
/// Represents a frame in the CFG
/// </summary>
struct ControlFlowFrame
{
	std::vector<SSAID> locals;
	std::vector<SSAID> stack;
};

/// <summary>
/// Represents a node in the CFG
/// </summary>
struct ControlFlowNode
{
	std::span<const Instruction> instructions;

	std::set<ControlFlowNode*> parents;
	std::set<ControlFlowNode*> children;

	ControlFlowFrame entryFrame;
	ControlFlowFrame exitFrame;

	std::set<Phi> phis;
};

/// <summary>
/// Constructed control flow graph from bytecode
/// </summary>
class ControlFlowGraph
{
  public:
	ControlFlowGraph() = default;
	~ControlFlowGraph() = default;

	/// <summary>
	/// Store instructions
	/// </summary>
	/// <param name="instructions">Instructions to analyze</param>
	ControlFlowGraph(std::span<const Instruction> instructions);

	/// <summary>
	/// No copies allowed
	/// </summary>
	ControlFlowGraph(const ControlFlowGraph&) = delete;

	/// <summary>
	/// Move constructor still allowed
	/// </summary>
	ControlFlowGraph(ControlFlowGraph&&) = default;

	/// <summary>
	/// Build a control flow graph from the given instructions
	/// </summary>
	/// <returns>True if successful, false if graph cannot be built (should never happen on compiler-generated code)</returns>
	bool Build();

  private:
	/// <summary>
	/// Branch opcodes
	/// </summary>
	static constexpr std::array<Opcode, 20> BRANCH_OPCODES = {
		Opcodes::IFEQ,		Opcodes::IFNE,		Opcodes::IFLT,		Opcodes::IFGE,		  Opcodes::IFGT,
		Opcodes::IFLE,		Opcodes::IF_ICMPEQ, Opcodes::IF_ICMPNE, Opcodes::IF_ICMPLT,	  Opcodes::IF_ICMPGE,
		Opcodes::IF_ICMPGT, Opcodes::IF_ICMPLE, Opcodes::IF_ACMPEQ, Opcodes::IF_ACMPNE,	  Opcodes::IFNONNULL,
		Opcodes::IFNULL,	Opcodes::GOTO,		Opcodes::JSR,		Opcodes::TABLESWITCH, Opcodes::LOOKUPSWITCH};

	/// <summary>
	/// Branch opcodes that don't fall through
	/// </summary>
	static constexpr std::array<Opcode, 4> NON_FALLTHROUGH_BRANCH_OPCODES = {Opcodes::GOTO, Opcodes::JSR, Opcodes::TABLESWITCH,
																			 Opcodes::LOOKUPSWITCH};

	/// <summary>
	/// Terminator instructions
	/// </summary>
	static constexpr std::array<Opcode, 6> TERMINATOR_OPCODES = {Opcodes::RETURN,  Opcodes::ARETURN, Opcodes::DRETURN,
																 Opcodes::FRETURN, Opcodes::IRETURN, Opcodes::LRETURN};

	std::span<const Instruction> m_instructions;
	std::map<uint32_t, uint32_t> m_instructionMap; // offset to index in m_instructions
	std::map<uint32_t, ControlFlowNode> m_nodes;   // offset to node
	std::vector<SSAValue> m_ssaValues;			   // IDs are indexes into this
	ControlFlowNode* m_root = nullptr;

	/// <summary>
	/// Gets whether an opcode is a branch
	/// </summary>
	static constexpr bool IsBranch(Opcode opcode)
	{
		return std::find(BRANCH_OPCODES.cbegin(), BRANCH_OPCODES.cend(), opcode) != BRANCH_OPCODES.cend();
	}

	/// <summary>
	/// Gets whether an opcode can fall through
	/// </summary>
	static constexpr bool CanFallThrough(Opcode opcode)
	{
		return std::find(NON_FALLTHROUGH_BRANCH_OPCODES.cbegin(), NON_FALLTHROUGH_BRANCH_OPCODES.cend(), opcode) ==
			   NON_FALLTHROUGH_BRANCH_OPCODES.cend();
	}

	/// <summary>
	/// Gets whether an opcode is a terminator
	/// </summary>
	static constexpr bool IsTerminator(Opcode opcode)
	{
		return std::find(TERMINATOR_OPCODES.cbegin(), TERMINATOR_OPCODES.cend(), opcode) != TERMINATOR_OPCODES.cend();
	}

	/// <summary>
	/// Get the instruction at the given offset
	/// </summary>
	/// <param name="offset">The offset</param>
	/// <returns>The instruction</returns>
	const Instruction& GetInstruction(uint32_t offset) const;

	/// <summary>
	/// Find the offsets where an instruction can jump
	/// </summary>
	/// <param name="instruction">The instruction to analyze</param>
	/// <returns>The offsets that were found</returns>
	std::set<uint32_t> GetInstructionFlow(const Instruction& instruction);

	/// <summary>
	/// Find offsets where control goes
	/// </summary>
	/// <returns>The offsets that were found</returns>
	std::set<uint32_t> FindNodes();

	/// <summary>
	/// Build a CFG/SSA graph
	/// </summary>
	/// <param name="offsets"></param>
	/// <returns></returns>
	bool BuildGraph(const std::set<uint32_t>& offsets);
};

} // namespace XJVM::JIT
