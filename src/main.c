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
	unsigned long long total_instructions;

	printf("%s", "===================  Awesome profiler stats  ===================\n");
	printf("%s", "Opcode ? Instructions ? Usage\n");
	Uint16 opcode;
	for (opcode  = 0; opcode < NUMBER_OF_INSTRUCTIONS; opcode++) {
		printf("0x%x ? %s ? %d ? %d\n", opcode, z80_ldex[opcode].name, instruction_counter[opcode].instruction_counter, instruction_time_counter);
		total_instructions += instruction_counter[opcode];
	}
	printf("%s", "======================\n");
	printf("Total instructions: %d\n", total_instructions);
	printf("%s", "Time, it needs time (to win back your love again)\n");
	printf ("%d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
#endif

	return 0;
}
