/* Capstone Disassembly Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013-2015 */
/* M68K Backend by Daniel Collin <daniel@collin.com> */

#include <stdio.h>	// DEBUG
#include <stdlib.h>
#include <string.h>

//#include "M68KRegisterInfo.h"
#include "../../cs_priv.h"
#include "../../utils.h"

#include "../../MCInst.h"
#include "../../MCInstrDesc.h"
#include "../../MCRegisterInfo.h"

#include "M68Kdasm.h"

enum {
  M68K_NoRegister,
  M68K_SR,
  M68K_PC,
  M68K_D0,
  M68K_D1,
  M68K_D2,
  M68K_D3,
  M68K_D4,
  M68K_D5,
  M68K_D6,
  M68K_D7,
  M68K_A0,
  M68K_A1,
  M68K_A2,
  M68K_A3,
  M68K_A4,
  M68K_A5,
  M68K_A6,
  M68K_A7,
  M68K_NUM_TARGET_REGS
};

// Register classes
enum {
  M68K_DRegsRegClassID = 0,
  M68K_ARRegsRegClassID = 1
};

static MCPhysReg M68KRegDiffLists[] = {
  /* 0 */ 65535, 0,
};

static uint16_t M68KSubRegIdxLists[] = {
  /* 0 */ 0,
};

static MCRegisterDesc M68KRegDesc[] = { // Descriptors
  { 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
};

  // RRegs Register Class...
  static MCPhysReg DRegs[] = {
    M68K_D0, M68K_D1, M68K_D2, M68K_D3, M68K_D4, M68K_D5, M68K_D6, M68K_D7, M68K_SR, M68K_PC,
  };

  // RRegs Bit set.
  static uint8_t DRegsBits[] = {
    0xfe, 0xff, 0x01, 
  };

  // GRRegs Register Class...
  static MCPhysReg ARRegs[] = {
    M68K_A0, M68K_A1, M68K_A2, M68K_A3, M68K_A4, M68K_A5, M68K_A6, M68K_A7,
  };

  // GRRegs Bit set.
  static uint8_t ARRegsBits[] = {
    0xe0, 0xff, 0x01, 
  };

static MCRegisterClass M68KMCRegisterClasses[] = {
  { DRegs, DRegsBits, 1, 32, sizeof(DRegsBits), M68K_DRegsRegClassID, 4, 4, 1, 0 },
  { ARRegs, ARRegsBits, 0, 32, sizeof(ARRegsBits), M68K_ARRegsRegClassID, 4, 4, 1, 1 },
};

static const char* s_reg_names[] =
{
	"invalid",
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
	"sr", "pc",
};

void M68K_init(MCRegisterInfo *MRI)
{
	MCRegisterInfo_InitMCRegisterInfo(MRI, M68KRegDesc, 17,
			0, 0,
			M68KMCRegisterClasses, 2,
			0, 0,
			M68KRegDiffLists,
			0,
			M68KSubRegIdxLists, 1,
			0);
}

static uint8_t* s_disassemblyBuffer;
static uint32_t s_baseAddress;

unsigned int m68k_read_disassembler_8(uint64_t address)
{
	address -= s_baseAddress;
	return s_disassemblyBuffer[address];
}

unsigned int m68k_read_disassembler_16(uint64_t address)
{
	address -= s_baseAddress;


	uint16_t v0 = s_disassemblyBuffer[address + 0];
	uint16_t v1 = s_disassemblyBuffer[address + 1];

	return (v0 << 8) | v1; 
}

unsigned int m68k_read_disassembler_32(uint64_t address)
{
	address -= s_baseAddress;

	uint32_t v0 = s_disassemblyBuffer[address + 0];
	uint32_t v1 = s_disassemblyBuffer[address + 1];
	uint32_t v2 = s_disassemblyBuffer[address + 2];
	uint32_t v3 = s_disassemblyBuffer[address + 3];

	return (v0 << 24) | (v1 << 16) | (v2 << 8) | v3;
}

const char* getRegName(m68k_reg reg)
{
	return s_reg_names[(int)reg];
}

void printAddressingMode(SStream* O, const cs_m68k_op* op)
{
	switch (op->address_mode)
	{
		case M68K_RD_DATA : SStream_concat(O, "d%d", (op->reg - M68K_REG_D0)); break;
		case M68K_RD_ADDRESS : SStream_concat(O, "a%d", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS : SStream_concat(O, "(a%d)", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_PI : SStream_concat(O, "(a%d)+", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_PD : SStream_concat(O, "-(a%d)", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_D : SStream_concat(O, "$%x(a%d)", op->mem.disp, (op->reg - M68K_REG_A0)); break;
		case M68K_PCI_DISP : SStream_concat(O, "$%x(pc)", op->mem.disp); break;
		case M68K_ADA_SHORT : SStream_concat(O, "$%x.w", op->imm); break; 
		case M68K_ADA_LONG : SStream_concat(O, "$%x.l", op->imm); break; 
		case M68K_IMMIDATE : SStream_concat(O, "#$%x", op->imm); break; 
		case M68K_PCIIWI_8_BIT : 
		{
			SStream_concat(O, "$%x(pc,%s)", op->mem.disp, getRegName(op->mem.index_reg)); 
			break;
		}

		case M68K_PCIIWI_BASE : 
		case M68K_ARIWI_BASE : 
		{
			if (op->mem.in_disp > 0)
				SStream_concat(O, "$%x", op->mem.in_disp);

			SStream_concat(O, "(");

			if (op->address_mode == M68K_PCIIWI_BASE) {
				SStream_concat(O, "pc,%s.%c", getRegName(op->mem.index_reg), op->mem.index_size ? 'l' : 'w');
			} else { 
				if (op->mem.base_reg != M68K_REG_INVALID)
					SStream_concat(O, "a%d,", op->mem.base_reg - M68K_REG_A0);

				SStream_concat(O, "%s.%c", getRegName(op->mem.index_reg), op->mem.index_size ? 'l' : 'w');
			}

			if (op->mem.scale > 0)
				SStream_concat(O, "*%d)", op->mem.scale);
			else
				SStream_concat(O, ")", op->mem.scale);

			break;
		}

		// It's ok to just use PCMI here as is as we set base_reg to PC in the disassembler. While this is not strictly correct it makes the code
		// easier and that is what actually happens when the code is executed anyway.

		case M68K_PCMI_PI:
		case M68K_PCMI_PE:
		case M68K_MI_PE:
		case M68K_MI_PI:
		{
			SStream_concat(O, "([");

			if (op->mem.in_disp > 0)
				SStream_concat(O, "$%x", op->mem.in_disp);

			if (op->mem.base_reg != M68K_REG_INVALID) {
				if (op->mem.in_disp > 0)
					SStream_concat(O, ",%s", getRegName(op->mem.base_reg)); 
				else
					SStream_concat(O, "%s", getRegName(op->mem.base_reg)); 
			}

			if (op->address_mode == M68K_MI_PI || op->address_mode == M68K_PCMI_PI)
				SStream_concat(O, "]");

			if (op->mem.index_reg != M68K_REG_INVALID)
				SStream_concat(O, ",%s.%c", getRegName(op->mem.index_reg), op->mem.index_size ? 'l' : 'w');

			if (op->mem.scale > 0)
				SStream_concat(O, "*%d", op->mem.scale);

			if (op->address_mode == M68K_MI_PE || op->address_mode == M68K_PCMI_PE)
				SStream_concat(O, "]");

			if (op->mem.out_disp > 0)
				SStream_concat(O, ",$%x", op->mem.out_disp);

			SStream_concat(O, ")");
		}

		default:
			break;
	}
}

void M68K_printInst(MCInst* MI, SStream* O, void* Info)
{
	printf("MI->Opcode %d\n", MI->Opcode);

	cs_m68k* info = &MI->flat_insn->detail->m68k;

	switch (MI->Opcode)
	{
		case M68K_INSN_NOP : SStream_concat0(O, "nop"); break;
		case M68K_INSN_OR : SStream_concat0(O, "or"); break;
		case M68K_INSN_AND : SStream_concat0(O, "and"); break;
	}

	switch (info->op_size)
	{
		case 1 : SStream_concat0(O, ".b"); break;
		case 2 : SStream_concat0(O, ".w"); break;
		case 4 : SStream_concat0(O, ".l"); break;
	}

	if (info->op_count == 0)
		return;
	
	SStream_concat0(O, " ");

	printAddressingMode(O, &info->operands[0]);

	if (info->op_count == 1)
		return;

	SStream_concat0(O, ",");

	printAddressingMode(O, &info->operands[1]);
}

bool M68K_getInstruction(csh ud, const uint8_t* code, size_t code_len, MCInst* instr, uint16_t* size, uint64_t address, void* info)
{
	int s;

	s_disassemblyBuffer = (uint8_t*)code;
	s_baseAddress = (uint32_t)address;

	printf("getInstruction: %d %p\n", (int)address, s_disassemblyBuffer);

	s = m68k_disassemble(instr, address, M68K_CPU_TYPE_68000);

	if (s == 0)
		return false;

	*size = (uint16_t)s;

	return true;
}

const char* M68K_reg_name(csh handle, unsigned int reg)
{
	return s_reg_names[(int)reg];
}

void M68K_get_insn_id(cs_struct* h, cs_insn* insn, unsigned int id)
{
	insn->id = id; // These id's matches for 68k
}

const char* M68K_insn_name(csh handle, unsigned int id)
{
	switch (id)
	{
		case M68K_INSN_NOP : return "nop";
		case M68K_INSN_OR : return "or";
		case M68K_INSN_AND : return "and";
	}

	return 0;
}

const char* M68K_group_name(csh handle, unsigned int id)
{
	return 0;
}

void M68K_post_printer(csh handle, cs_insn* flat_insn, char* insn_asm, MCInst* mci)
{
}

