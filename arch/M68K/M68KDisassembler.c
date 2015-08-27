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
  { 3, 0, 0, 0, 0, 0 },
  { 38, 1, 1, 0, 1, 0 },
  { 41, 1, 1, 0, 1, 0 },
  { 47, 1, 1, 0, 1, 0 },
  { 44, 1, 1, 0, 1, 0 },
  { 4, 1, 1, 0, 1, 0 },
  { 11, 1, 1, 0, 1, 0 },
  { 14, 1, 1, 0, 1, 0 },
  { 17, 1, 1, 0, 1, 0 },
  { 20, 1, 1, 0, 1, 0 },
  { 23, 1, 1, 0, 1, 0 },
  { 26, 1, 1, 0, 1, 0 },
  { 29, 1, 1, 0, 1, 0 },
  { 32, 1, 1, 0, 1, 0 },
  { 35, 1, 1, 0, 1, 0 },
  { 0, 1, 1, 0, 1, 0 },
  { 7, 1, 1, 0, 1, 0 },
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


void M68K_printInst(MCInst *MI, SStream *O, void *Info)
{
	printf("%s\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int m68k_read_disassembler_8(uint64_t address)
{
	uint8_t* data = (uint8_t*)(uintptr_t)address;
	return *data; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int m68k_read_disassembler_16(uint64_t address)
{
	uint8_t* data = (uint8_t*)(uintptr_t)address;

	uint16_t v0 = data[0];
	uint16_t v1 = data[1];

	return (v0 << 8) | v1; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int m68k_read_disassembler_32(unsigned int address)
{
	uint8_t* data = (uint8_t*)(uintptr_t)address;

	uint32_t v0 = data[0]; 
	uint32_t v1 = data[1];
	uint32_t v2 = data[2];
	uint32_t v3 = data[3];

	return (v0 << 24) | (v1 << 16) | (v2 << 8) | v3;
}

bool M68K_getInstruction(csh ud, const uint8_t* code, size_t code_len, MCInst* instr, uint16_t* size, uint64_t address, void* info)
{
	printf("%p %d %p %llx \n", code, (int)code_len, instr, address);
	printf("%s\n", __FUNCTION__);
	return false;
}

const char* M68K_reg_name(csh handle, unsigned int reg)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

void M68K_get_insn_id(cs_struct* h, cs_insn* insn, unsigned int id)
{
	printf("%s\n", __FUNCTION__);
}

const char* M68K_insn_name(csh handle, unsigned int id)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

const char* M68K_group_name(csh handle, unsigned int id)
{
	printf("%s\n", __FUNCTION__);
	return 0;
}

void M68K_post_printer(csh handle, cs_insn* flat_insn, char* insn_asm, MCInst* mci)
{
	printf("%s\n", __FUNCTION__);
	(void)handle;
	(void)flat_insn;
	(void)insn_asm;
	(void)handle;
}

