/* Capstone Disassembly Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013-2014 */

#ifndef CS_M68KDISASSEMBLER_H
#define CS_M68KDISASSEMBLER_H

#include <stdint.h>

#include "capstone/capstone.h"
#include "../../MCRegisterInfo.h"
#include "../../MCInst.h"

void M68K_init(MCRegisterInfo *MRI);

//bool XCore_getInstruction(csh ud, const uint8_t *code, size_t code_len,
//		MCInst *instr, uint16_t *size, uint64_t address, void *info);

#endif

