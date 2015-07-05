/* RealBoy Emulator: Free, Fast, Yet Accurate, Game Boy/Game Boy Color Emulator.
 * Copyright (C) 2013 Sergio Andrés Gómez del Real
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gboy.h"
#include "gboy_profiler.h"
#include "gboy_cpu.h"

extern struct z80_set z80_ldex[];
int profiler = 0;
struct profilerInfo profilerData[512];

void outputProfilerData(){
	unsigned long long total_instructions = 0, total_time = 0;

	change_cur_dir("/tmp");
	FILE* pgdataFile = fopen("profiler.general_data", "w");
	FILE* prdataFile = fopen("profiler.run_data", "a");

	printf("%s", "===================  Awesome profiler stats  ===================\n");
	fprintf(pgdataFile, "%s", "===================  Awesome profiler stats  ===================\n");
	fprintf(prdataFile, "%s", "===================  Awesome profiler stats  ===================\n");
	printf("%s", "Opcode - Instruction - DELAY - RD_XOR_WR - RD_WR - Ext - N_Instructions - CPU_Ticks - AVG_time\n");
	fprintf(pgdataFile, "%s", "Opcode?Opcode(0x)?Instruction?DELAY?RD_XOR_WR?RD_WR?Ext\n");
	fprintf(prdataFile, "%s", "Opcode?Usage?Time (ml)\n");

	Uint16 opcode;
	for (opcode  = 0; opcode < NUMBER_OF_INSTRUCTIONS; opcode++) {
		printf("0x%-4x, %-15s, %s, %s, %s, %s, %16llu, %16llu, ",
				opcode, z80_ldex[opcode].name,
				(z80_ldex[opcode].format[5] & DELAY) ? "Y" : "N",
				(z80_ldex[opcode].format[5] & RD_XOR_WR) ? "Y" : "N",
				(z80_ldex[opcode].format[5] & RD_WR) ? "Y" : "N",
				(opcode > 0xFF) ? "Y" : "N",
				profilerData[opcode].instruction_counter,
				profilerData[opcode].instruction_time_counter);

		if(profilerData[opcode].instruction_counter != 0)
			printf("AVG:%10llu\n", profilerData[opcode].instruction_time_counter /
					profilerData[opcode].instruction_counter);
		else
			printf("AVG:%10llu\n", profilerData[opcode].instruction_time_counter);

		fprintf(pgdataFile, "%d?0x%x?%s?%s?%s?%s?%s\n",
						opcode, opcode, z80_ldex[opcode].name,
						(z80_ldex[opcode].format[5] & DELAY) ? "Y" : "N",
						(z80_ldex[opcode].format[5] & RD_XOR_WR) ? "Y" : "N",
						(z80_ldex[opcode].format[5] & RD_WR) ? "Y" : "N",
						(opcode > 0xFF) ? "Y" : "N");

		fprintf(prdataFile, "%d?%llu?%llu?",
						opcode,
						profilerData[opcode].instruction_counter,
						profilerData[opcode].instruction_time_counter);

		if(profilerData[opcode].instruction_counter != 0)
			fprintf(prdataFile, "%llu\n", profilerData[opcode].instruction_time_counter /
					profilerData[opcode].instruction_counter);
		else
			fprintf(prdataFile, "%llu\n", profilerData[opcode].instruction_time_counter);

		total_instructions += profilerData[opcode].instruction_counter;
		total_time += profilerData[opcode].instruction_time_counter;
	}

	printf("%s", "======================\n");
	fprintf(prdataFile, "%s", "======================\n");
	printf("Total GameBoy instructions: %llu\n", total_instructions);
	fprintf(prdataFile, "Total GameBoy instructions: %llu\n", total_instructions);

	printf ("%s", "Files with profiling data are in /tmp/profiler.general_data (general instructions data)");
	printf (" and /tmp/profiler.run_data (data for this run).\n\n");

	fclose(pgdataFile);
	fclose(prdataFile);
}





long addr_sp_ptrs_predecoded[8] = { 0 }; // 0x0000 - 0x7FFF
struct predecode_mold{
	void (* operation_adress) (struct z80_set *rec);
	Uint8 uint8;
};
struct predecode_mold *predecoded_instructions;


void bootstrap_func(struct z80_set *rec){
	rec->func(rec);
}

void init_predecode_structure(){

	/* Allocate space for predecoded */
	Uint32 predecoded_mem_size= (0x8000<<gb_cart.cart_rom_size);
	predecoded_instructions = malloc( predecoded_mem_size * sizeof(struct predecode_mold));

	/* initialize predecoded code to bootstrap function */
	int i;
	for (i=0; i < predecoded_mem_size; i++){
		predecoded_instructions[i].operation_adress = &bootstrap_func;
	}
	printf("SIZE:%u\n", sizeof(struct predecode_mold));
	printf("%u\n", predecoded_mem_size);

}







