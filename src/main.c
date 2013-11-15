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
/* External symbols */
extern char *file_path;
extern int use_boot_rom;
extern Uint32 scale;
extern Uint32 fullscreen;
extern int frames_per_second;
extern int start_vm();
extern void vid_scale(int);
extern void set_fps(int);

/* Locally-global variables*/
struct option options[] = {
	{ "video-2x", no_argument, 0, '2' },
	{ "video-3x", no_argument, 0, '3' },
	{ "video-4x", no_argument, 0, '4' },
	{ "debug", no_argument, 0, 'd' },
	{ "frame-rate", required_argument, 0, 'r' },
	{ "fullscreen", no_argument, 0, 'f' },
	{ "with-boot", no_argument, 0, 'b' },
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'h' },
	{ NULL, no_argument, 0, 0 }
};

static void
usage(char *cmd)
{
  printf("%s [option ...] file\n", cmd);
  printf("\
\n\
Options:\n\
  -2, --video-2x\t\t2x scale\n\
  -3, --video-3x\t\t3x scale\n\
  -4, --video-4x\t\t4x scale\n\
  -r, --frame-rate=RATE\t\tSet frame rate (range [10, 60])\n\
  -f, --fullscreen\t\tFullscreen\n\
  -b, --with-boot\t\tExecute boot ROM\n\
  -h, --help\t\t\tPrint this help\n\
  -d, --debug\t\t\tEnable GDDB debugger\n\
  -v, --version\t\t\tPrint current version and exit\n\
");
}

/*
 * Main function.
 */
int
main(int argc, char *argv[])
{
	/* If no arguments, print usage and exit. */
	if (argc==1) {
		usage(argv[0]);
		return 0;
	}

	if (optind < argc) {
		if ( (rom_file=fopen(argv[optind], "r")) == NULL)
			perror(argv[optind]);
		else
			file_path = strndup(argv[optind], 256);
	}
	init_conf();

	/* Parse arguments. */
	int op;
	do {
		op = getopt_long(argc, argv, "r:234fhdbv", options, NULL);
		int arg;
		switch (op) {
			/* Video */
			case '2':
					vid_scale(2);
					break;
			case '3':
					vid_scale(3);
					break;
			case '4':
					vid_scale(4);
					break;
			case 'f':
					vid_toggle_fullscreen();
					break;
			case 'r':
					arg = atoi(optarg);
					if (arg >= 10 && arg <= 60)
						set_fps(arg);
					else
						printf("Bad argument for frame rate; defaulting to 60fps\n");
					break;
			/* Enable boot ROM */
			case 'b':
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
				printf("RealBoy 0.1.4\n\n");
				break;
			default:
				break;

		}
	} while (op != -1);


	if (rom_file != NULL)	{
		int ret_val; // value returned from emulation
		/* Start Virtual Machine */
		ret_val=start_vm();
		/* Error returned if file not valid */
		if (ret_val == -1)
			printf("File not a gb binary\n\n");
		else
			printf("\nThank you for using RealBoy!\n\n");
	}

	return 0;
}
