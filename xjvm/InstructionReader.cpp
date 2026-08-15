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

	// helper to shorten calls to the instruction processor
	auto process = [&](uint16_t size) -> uint32_t {
		// bounds check
		if (here.size() < size - 1)
		{
			Message("Instruction at offset %u truncated, expected %hu bytes but got %zu", offset, size, here.size());
			return UINT32_MAX;
		}

		// call instruction processor if given
		if (processor)
		{
			processor->ProcessInstruction(offset, opcode, here.subspan(1, size - 1));
		}

		// simplifies switch labels
		return size;
	};

#define PARSE(opcode)                                      \
	case Opcodes::opcode:                                  \
		return process(OPCODE_INFO[Opcodes::opcode].size);

	// parse
	switch (opcode)
	{
		PARSE(NOP, 0x00, 1)
		PARSE(ACONST_NULL, 0x01, 1)
		PARSE(ICONST_M1, 0x02, 1)
		PARSE(ICONST_0, 0x03, 1)
		PARSE(ICONST_1, 0x04, 1)
		PARSE(ICONST_2, 0x05, 1)
		PARSE(ICONST_3, 0x06, 1)
		PARSE(ICONST_4, 0x07, 1)
		PARSE(ICONST_5, 0x08, 1)
		PARSE(LCONST_0, 0x09, 1)
		PARSE(LCONST_1, 0x0A, 1)
		PARSE(FCONST_0, 0x0B, 1)
		PARSE(FCONST_1, 0x0C, 1)
		PARSE(FCONST_2, 0x0D, 1)
		PARSE(DCONST_0, 0x0E, 1)
		PARSE(DCONST_1, 0x0F, 1)
		PARSE(BIPUSH, 0x10, 2)
		PARSE(SIPUSH, 0x11, 2)
		PARSE(LDC, 0x12, 2)
		PARSE(LDC_W, 0x13, 3)
		PARSE(LDC2_W, 0x14, 3)
		PARSE(ILOAD, 0x15, 2)
		PARSE(LLOAD, 0x16, 2)
		PARSE(FLOAD, 0x17, 2)
		PARSE(DLOAD, 0x18, 2)
		PARSE(ALOAD, 0x19, 2)
		PARSE(ILOAD_0, 0x1A, 1)
		PARSE(ILOAD_1, 0x1B, 1)
		PARSE(ILOAD_2, 0x1C, 1)
		PARSE(ILOAD_3, 0x1D, 1)
		PARSE(LLOAD_0, 0x1E, 1)
		PARSE(LLOAD_1, 0x1F, 1)
		PARSE(LLOAD_2, 0x20, 1)
		PARSE(LLOAD_3, 0x21, 1)
		PARSE(FLOAD_0, 0x22, 1)
		PARSE(FLOAD_1, 0x23, 1)
		PARSE(FLOAD_2, 0x24, 1)
		PARSE(FLOAD_3, 0x25, 1)
		PARSE(DLOAD_0, 0x26, 1)
		PARSE(DLOAD_1, 0x27, 1)
		PARSE(DLOAD_2, 0x28, 1)
		PARSE(DLOAD_3, 0x29, 1)
		PARSE(ALOAD_0, 0x2A, 1)
		PARSE(ALOAD_1, 0x2B, 1)
		PARSE(ALOAD_2, 0x2C, 1)
		PARSE(ALOAD_3, 0x2D, 1)
		PARSE(IALOAD, 0x2E, 1)
		PARSE(LALOAD, 0x2F, 1)
		PARSE(FALOAD, 0x30, 1)
		PARSE(DALOAD, 0x31, 1)
		PARSE(AALOAD, 0x32, 1)
		PARSE(BALOAD, 0x33, 1)
		PARSE(CALOAD, 0x34, 1)
		PARSE(SALOAD, 0x35, 1)
		PARSE(ISTORE, 0x36, 2)
		PARSE(LSTORE, 0x37, 2)
		PARSE(FSTORE, 0x38, 2)
		PARSE(DSTORE, 0x39, 2)
		PARSE(ASTORE, 0x3A, 2)
		PARSE(ISTORE_0, 0x3B, 1)
		PARSE(ISTORE_1, 0x3C, 1)
		PARSE(ISTORE_2, 0x3D, 1)
		PARSE(ISTORE_3, 0x3E, 1)
		PARSE(LSTORE_0, 0x3F, 1)
		PARSE(LSTORE_1, 0x40, 1)
		PARSE(LSTORE_2, 0x41, 1)
		PARSE(LSTORE_3, 0x42, 1)
		PARSE(FSTORE_0, 0x43, 1)
		PARSE(FSTORE_1, 0x44, 1)
		PARSE(FSTORE_2, 0x45, 1)
		PARSE(FSTORE_3, 0x46, 1)
		PARSE(DSTORE_0, 0x47, 1)
		PARSE(DSTORE_1, 0x48, 1)
		PARSE(DSTORE_2, 0x49, 1)
		PARSE(DSTORE_3, 0x4A, 1)
		PARSE(ASTORE_0, 0x4B, 1)
		PARSE(ASTORE_1, 0x4C, 1)
		PARSE(ASTORE_2, 0x4D, 1)
		PARSE(ASTORE_3, 0x4E, 1)
		PARSE(IASTORE, 0x4F, 1)
		PARSE(LASTORE, 0x50, 1)
		PARSE(FASTORE, 0x51, 1)
		PARSE(DASTORE, 0x52, 1)
		PARSE(AASTORE, 0x53, 1)
		PARSE(BASTORE, 0x54, 1)
		PARSE(CASTORE, 0x55, 1)
		PARSE(SASTORE, 0x56, 1)
		PARSE(POP, 0x57, 1)
		PARSE(POP2, 0x58, 1)
		PARSE(DUP, 0x59, 1)
		PARSE(DUP_X1, 0x5A, 1)
		PARSE(DUP_X2, 0x5B, 1)
		PARSE(DUP2, 0x5C, 1)
		PARSE(DUP2_X1, 0x5D, 1)
		PARSE(DUP2_X2, 0x5E, 1)
		PARSE(SWAP, 0x5F, 1)
		PARSE(IADD, 0x60, 1)
		PARSE(LADD, 0x61, 1)
		PARSE(FADD, 0x62, 1)
		PARSE(DADD, 0x63, 1)
		PARSE(ISUB, 0x64, 1)
		PARSE(LSUB, 0x65, 1)
		PARSE(FSUB, 0x66, 1)
		PARSE(DSUB, 0x67, 1)
		PARSE(IMUL, 0x68, 1)
		PARSE(LMUL, 0x69, 1)
		PARSE(FMUL, 0x6A, 1)
		PARSE(DMUL, 0x6B, 1)
		PARSE(IDIV, 0x6C, 1)
		PARSE(LDIV, 0x6D, 1)
		PARSE(FDIV, 0x6E, 1)
		PARSE(DDIV, 0x6F, 1)
		PARSE(IREM, 0x70, 1)
		PARSE(LREM, 0x71, 1)
		PARSE(FREM, 0x72, 1)
		PARSE(DREM, 0x73, 1)
		PARSE(INEG, 0x74, 1)
		PARSE(LNEG, 0x75, 1)
		PARSE(FNEG, 0x76, 1)
		PARSE(DNEG, 0x77, 1)
		PARSE(ISHL, 0x78, 1)
		PARSE(LSHL, 0x79, 1)
		PARSE(ISHR, 0x7A, 1)
		PARSE(LSHR, 0x7B, 1)
		PARSE(IUSHR, 0x7C, 1)
		PARSE(LUSHR, 0x7D, 1)
		PARSE(IAND, 0x7E, 1)
		PARSE(LAND, 0x7F, 1)
		PARSE(IOR, 0x80, 1)
		PARSE(LOR, 0x81, 1)
		PARSE(IXOR, 0x82, 1)
		PARSE(LXOR, 0x83, 1)
		PARSE(IINC, 0x84, 3)
		PARSE(I2L, 0x85, 1)
		PARSE(I2F, 0x86, 1)
		PARSE(I2D, 0x87, 1)
		PARSE(L2I, 0x88, 1)
		PARSE(L2F, 0x89, 1)
		PARSE(L2D, 0x8A, 1)
		PARSE(F2I, 0x8B, 1)
		PARSE(F2L, 0x8C, 1)
		PARSE(F2D, 0x8D, 1)
		PARSE(D2I, 0x8E, 1)
		PARSE(D2L, 0x8F, 1)
		PARSE(D2F, 0x90, 1)
		PARSE(I2B, 0x91, 1)
		PARSE(I2C, 0x92, 1)
		PARSE(I2S, 0x93, 1)
		PARSE(LCMP, 0x94, 1)
		PARSE(FCMPL, 0x95, 1)
		PARSE(FCMPG, 0x96, 1)
		PARSE(DCMPL, 0x97, 1)
		PARSE(DCMPG, 0x98, 1)
		PARSE(IFEQ, 0x99, 3)
		PARSE(IFNE, 0x9A, 3)
		PARSE(IFLT, 0x9B, 3)
		PARSE(IFGE, 0x9C, 3)
		PARSE(IFGT, 0x9D, 3)
		PARSE(IFLE, 0x9E, 3)
		PARSE(IF_ICMPEQ, 0x9F, 3)
		PARSE(IF_ICMPNE, 0xA0, 3)
		PARSE(IF_ICMPLT, 0xA1, 3)
		PARSE(IF_ICMPGE, 0xA2, 3)
		PARSE(IF_ICMPGT, 0xA3, 3)
		PARSE(IF_ICMPLE, 0xA4, 3)
		PARSE(IF_ACMPEQ, 0xA5, 3)
		PARSE(IF_ACMPNE, 0xA6, 3)
		PARSE(GOTO, 0xA7, 3)
		PARSE(JSR, 0xA8, 3)
		PARSE(RET, 0xA9, 2)
		PARSE(IRETURN, 0xAC, 1)
		PARSE(LRETURN, 0xAD, 1)
		PARSE(FRETURN, 0xAE, 1)
		PARSE(DRETURN, 0xAF, 1)
		PARSE(ARETURN, 0xB0, 1)
		PARSE(RETURN, 0xB1, 1)
		PARSE(GETSTATIC, 0xB2, 3)
		PARSE(PUTSTATIC, 0xB3, 3)
		PARSE(GETFIELD, 0xB4, 3)
		PARSE(PUTFIELD, 0xB5, 3)
		PARSE(INVOKEVIRTUAL, 0xB6, 3)
		PARSE(INVOKESPECIAL, 0xB7, 3)
		PARSE(INVOKESTATIC, 0xB8, 3)
		PARSE(INVOKEINTERFACE, 0xB9, 5)
		PARSE(NEW, 0xBB, 3)
		PARSE(NEWARRAY, 0xBC, 2)
		PARSE(ANEWARRAY, 0xBD, 3)
		PARSE(ARRAYLENGTH, 0xBE, 1)
		PARSE(ATHROW, 0xBF, 1)
		PARSE(CHECKCAST, 0xC0, 3)
		PARSE(INSTANCEOF, 0xC1, 3)
		PARSE(MONITORENTER, 0xC2, 1)
		PARSE(MONITOREXIT, 0xC3, 1)
		PARSE(MULTIANEWARRAY, 0xC5, 4)
		PARSE(IFNULL, 0xC6, 3)
		PARSE(IFNONNULL, 0xC7, 3)
		PARSE(GOTO_W, 0xC8, 5)
		PARSE(JSR_W, 0xC9, 5)
		PARSE(BREAKPOINT, 0xCA, 1)

	default: {
		Message("Unknown opcode 0x%02X", opcode);
		return UINT32_MAX;
	}
	}
}
