#pragma once

#include "../InstructionReader.h"
#include "SSA.h"
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
	const Instruction* terminator; // points to instructions.end()

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
	/// <param name="instructions">The instructions to analyze</param>
	/// <returns>True if successful, false if graph cannot be built (should never happen on compiler-generated code)</returns>
	bool Build(std::span<const Instruction> instructions);

  private:
	std::vector<ControlFlowNode> m_nodes;
	std::vector<SSAValue> m_ssaValues; // IDs are indexes into this
	ControlFlowNode* m_root = nullptr;
};

} // namespace XJVM::JIT
