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
#include "gboy_mbc.h"

#define MBC_2 1
#define MBC_5 3

/*
 * Initialize gb_mbc structure.
 */


void generic_write(Uint8 *host_addr, int val){
	*host_addr = val;
}

/*
 * General RAM remap function.
 */
void
mbc_ram_remap()
{
	addr_sp_ptrs[0xa] = addr_sp_ptrs[0xb] = ((long)((&gb_cart.cart_ram_banks[0x2000*gb_cart.cart_curam_bank])-0xa000));
}

/*
 * General ROM remap function.
 */
void
mbc_rom_remap()
{
	addr_sp_ptrs[7] = addr_sp_ptrs[4] = addr_sp_ptrs[5] = addr_sp_ptrs[6] = ((long)((&gb_cart.cart_rom_banks[0x4000*(gb_cart.cart_curom_bank-1)])-0x4000));
}


/*
 * Main structure holding the various mbcX-specific functions.
 */
static void (*mbc_def_funcs[4][6])(Uint8*, int) = { mbc1_ram_en, mbc1_rom_bank, mbc1_ram_bank, mbc1_mode, NULL, NULL,
											mbc2_ram_en, mbc2_rom_bank, mbc2_dummy, mbc2_dummy, mbc2_ram_wr, mbc2_dummy,
											mbc3_ramtim_en, mbc3_rom_bank, mbc3_ramrtc_bank, mbc3_clk, NULL, NULL,
											mbc5_ram_en, mbc5_rom_bank_low, mbc5_rom_bank_high, mbc5_ram_bank, mbc5_dummy, NULL };


//NEW FUNCTION
//A0 - A1 && A2 - BF -> 0xA-BF
void specialCaseForMBC2(Uint8* host_addr, int val){

	if(mbc_num_global_var == MBC_2 && gb_addr_global_var > 0x9F00 && gb_addr_global_var < 0xA200)
		mbc_def_funcs[mbc_num_global_var][4](host_addr, val);
	else if(mbc_num_global_var == MBC_2 && gb_addr_global_var > 0xA100 && gb_addr_global_var < 0xC000)
		mbc_def_funcs[mbc_num_global_var][5](host_addr, val);
	else
		generic_write(host_addr, val);
}

//NEW FUNCTION
//0xF address
void specialCaseIOControlWrite(Uint8* host_addr, int val){
	if (gb_addr_global_var >= 0xff00 && gb_addr_global_var < 0xff80)
		io_ctrl_wr(gb_addr_global_var&0xff, val);
	else
		generic_write(host_addr, val);
}


void
mbc_init(int mbc_num)
{
	switch (mbc_num) {
		case 1:
		case 2:
		case 3:
			mbc_num = 0;
			break;
		case 5:
		case 6:
			mbc_num = 1;
			break;
		case 0x0f:
		case 0x10:
			mbc3_read_rtc();
		case 0x11:
		case 0x12:
		case 0x13:
			mbc_num = 2;
			break;
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
			mbc_num = 3;
			break;
	};
	mbc_num_global_var = mbc_num;
	//TODO: I SHALL PUT A THE mbc_funcs SMALLER!
	//Dividir de de FF -> F e por um if para lidar com os casos adicionais do mbc2 (0xA até 0xC)
	
	int i;
	if (mbc_num == MBC_5) {
		for(i = 0; i <= 0xF; i++){
			if(i < 0x2)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][0];
			else if(i < 0x3)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][1];
			else if(i < 0x4)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][2];
			else if(i < 0x6)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][3];
			else if(i < 0x8)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][4];
			else
				gb_mbc.mbc_funcs[i] = generic_write;
		}
	}
	else {
		for(i = 0; i <= 0xF; i++){
			if(i < 0x2)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][0];
			else if(i < 0x4)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][1];
			else if(i < 0x6)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][2];
			else if(i < 0x8)
				gb_mbc.mbc_funcs[i] = mbc_def_funcs[mbc_num][3];
			else if(i > 0x9 && i < 0xC)// 0xA-BF
				gb_mbc.mbc_funcs[i] = specialCaseForMBC2;
			else if (i == 0xF)
				gb_mbc.mbc_funcs[i] = specialCaseIOControlWrite;
			else
				gb_mbc.mbc_funcs[i] = generic_write;//0x8-0x9 && 0xC-0xE
		}
	}
}













