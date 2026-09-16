#include "..\stdafx.h"

using namespace XJVM;
using namespace XJVM::JIT;

ControlFlowGraph::ControlFlowGraph(std::span<const Instruction> instructions) : m_instructions(instructions)
{
}

bool ControlFlowGraph::Build()
{
	// map offsets to instructions
	for (uint32_t i = 0; i < m_instructions.size(); i++)
	{
		m_instructionMap[m_instructions[i].offset] = i;
	}

	// find nodes and build the graph
	auto potentialNodes = FindNodes();
	if (!BuildGraph(potentialNodes))
	{
		return false;
	}

	return true;
}

const XJVM::Instruction& ControlFlowGraph::GetInstruction(uint32_t offset) const
{
	auto instruction = m_instructionMap.at(offset);
	return m_instructions[instruction];
}

std::set<uint32_t> ControlFlowGraph::GetInstructionFlow(const Instruction& instruction)
{
	std::set<uint32_t> offsets;

	auto addAbsoluteOffset = [&](int32_t offset) { offsets.insert(offset); };
	auto addOffset = [&](int32_t offset) { addAbsoluteOffset(instruction.offset + offset); };

	switch (instruction.opcode)
	{
	case Opcodes::IFEQ:
	case Opcodes::IFNE:
	case Opcodes::IFLT:
	case Opcodes::IFGE:
	case Opcodes::IFGT:
	case Opcodes::IFLE:
	case Opcodes::IF_ICMPEQ:
	case Opcodes::IF_ICMPNE:
	case Opcodes::IF_ICMPLT:
	case Opcodes::IF_ICMPGE:
	case Opcodes::IF_ICMPGT:
	case Opcodes::IF_ICMPLE:
	case Opcodes::IF_ACMPEQ:
	case Opcodes::IF_ACMPNE:
	case Opcodes::IFNONNULL:
	case Opcodes::IFNULL:
	case Opcodes::GOTO:
	case Opcodes::JSR: {
		// target of this branch is a node
		addOffset(instruction.GetOperand<int16_t>());
		break;
	}
	case Opcodes::TABLESWITCH: {
		auto defaultOffset = instruction.GetOperand<int32_t>(0);
		auto lowIdx = instruction.GetOperand<int32_t>(4);
		auto highIdx = instruction.GetOperand<int32_t>(8);
		auto count = highIdx - lowIdx + 1;

		// target for default branch
		addOffset(defaultOffset);
		for (int32_t i = 0; i < count; i++)
		{
			// other offsets
			addOffset(instruction.GetOperand<int32_t>((3 + i) * 4));
		}
		break;
	}
	case Opcodes::LOOKUPSWITCH: {
		auto defaultOffset = instruction.GetOperand<int32_t>(0);
		auto count = instruction.GetOperand<int32_t>(4);

		// target for default branch
		addOffset(defaultOffset);
		for (int32_t i = 0; i < count; i++)
		{
			// add offsets, but skip match values
			addOffset(instruction.GetOperand<int32_t>((2 + i * 2 + 1) * 4));
		}
		break;
	}
	// TODO: wide stuff
	default: {
		break;
	}
	}

	return offsets;
}

std::set<uint32_t> ControlFlowGraph::FindNodes()
{
	std::set<uint32_t> offsets;

	// add the start
	offsets.insert(0);

	// add offsets from instructions
	for (uint32_t i = 0; i < m_instructions.size(); i++)
	{
		const auto& instruction = m_instructions[i];
		// things that arent branches are irrelevant
		if (IsBranch(instruction.opcode))
		{
			offsets.insert_range(GetInstructionFlow(instruction));
		}

		// instructions after a branch or terminator form nodes
		if ((IsBranch(instruction.opcode) || IsTerminator(instruction.opcode)) && i < m_instructions.size() - 1)
		{
			offsets.insert(m_instructions[i + 1].offset);
		}
	}

	return offsets;
}

bool ControlFlowGraph::BuildGraph(const std::set<uint32_t>& offsets)
{
	// if empty or just one instruction, just bail
	if (offsets.size() < 2)
	{
		return true;
	}

	// build node locations
	auto bounds = std::vector(offsets.begin(), offsets.end());
	const auto& last = m_instructions[m_instructions.size() - 1];
	bounds.push_back(last.offset + last.size); // add the end of the bytecode as a terminator
	for (uint32_t i = 0; i < bounds.size() - 1; i++)
	{
		auto start = bounds[i];
		auto end = bounds[i + 1];

		const auto& instruction = GetInstruction(start);
		const auto& name = OPCODE_INFO[instruction.opcode].name;

		// get start offset
		auto firstInstruction = m_instructionMap.at(start);

		// get end offset, clamped to the end of the list
		auto lastIt = m_instructionMap.lower_bound(end);
		auto lastInstruction = lastIt == m_instructionMap.end() ? m_instructions.size() : lastIt->second;

		// get the instructions
		auto instructions = m_instructions.subspan(firstInstruction, lastInstruction - firstInstruction);

		// insert the node
		m_nodes[start] = ControlFlowNode(instructions);
	}

	// connect nodes
	for (auto& node : m_nodes) {}

	return true;
}
