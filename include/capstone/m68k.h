#ifndef CAPSTONE_68K_H
#define CAPSTONE_68K_H

/* Capstone Disassembly Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2014 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "platform.h"

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif
	
//> M68K registers
typedef enum m68k_reg {
	M68K_REG_INVALID = 0,

	M68K_REG_D0,
	M68K_REG_D1,
	M68K_REG_D2,
	M68K_REG_D3,
	M68K_REG_D4,
	M68K_REG_D5,
	M68K_REG_D6,
	M68K_REG_D7,

	M68K_REG_A0,
	M68K_REG_A1,
	M68K_REG_A2,
	M68K_REG_A3,
	M68K_REG_A4,
	M68K_REG_A5,
	M68K_REG_A6,
	M68K_REG_A7,

	M68K_REG_SR,
	M68K_REG_PC,

	M68K_REG_ENDING,   // <-- mark the end of the list of registers
} m68k_reg;


//> M68K Addressing Modes
typedef enum m68k_adress_mode {
	M68K_RD_DATA,				// Register Direct - Data
	M68K_RD_ADDRESS,			// Register Direct - Address

	M68K_RI_ADDRESS,			// Register Indirect - Address
	M68K_RI_ADDRESS_PI,			// Register Indirect - Address with Postincrement
	M68K_RI_ADDRESS_PD,			// Register Indirect - Address with Predecrement
	M68K_RI_ADDRESS_D,			// Register Indirect - Address with Displacement

	M68K_ARIWI_8_BIT,			// Address Register Indirect With Index- 8-bit displacement
	M68K_ARIWI_BASE,			// Address Register Indirect With Index- Base displacement

	M68K_MI_PI,					// Memory indirect - Postindex
	M68K_MI_PE,					// Memory indirect - Preindex

	M68K_PCI_DISP,				// Program Counter Indirect - with Displacement

	M68K_PCIIWI_8_BIT,			// Program Counter Indirect with Index - with 8-Bit Displacement
	M68K_PCIIWI_BASE,			// Program Counter Indirect with Index - with Base Displacement

	M68K_PCMI_PI,				// Program Counter Memory Indirect - Postindexed
	M68K_PCMI_PE,				// Program Counter Memory Indirect - Preindexed

	M68K_ADA_SHORT,				// Absolute Data Addressing  - Short
	M68K_ADA_LONG,				// Absolute Data Addressing  - Long
	M68K_IMMIDATE,				// Immidate value

} m68k_adress_mode; 

//> Operand type for instruction's operands

typedef enum m68k_op_type {
	M68K_OP_INVALID = 0, // = CS_OP_INVALID (Uninitialized).
	M68K_OP_REG, // = CS_OP_REG (Register operand).
	M68K_OP_IMM, // = CS_OP_IMM (Immediate operand).
	M68K_OP_MEM, // = CS_OP_MEM (Memory operand).
} m68k_op_type;

// Instruction's operand referring to memory
// This is associated with M68K_OP_MEM operand type above

typedef struct m68k_op_mem {
	m68k_reg base_reg; // base register (or M68K_REG_INVALID if irrelevant)
	m68k_reg index_reg; // index register (or M68K_REG_INVALID if irrelevant)
	m68k_reg in_base_reg; // indirect base register (or M68K_REG_INVALID if irrelevant)
	uint32_t in_disp; // indirect displacement 
	uint32_t out_disp; // outher displacement 
	uint8_t scale;	// scale for index register
	uint16_t disp;	// displacement value
	uint8_t  width;	// used for bf* instructions 
	uint8_t offset;	// used for bf* instructions
	uint8_t index_size; // 0 = w, 1 = l
} m68k_op_mem;

// Instruction operand
typedef struct cs_m68k_op {
	m68k_adress_mode address_mode;	// addressing mode for
	union {
		uint64_t imm;		// immediate value for IMM operand
		double fimm;
		m68k_reg reg;	// register value for REG operand
		m68k_op_mem mem;
	};
} cs_m68k_op;

typedef struct cs_m68k
{
	// Number of operands of this instruction, 
	// or 0 when instruction has no operand.
	cs_m68k_op operands[2]; // operands for this instruction.
	uint8_t op_count;
	uint8_t op_size;	// size of data operand works on in bytes (.b, .w, .l, etc)
} cs_m68k;

//> M68K instruction
typedef enum m68k_insn {
	M68K_INS_INVALID = 0,

	M68K_INSN_ILLEGAL,

	M68K_INSN_NOP,
	M68K_INSN_OR,

} m68k_insn;

#ifdef __cplusplus
}
#endif

#endif

