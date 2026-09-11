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

	std::set<uint32_t> potentialNodes;
	if (!FindNodes(potentialNodes))
	{
		return false;
	}

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

bool ControlFlowGraph::FindNodes(std::set<uint32_t>& offsets)
{
	// add the start
	offsets.insert(0);

	for (uint32_t i = 0; i < m_instructions.size(); i++)
	{
		auto& instruction = m_instructions[i];
		auto addAbsoluteOffset = [&](int32_t offset) {
			offsets.insert(offset);
		};
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
			// if present, instruction after this one is a node
			if (i < m_instructions.size() - 1)
			{
				addAbsoluteOffset(m_instructions[i + 1].offset);
			}
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
	}

	return true;
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
	bounds.push_back(last.offset); // add the end of the bytecode as a terminator
	for (uint32_t i = 0; i < bounds.size() - 1; i++)
	{
		auto start = bounds[i];
		auto end = bounds[i + 1];
		
		const auto& instruction = GetInstruction(start);
		const auto& name = OPCODE_INFO[instruction.opcode].name;

		DbgMessage("potential node at %08X-%08X: %.*s", start, end, name.size(), name.data());

		auto firstInstruction = m_instructionMap[start];
		auto lastInstruction = m_instructionMap[end];
		auto size = std::clamp(lastInstruction - firstInstruction, 0u, m_instructions.size() - firstInstruction - 1);
		auto instructions = m_instructions.subspan(firstInstruction, size);

		// insert the node
		m_nodes[start] = ControlFlowNode(instructions);
	}

	return true;
}
