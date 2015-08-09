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

//> Operand type for instruction's operands

typedef enum m68k_op_type {
	M68K_OP_INVALID = 0, // = CS_OP_INVALID (Uninitialized).
	M68K_OP_REG, // = CS_OP_REG (Register operand).
	M68K_OP_IMM, // = CS_OP_IMM (Immediate operand).
	M68K_OP_MEM, // = CS_OP_MEM (Memory operand).
} m68k_op_type;

// Instruction operand
typedef struct cs_m68k_op {
	m68k_op_type type;	// operand type
	union {
		unsigned int reg;	// register value for REG operand
		int64_t imm;		// immediate value for IMM operand
	};
} cs_m68k_op;

typedef struct cs_m68k
{
	// Number of operands of this instruction, 
	// or 0 when instruction has no operand.
	uint8_t op_count;
	cs_m68k_op operands[8]; // operands for this instruction.
} cs_m68k;

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


#ifdef __cplusplus
}
#endif

#endif

