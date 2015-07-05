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



#define NUMBER_OF_INSTRUCTIONS 512
//note: it is created a new scope ( { } ), because of the temporary values lo and hi
#define GET_REAL_CPU_TICKS(CPUToalTicks) { \
     Uint32 lo, hi; \
     asm volatile("cpuid \n\t rdtsc" : "=a" (lo), "=d" (hi)); \
     CPUToalTicks = ( ((realCpuTicks)lo) | ( ((realCpuTicks)hi ) << 32) ); \
}

typedef unsigned long long realCpuTicks;


struct profilerInfo {
	realCpuTicks instruction_counter;
	realCpuTicks instruction_time_counter;
};





