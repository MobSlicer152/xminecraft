#include "stdafx.h"

using namespace XJVM;

InstructionReader::InstructionReader(std::span<const uint8_t> bytecode) : m_bytecode(bytecode)
{
}

uint32_t InstructionReader::Parse(IInstructionProcessor* processor /*= nullptr*/)
{
	// go until done
	uint32_t parsed = 0;
	uint32_t offset = 0;
	while (parsed < m_bytecode.size())
	{
		// visit the instruction at the current offset
		auto origOffset = offset;
		auto instructionSize = VisitInstruction(offset, processor);
		if (instructionSize == UINT32_MAX) // stop if invalid
		{
			Message("Bytecode parsing failed due to invalid instruction at offset %u", offset);
			break;
		}

		// if no jump happened, go to next instruction
		if (origOffset == offset)
		{
			offset += instructionSize;
		}

		// add to the total
		parsed += instructionSize;
	}

	return parsed;
}

uint32_t InstructionReader::VisitInstruction(uint32_t& offset, IInstructionProcessor* processor)
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
	InstructionFlags flags = InstructionFlags::None;

	// helper to shorten calls to the instruction processor
	auto process = [&](uint16_t size) -> uint32_t {
		// bounds check
		if (here.size() < (size_t)(size - 1))
		{
			Message("Instruction at offset %u truncated, expected %hu bytes but got %zu", offset, size, here.size());
			return UINT32_MAX;
		}

		// call instruction processor if given
		if (processor)
		{
			processor->ProcessInstruction(offset, opcode, flags, here.subspan(1, size - 1));
		}

		// simplifies switch labels
		return size;
	};

#define PARSE_COMMON(opcode, sizeName)                         \
	case Opcodes::opcode: {                                    \
		return process(OPCODE_INFO[Opcodes::opcode].sizeName); \
	}

#define PARSE(opcode)	   PARSE_COMMON(opcode, size)
#define PARSE_WIDE(opcode) PARSE_COMMON(opcode, wideSize)

	// parse
	switch (opcode)
	{
		PARSE(NOP)
		PARSE(ACONST_NULL)
		PARSE(ICONST_M1)
		PARSE(ICONST_0)
		PARSE(ICONST_1)
		PARSE(ICONST_2)
		PARSE(ICONST_3)
		PARSE(ICONST_4)
		PARSE(ICONST_5)
		PARSE(LCONST_0)
		PARSE(LCONST_1)
		PARSE(FCONST_0)
		PARSE(FCONST_1)
		PARSE(FCONST_2)
		PARSE(DCONST_0)
		PARSE(DCONST_1)
		PARSE(BIPUSH)
		PARSE(SIPUSH)
		PARSE(LDC)
		PARSE(LDC_W)
		PARSE(LDC2_W)
		PARSE(ILOAD)
		PARSE(LLOAD)
		PARSE(FLOAD)
		PARSE(DLOAD)
		PARSE(ALOAD)
		PARSE(ILOAD_0)
		PARSE(ILOAD_1)
		PARSE(ILOAD_2)
		PARSE(ILOAD_3)
		PARSE(LLOAD_0)
		PARSE(LLOAD_1)
		PARSE(LLOAD_2)
		PARSE(LLOAD_3)
		PARSE(FLOAD_0)
		PARSE(FLOAD_1)
		PARSE(FLOAD_2)
		PARSE(FLOAD_3)
		PARSE(DLOAD_0)
		PARSE(DLOAD_1)
		PARSE(DLOAD_2)
		PARSE(DLOAD_3)
		PARSE(ALOAD_0)
		PARSE(ALOAD_1)
		PARSE(ALOAD_2)
		PARSE(ALOAD_3)
		PARSE(IALOAD)
		PARSE(LALOAD)
		PARSE(FALOAD)
		PARSE(DALOAD)
		PARSE(AALOAD)
		PARSE(BALOAD)
		PARSE(CALOAD)
		PARSE(SALOAD)
		PARSE(ISTORE)
		PARSE(LSTORE)
		PARSE(FSTORE)
		PARSE(DSTORE)
		PARSE(ASTORE)
		PARSE(ISTORE_0)
		PARSE(ISTORE_1)
		PARSE(ISTORE_2)
		PARSE(ISTORE_3)
		PARSE(LSTORE_0)
		PARSE(LSTORE_1)
		PARSE(LSTORE_2)
		PARSE(LSTORE_3)
		PARSE(FSTORE_0)
		PARSE(FSTORE_1)
		PARSE(FSTORE_2)
		PARSE(FSTORE_3)
		PARSE(DSTORE_0)
		PARSE(DSTORE_1)
		PARSE(DSTORE_2)
		PARSE(DSTORE_3)
		PARSE(ASTORE_0)
		PARSE(ASTORE_1)
		PARSE(ASTORE_2)
		PARSE(ASTORE_3)
		PARSE(IASTORE)
		PARSE(LASTORE)
		PARSE(FASTORE)
		PARSE(DASTORE)
		PARSE(AASTORE)
		PARSE(BASTORE)
		PARSE(CASTORE)
		PARSE(SASTORE)
		PARSE(POP)
		PARSE(POP2)
		PARSE(DUP)
		PARSE(DUP_X1)
		PARSE(DUP_X2)
		PARSE(DUP2)
		PARSE(DUP2_X1)
		PARSE(DUP2_X2)
		PARSE(SWAP)
		PARSE(IADD)
		PARSE(LADD)
		PARSE(FADD)
		PARSE(DADD)
		PARSE(ISUB)
		PARSE(LSUB)
		PARSE(FSUB)
		PARSE(DSUB)
		PARSE(IMUL)
		PARSE(LMUL)
		PARSE(FMUL)
		PARSE(DMUL)
		PARSE(IDIV)
		PARSE(LDIV)
		PARSE(FDIV)
		PARSE(DDIV)
		PARSE(IREM)
		PARSE(LREM)
		PARSE(FREM)
		PARSE(DREM)
		PARSE(INEG)
		PARSE(LNEG)
		PARSE(FNEG)
		PARSE(DNEG)
		PARSE(ISHL)
		PARSE(LSHL)
		PARSE(ISHR)
		PARSE(LSHR)
		PARSE(IUSHR)
		PARSE(LUSHR)
		PARSE(IAND)
		PARSE(LAND)
		PARSE(IOR)
		PARSE(LOR)
		PARSE(IXOR)
		PARSE(LXOR)
		PARSE(IINC)
		PARSE(I2L)
		PARSE(I2F)
		PARSE(I2D)
		PARSE(L2I)
		PARSE(L2F)
		PARSE(L2D)
		PARSE(F2I)
		PARSE(F2L)
		PARSE(F2D)
		PARSE(D2I)
		PARSE(D2L)
		PARSE(D2F)
		PARSE(I2B)
		PARSE(I2C)
		PARSE(I2S)
		PARSE(LCMP)
		PARSE(FCMPL)
		PARSE(FCMPG)
		PARSE(DCMPL)
		PARSE(DCMPG)
		PARSE(IFEQ)
		PARSE(IFNE)
		PARSE(IFLT)
		PARSE(IFGE)
		PARSE(IFGT)
		PARSE(IFLE)
		PARSE(IF_ICMPEQ)
		PARSE(IF_ICMPNE)
		PARSE(IF_ICMPLT)
		PARSE(IF_ICMPGE)
		PARSE(IF_ICMPGT)
		PARSE(IF_ICMPLE)
		PARSE(IF_ACMPEQ)
		PARSE(IF_ACMPNE)
		PARSE(GOTO)
		PARSE(JSR)
		PARSE(RET)
		PARSE(IRETURN)
		PARSE(LRETURN)
		PARSE(FRETURN)
		PARSE(DRETURN)
		PARSE(ARETURN)
		PARSE(RETURN)
		PARSE(GETSTATIC)
		PARSE(PUTSTATIC)
		PARSE(GETFIELD)
		PARSE(PUTFIELD)
		PARSE(INVOKEVIRTUAL)
		PARSE(INVOKESPECIAL)
		PARSE(INVOKESTATIC)
		PARSE(INVOKEINTERFACE)
		PARSE(NEW)
		PARSE(NEWARRAY)
		PARSE(ANEWARRAY)
		PARSE(ARRAYLENGTH)
		PARSE(ATHROW)
		PARSE(CHECKCAST)
		PARSE(INSTANCEOF)
		PARSE(MONITORENTER)
		PARSE(MONITOREXIT)
		PARSE(MULTIANEWARRAY)
		PARSE(IFNULL)
		PARSE(IFNONNULL)
		PARSE(GOTO_W)
		PARSE(JSR_W)
		PARSE(BREAKPOINT)

	case Opcodes::TABLESWITCH: {
		// up to 3 bytes of padding
		size_t size = 1;
		size += Align(offset + size, 4u) - offset;

		// read values so the size of the table can be calculated
		auto defaultIdx = ReadNextValue<uint32_t>(here, size);
		auto lowIdx = ReadNextValue<uint32_t>(here, size);
		auto highIdx = ReadNextValue<uint32_t>(here, size);
		auto count = highIdx - lowIdx + 1;
		size += count * 4;

		// process
		return process((uint16_t)size);
	}

	case Opcodes::LOOKUPSWITCH: {
		// up to 3 bytes of padding
		size_t size = 1;
		size += Align(offset + size, 4u) - offset;

		// read values so the size of the table can be calculated
		auto defaultIdx = ReadNextValue<uint32_t>(here, size);
		auto npairs = ReadNextValue<uint32_t>(here, size);
		size += npairs * 2 * 4;

		// process
		return process((uint16_t)size);
	}

	case Opcodes::WIDE: {
		// advance things
		offset++;
		here = here.subspan(1);
		opcode = here[0];

		// set the flag
		flags |= InstructionFlags::Wide;
		
		// handle the instruction after
		switch (opcode)
		{
			PARSE_WIDE(ILOAD)
			PARSE_WIDE(LLOAD)
			PARSE_WIDE(FLOAD)
			PARSE_WIDE(DLOAD)
			PARSE_WIDE(ALOAD)
			PARSE_WIDE(ISTORE)
			PARSE_WIDE(LSTORE)
			PARSE_WIDE(FSTORE)
			PARSE_WIDE(DSTORE)
			PARSE_WIDE(ASTORE)
			PARSE_WIDE(RET)
			PARSE_WIDE(IINC)
		default: {
			Message("Opcode 0x%02X is undefined after WIDE", opcode);
			return UINT32_MAX;
		}
		}
	}

	default: {
		Message("Unknown opcode 0x%02X", opcode);
		return UINT32_MAX;
	}
	}
}
