#include "stdafx.h"

using namespace XJVM;
using namespace XJVM::JIT;

bool ControlFlowGraph::Build(std::span<const Instruction> instructions)
{
	// store the instructions, and a map of them by offset
	m_instructions = instructions;
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
	return m_instructions[m_instructionMap.at(offset)];
}

bool ControlFlowGraph::FindNodes(std::set<uint32_t>& offsets)
{

	for (uint32_t i = 0; i < m_instructions.size(); i++)
	{
		auto& instruction = m_instructions[i];
		auto addOffset = [&](int32_t offset) { offsets.insert(instruction.offset + offset); };

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
				offsets.insert(m_instructions[i + 1].offset);
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
				// other offsets
				addOffset(instruction.GetOperand<int32_t>((2 + i * 2 + 0) * 4));
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
	for (auto offset : offsets)
	{
		const auto& instruction = GetInstruction(offset);
		const auto& name = OPCODE_INFO[instruction.opcode].name;
		Message("potential node at %08X: %.*s", offset, name.size(), name.data());
	}
	return true;
}
