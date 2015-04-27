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
#include "main.h"

#ifdef PROFILER
#include <time.h>
#include "gboy_cpu.h"
extern struct z80_set z80_ldex[];
extern Uint32 instruction_counter[];
#endif

static void
usage(char *cmd)
{
  printf("%s [option ...] file\n", cmd);
  printf("\
\n\
Options:\n\
  -1, --video-1x\t\tDon't scale\n\
  -2, --video-2x\t\t2x scale\n\
  -3, --video-3x\t\t3x scale\n\
  -4, --video-4x\t\t4x scale\n\
  -r, --frame-rate=RATE\t\tSet frame rate (range [10, 60])\n\
  -f, --fullscreen\t\tFullscreen\n\
  -b, --with-boot\t\tExecute boot ROM\n\
  -h, --help\t\t\tPrint this help\n\
  -d, --debug\t\t\tEnable GDDB debugger\n\
  -v, --version\t\t\tPrint current version and exit\n\
  -D, --DMG\t\t\tForce Game Boy Mode\n\
  -C, --CGB\t\t\tForce Color Game Boy Mode\n\
  -S, --SGB\t\t\tForce Super Game Boy Mode\n\
");
}

/*
 * Main function.
 */
int
main(int argc, char *argv[])
{
#ifdef PROFILER
	clock_t t;
	t = clock();
#endif

	/* If no arguments, print usage and exit. */
	if (argc==1) {
		usage(argv[0]);
		return 0;
	}

	/* Parse arguments. */
	int op;
	do {
		op = getopt_long(argc, argv, "r:1234fhdbvDCS", options, NULL);
		int arg;
		switch (op) {
			/* Video */
			case '1':
					vid_scale(1);
					ignore_conf(SCALE);
					break;
			case '2':
					vid_scale(2);
					ignore_conf(SCALE);
					break;
			case '3':
					vid_scale(3);
					ignore_conf(SCALE);
					break;
			case '4':
					vid_scale(4);
					ignore_conf(SCALE);
					break;
			case 'f':
					vid_toggle_fullscreen();
					ignore_conf(FULLSCREEN);
					break;
			case 'r':
					ignore_conf(FPS);
					arg = atoi(optarg);
					if (arg >= 10 && arg <= 60)
						set_fps(arg);
					else
						printf("Bad argument for frame rate; defaulting to 60fps\n");
					break;
			/* Enable boot ROM */
			case 'b':
				ignore_conf(BOOT);
				use_boot_rom=1;
				break;
			/* Print help and exit */
			case 'h':
				usage(argv[0]);
				return 0;
			/* Enable debugger */
			case 'd':
				gbddb=1;
				break;
			/* Print current version and exit */
			case 'v':
				printf("\nRealBoy 0.2.2\n");
				return 0;
			/* Force Operation Mode */
			case 'D':
				ignore_conf(GB_MODE);
				gboy_hw=DMG;
				break;
			case 'C':
				ignore_conf(GB_MODE);
				gboy_hw=CGB;
				break;
			case 'S':
				ignore_conf(GB_MODE);
				gboy_hw=SGB;
				break;
			default:
				break;

		}
	} while (op != -1);

	if (optind < argc) {
		if ( (rom_file=fopen(argv[optind], "r")) == NULL)
			perror(argv[optind]);
		else
			file_path = strndup(argv[optind], 256);
	}

	if (rom_file != NULL)	{
		init_conf();
		int ret_val; // value returned from emulation
		/* Start Virtual Machine */
		ret_val=start_vm();
		/* Error returned if file not valid */
		if (ret_val == -1)
			printf("File not a gb binary\n\n");
		else
			printf("\nThank you for using RealBoy!\n\n");
	}
	else
		usage(argv[0]);

#ifdef PROFILER
	t = clock() - t;
	unsigned long long total_instructions = 0, total_time = 0;

	change_cur_dir("/tmp");
	FILE* pgdataFile = fopen("profiler.general_data", "w");
	FILE* prdataFile = fopen("profiler.run_data", "w");

	printf("%s", "===================  Awesome profiler stats  ===================\n");
	fprintf(pgdataFile, "%s", "===================  Awesome profiler stats  ===================\n");
	fprintf(prdataFile, "%s", "===================  Awesome profiler stats  ===================\n");
	printf("%s", "Opcode - Instruction - DELAY - RD_XOR_WR - RD_WR - Ext - N_Instructions - CPU_Ticks - AVG_time\n");
	fprintf(pgdataFile, "%s", "Opcode?Opcode(0x)?Instruction?DELAY?RD_XOR_WR?RD_WR?Ext\n");
	fprintf(prdataFile, "%s", "Opcode?Usage?Time (ml)\n");

	Uint16 opcode;
	for (opcode  = 0; opcode < NUMBER_OF_INSTRUCTIONS; opcode++) {
		printf("0x%-4x, %-15s, %s, %s, %s, %s, %10llu, %10llu, ",
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
	printf("%s", "Time, it needs time (to win back your love again)\n");
	fprintf(prdataFile, "%s", "Time, it needs time (to win back your love again)\n");
	printf ("%d clicks (%f seconds).\n",(int)t,((float)t)/CLOCKS_PER_SEC);
	fprintf (prdataFile, "%d clicks (%f seconds).\n",(int)t,((float)t)/CLOCKS_PER_SEC);

	printf ("%s", "\n\nFiles with data in /tmp/profiler.general_data (general instructions data)");
	printf (" and /tmp/profiler.run_data (data for this run).\n");

	fclose(pgdataFile);
	fclose(prdataFile);
#endif

	return 0;
}
