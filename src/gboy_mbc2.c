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
#include "gboy_mbc2.h"

void
mbc2_rom_bank(int val)
{
	gb_cart.cart_curom_bank = val & 0xf; // update current ROM bank
	mbc_rom_remap();
}

void
mbc2_ram_en(int val)
{
	/* Sync RAM file */
	if (((val&=0xf) != 0xa) && (gb_cart.cart_ram_banks!=NULL)) {
		rewind(gb_cart.cart_ram_fd);
		fwrite(gb_cart.cart_ram_banks, 1, 1024*gb_cart.cart_ram_size, gb_cart.cart_ram_fd);
	}
}

void
mbc2_ram_wr(int val)
{
	//Its not used... for now...
}
