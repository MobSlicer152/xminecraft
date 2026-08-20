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
	SSAValue result;
	std::vector<SSAValue> parents;
};

struct ControlFlowFrame
{
	std::vector<SSAValue> locals;
	std::vector<SSAValue> stack;
};

struct ControlFlowNode
{
	uint32_t id;
	
	std::span<const Instruction> instructions;
	const Instruction* terminator; // points to instructions.end()

	std::vector<ControlFlowNode*> parents;
	std::vector<ControlFlowNode*> children;

	ControlFlowFrame entryFrame;
	ControlFlowFrame exitFrame;

	std::vector<Phi> phis;
};

class ControlFlowGraph
{
  private:
	std::vector<ControlFlowNode> m_nodes;
	ControlFlowNode* m_root;
};

} // namespace XJVM::JIT
