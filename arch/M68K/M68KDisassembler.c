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
	"fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7",
	"pc",
	"sr", "ccr", "sfc", "dfc", "usp", "vbr", "cacr",
	"caar", "msp", "isp", "tc", "itt0", "itt1", "dtt0",
	"dtt1", "mmusr", "urp", "srp",
};

static const char* s_instruction_names[] = {
	"invalid",
	"abcd", "add", "adda", "addi", "addq", "addx", "and", "andi", "asl", "asr", "bhs", "blo", "bhi", "bls", "bcc", "bcs", "bne", "beq", "bvc",
	"bvs", "bpl", "bmi", "bge", "blt", "bgt", "ble", "bra", "bsr", "bchg", "bclr", "bset", "btst", "bfchg", "bfclr", "bfexts", "bfextu", "bfffo", "bfins",
	"bfset", "bftst", "bkpt", "callm", "cas", "cas2", "chk", "chk2", "clr", "cmp", "cmpa", "cmpi", "cmpm", "cmp2", "cinvl", "cinvp", "cinva", "cpushl", "cpushp",
	"cpusha", "dbt", "dbf", "dbhi", "dbls", "dbcc", "dbcs", "dbne", "dbeq", "dbvc", "dbvs", "dbpl", "dbmi", "dbge", "dblt", "dbgt", "dble", "dbra",
	"divs", "divsl", "divu", "divul", "eor", "eori", "exg", "ext", "extb", "fabs", "fsabs", "fdabs", "facos", "fadd", "fsadd", "fdadd", "fasin",
	"fatan", "fatanh", "fbf", "fbeq", "fbogt", "fboge", "fbolt", "fbole", "fbogl", "fbor", "fbun", "fbueq", "fbugt", "fbuge", "fbult", "fbule", "fbne", "fbt",
	"fbsf", "fbseq", "fbgt", "fbge", "fblt", "fble", "fbgl", "fbgle", "fbngle", "fbngl", "fbnle", "fbnlt", "fbnge", "fbngt", "fbsne", "fbst", "fcmp", "fcos",
	"fcosh", "fdbf", "fdbeq", "fdbogt", "fdboge", "fdbolt", "fdbole", "fdbogl", "fdbor", "fdbun", "fdbueq", "fdbugt", "fdbuge", "fdbult", "fdbule", "fdbne",
	"fdbt", "fdbsf", "fdbseq", "fdbgt", "fdbge", "fdblt", "fdble", "fdbgl", "fdbgle", "fdbngle", "fdbngl", "fdbnle", "fdbnlt", "fdbnge", "fdbngt", "fdbsne",
	"fdbst", "fdiv", "fsdiv", "fddiv", "fetox", "fetoxm1", "fgetexp", "fgetman", "fint", "fintrz", "flog10", "flog2", "flogn", "flognp1", "fmod", "fmove",
	"fsmove", "fdmove", "fmovecr", "fmovem", "fmul", "fsmul", "fdmul", "fneg", "fsneg", "fdneg", "fnop", "frem", "fscale", "fsgldiv", "fsglmul", "fsin",
	"fsincos", "fsinh", "fsqrt", "fssqrt", "fdsqrt", "fsub", "fssub", "fdsub", "ftan", "ftanh", "ftentox", "ftrapf", "ftrapeq", "ftrapogt", "ftrapoge",
	"ftrapolt", "ftrapole", "ftrapogl", "ftrapor", "ftrapun", "ftrapueq", "ftrapugt", "ftrapuge", "ftrapult", "ftrapule", "ftrapne", "ftrapt", "ftrapsf",
	"ftrapseq", "ftrapgt", "ftrapge", "ftraplt", "ftraple", "ftrapgl", "ftrapgle", "ftrapngle", "ftrapngl", "ftrapnle", "ftrapnlt", "ftrapnge", "ftrapngt",
	"ftrapsne", "ftrapst", "ftst", "ftwotox", "halt", "illegal", "jmp", "jsr", "lea", "link", "lpstop", "lsl", "lsr", "move", "movea", "movec", "movem",
	"movep", "moveq", "moves", "move16", "muls", "mulu", "nbcd", "neg", "negx", "nop", "not", "or", "ori", "pack", "pea", "pflush", "pflusha", "pflushan",
	"pflushn", "ploadr", "ploadw", "plpar", "plpaw", "pmove", "pmovefd", "ptestr", "ptestw", "pulse", "rems", "remu", "reset", "rol", "ror", "roxl",
	"roxr", "rtd", "rte", "rtm", "rtr", "rts", "sbcd", "st", "sf", "shi", "sls", "scc", "shs", "scs", "slo", "sne", "seq", "svc", "svs", "spl", "smi",
	"sge", "slt", "sgt", "sle", "stop", "sub", "suba", "subi", "subq", "subx", "swap", "tas", "trap", "trapv", "trapt", "trapf", "traphi", "trapls",
	"trapcc", "traphs", "trapcs", "traplo", "trapne", "trapeq", "trapvc", "trapvs", "trappl", "trapmi", "trapge", "traplt", "trapgt", "traple", "tst", "unlk", "unpk",
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

static void registerBits(SStream* O, const cs_m68k_op* op)
{
	char buffer[40];
	unsigned int first;
	unsigned int run_length;
	unsigned int data = op->register_bits; 

	buffer[0] = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (data & (1 << i)) {
			first = i;
			run_length = 0;
			while (i < 7 && (data & (1 << (i + 1)))) {
				i++;
				run_length++;
			}

			if (buffer[0] != 0)
				strcat(buffer, "/");

			sprintf(buffer + strlen(buffer), "d%d", first);
			if (run_length > 0)
				sprintf(buffer + strlen(buffer), "-d%d", first + run_length);
		}
	}

	for (int i = 0; i < 8; ++i) {
		if (data & (1 << (i +8 ))) {
			first = i;
			run_length = 0;

			while(i < 7 && (data & (1 << (i + 8 + 1)))) {
				i++;
				run_length++;
			}

			if (buffer[0] != 0)
				strcat(buffer, "/");

			sprintf(buffer + strlen(buffer), "a%d", first);

			if (run_length > 0)
				sprintf(buffer + strlen(buffer), "-a%d", first + run_length);
		}
	}

	SStream_concat(O, "%s", buffer);
}

static void registerPair(SStream* O, const cs_m68k_op* op)
{
	int reg_value_0 = op->register_bits >> 4;
	int reg_value_1 = op->register_bits & 0xf;

	SStream_concat(O, "%s:%s", s_reg_names[M68K_REG_D0 + (op->register_bits >> 4)], 
							   s_reg_names[M68K_REG_D0 + (op->register_bits & 0xf)]);
}

void printAddressingMode(SStream* O, const cs_m68k_op* op)
{
	switch (op->address_mode)
	{
		case M68K_AM_NONE:
		{
			switch (op->type)
			{
				case M68K_OP_REG_BITS:
				{
					registerBits(O, op);
					break;
				}

				case M68K_OP_REG_PAIR:
				{
					registerPair(O, op);
					break;
				}

				case M68K_OP_REG:
				{
					SStream_concat(O, "%s", s_reg_names[op->reg]);
					break;
				}

				default:
					break;
			}

			break;
		}


		case M68K_RD_DATA : SStream_concat(O, "d%d", (op->reg - M68K_REG_D0)); break;
		case M68K_RD_ADDRESS : SStream_concat(O, "a%d", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS : SStream_concat(O, "(a%d)", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_PI : SStream_concat(O, "(a%d)+", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_PD : SStream_concat(O, "-(a%d)", (op->reg - M68K_REG_A0)); break;
		case M68K_RI_ADDRESS_D : SStream_concat(O, "$%x(a%d)", op->mem.disp, (op->reg - M68K_REG_A0)); break;
		case M68K_PCI_DISP : SStream_concat(O, "$%x(pc)", op->mem.disp); break;
		case M68K_ADA_SHORT : SStream_concat(O, "$%x.w", op->imm); break; 
		case M68K_ADA_LONG : SStream_concat(O, "$%x.l", op->imm); break; 
		case M68K_IMMIDIATE : SStream_concat(O, "#$%x", op->imm); break; 
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

	if (op->mem.bitfield)
		SStream_concat(O, "{%d:%d}", op->mem.offset, op->mem.width);
}

void M68K_printInst(MCInst* MI, SStream* O, void* Info)
{
	cs_m68k* info = &MI->flat_insn->detail->m68k;

	const int op_count = info->op_count;


	SStream_concat0(O, (char*)s_instruction_names[MI->Opcode]);

	switch (info->op_size)
	{
		case 1 : SStream_concat0(O, ".b"); break;
		case 2 : SStream_concat0(O, ".w"); break;
		case 4 : SStream_concat0(O, ".l"); break;
	}

	SStream_concat0(O, " ");

	// this one is a bit spacial so we do spacial things

	if (MI->Opcode == M68K_INS_CAS2)
	{
		printAddressingMode(O, &info->operands[0]); SStream_concat0(O, ",");
		printAddressingMode(O, &info->operands[1]); SStream_concat0(O, ",");

		int reg_value_0 = info->operands[2].register_bits >> 4;
		int reg_value_1 = info->operands[2].register_bits & 0xf;

		SStream_concat(O, "(%s):(%s)", s_reg_names[M68K_REG_D0 + reg_value_0], s_reg_names[M68K_REG_D0 + reg_value_1]); 

		return;
	}

	for (int i  = 0; i < op_count; ++i)
	{
		printAddressingMode(O, &info->operands[i]);

		if ((i + 1) != op_count)
			SStream_concat0(O, ",");
	}
}

bool M68K_getInstruction(csh ud, const uint8_t* code, size_t code_len, MCInst* instr, uint16_t* size, uint64_t address, void* info)
{
	int s;

	s_disassemblyBuffer = (uint8_t*)code;
	s_baseAddress = (uint32_t)address;

	s = m68k_disassemble(instr, address, M68K_CPU_TYPE_68000);
	printf("getInstruction: %p %d %p\n", (void*)address, s, s_disassemblyBuffer);

	if (s == 0)
	{
		*size = 2;
		return false;
	}

	SStream ss;
	SStream_Init(&ss);
	M68K_printInst(instr, &ss, info);
	printf("%s\n", ss.buffer);

	// Make sure we always stay within range 

	if (s > code_len)
		*size = code_len;
	else
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
	return s_instruction_names[id];
}

const char* M68K_group_name(csh handle, unsigned int id)
{
	return 0;
}

void M68K_post_printer(csh handle, cs_insn* flat_insn, char* insn_asm, MCInst* mci)
{
}

