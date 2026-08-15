#include "stdafx.h"

using namespace XJVM;

InstructionReader::InstructionReader(std::span<const uint8_t> bytecode)
	: m_bytecode(bytecode)
{
}

uint32_t InstructionReader::Parse(IInstructionProcessor* processor /*= nullptr*/)
{
	// go until done
	uint32_t parsed = 0;
	std::stack<Operand> operands;
	while (parsed < m_bytecode.size())
	{
		// visit the instruction at the current offset
		auto instructionSize = VisitInstruction(parsed, operands, processor);
		if (instructionSize == UINT32_MAX) // stop if invalid
		{
			Message("Bytecode parsing failed due to invalid instruction at offset %u", parsed);
			break;
		}

		// add to the total
		parsed += instructionSize;
	}

	return parsed;
}

uint32_t InstructionReader::VisitInstruction(uint32_t offset, std::stack<Operand>& operands, IInstructionProcessor* processor)
{
	// get a span that starts at the offset and check bounds
	auto here = m_bytecode.subspan(offset);
	if (here.empty())
	{
		Message("Instruction offset %u out of bounds", offset);
		return UINT32_MAX;
	}

	// get the opcode
	Opcode opcode = here[0];
	uint16_t size = 0;
	switch (opcode)
	{
	
	}
}
