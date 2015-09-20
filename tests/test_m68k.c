/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013-2014 */

#include <stdio.h>
#include "../myinttypes.h"

#include <capstone/capstone.h>

struct platform {
	cs_arch arch;
	cs_mode mode;
	unsigned char* code;
	size_t size;
	char* comment;
};

static csh handle;

static void print_string_hex(char* comment, unsigned char* str, size_t len)
{
	unsigned char *c;

	printf("%s", comment);
	for (c = str; c < str + len; c++) {
		printf("0x%02x ", *c & 0xff);
	}

	printf("\n");
}

static void print_insn_detail(cs_insn *ins)
{
	cs_m68k* m68k;
	int i;

	// detail can be NULL on "data" instruction if SKIPDATA option is turned ON
	if (ins->detail == NULL)
		return;

	m68k = &(ins->detail->m68k);
	if (m68k->op_count)
		printf("\top_count: %u\n", m68k->op_count);

	for (i = 0; i < m68k->op_count; i++) {
		cs_m68k_op* op = &(m68k->operands[i]);
		
		switch((int)op->type) {
			default:
				break;
			case M68K_OP_REG:
				printf("\t\toperands[%u].type: REG = %s\n", i, cs_reg_name(handle, op->reg));
				break;
			case M68K_OP_IMM:
				printf("\t\toperands[%u].type: IMM = 0x%x\n", i, (int)op->imm);
				break;
			case M68K_OP_MEM:
				printf("\t\toperands[%u].type: MEM\n", i);
				if (op->mem.base_reg != M68K_REG_INVALID)
					printf("\t\t\toperands[%u].mem.base: REG = %s\n",
							i, cs_reg_name(handle, op->mem.base_reg));
				if (op->mem.index_reg != M68K_REG_INVALID)
					printf("\t\t\toperands[%u].mem.index: REG = %s\n",
							i, cs_reg_name(handle, op->mem.index_reg));
				if (op->mem.disp != 0)
					printf("\t\t\toperands[%u].mem.disp: 0x%x\n", i, op->mem.disp);

				break;
		}
	}

	printf("\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* loadToMemory(const char* filename, size_t* size)  
{
    FILE* f = fopen(filename, "rb");
    void* data = 0;
    size_t s = 0, t = 0;

    *size = 0;

    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);
    long ts = ftell(f);

    if (ts < 0)
    	goto end;

    s = (size_t)ts;

    data = malloc(s);

    if (!data)
    	goto end;

    fseek(f, 0, SEEK_SET);

    t = fread(data, s, 1, f);
    (void)t;

    *size = s;

end:

    fclose(f);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void test()
{
	size_t size;
	void* code = loadToMemory("../u/m68k.bin", &size);
	printf("size %d\n", (int)size);

//#define M68K_CODE "\x4E\x71\x22\x00\x4E\x75"
//#define M68K_CODE "\x4E\x71"

	struct platform platforms[] = {
		{
			CS_ARCH_M68K,
			CS_MODE_BIG_ENDIAN,
			code,
			size,
			//(unsigned char*)M68K_CODE,
			//sizeof(M68K_CODE) - 1,
			"M68K",
		},
	};

	uint64_t address = 0x030672;
	cs_insn *insn;
	int i;
	size_t count;

	for (i = 0; i < sizeof(platforms)/sizeof(platforms[0]); i++) {
		cs_err err = cs_open(platforms[i].arch, platforms[i].mode, &handle);
		if (err) {
			printf("Failed on cs_open() with error returned: %u\n", err);
			abort();
		}

		cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

		count = cs_disasm(handle, platforms[i].code, platforms[i].size, address, 0, &insn);
		if (count) {
			size_t j;

			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("Disasm:\n");

			for (j = 0; j < count; j++) {
				printf("0x%"PRIx64":\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
				print_insn_detail(&insn[j]);
			}
			printf("0x%"PRIx64":\n", insn[j-1].address + insn[j-1].size);

			// free memory allocated by cs_disasm()
			cs_free(insn, count);
		} else {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("ERROR: Failed to disasm given code!\n");
			abort();
		}

		printf("\n");

		cs_close(&handle);
	}
}

int main()
{
	test();

	return 0;
}
