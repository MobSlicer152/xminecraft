#pragma once

#include "../InstructionReader.h"
#include "SSA.h"
#include <map>
#include <set>
#include <vector>

namespace XJVM::JIT
{

/// <summary>
/// Phi for SSA, parents are ordered same as the parents of the containing node
/// </summary>
struct Phi
{
	SSAID result;
	std::vector<SSAID> parents;
};

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

	std::vector<ControlFlowNode*> parents;
	std::vector<ControlFlowNode*> children;

	ControlFlowFrame entryFrame;
	ControlFlowFrame exitFrame;

	std::vector<Phi> phis;
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
	std::span<const Instruction> m_instructions;
	std::map<uint32_t, uint32_t> m_instructionMap; // offset to index in m_instructions
	std::map<uint32_t, ControlFlowNode> m_nodes;   // offset to node
	std::vector<SSAValue> m_ssaValues;			   // IDs are indexes into this
	ControlFlowNode* m_root = nullptr;

	/// <summary>
	/// Get the instruction at the given offset
	/// </summary>
	/// <param name="offset">The offset</param>
	/// <returns>The instruction</returns>
	const Instruction& GetInstruction(uint32_t offset) const;

	bool FindNodes(std::set<uint32_t>& offsets);
	bool BuildGraph(const std::set<uint32_t>& offsets);
};

} // namespace XJVM::JIT
