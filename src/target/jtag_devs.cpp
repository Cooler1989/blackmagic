/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2022  1bitsquared - Rachel Mant <git@dragonmux.network>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "general.h"
#include "jtag_scan.h"
#include "adiv5.h"
#include "jtag_devs.h"

jtag_dev_descr_t dev_descr[] = {
	{.idcode = 0x0BA00477, .idmask = 0x0FFF0FFF,
		.descr = "ADIv5 JTAG-DP port.",
		.handler = adiv5_jtag_dp_handler},
	{.idcode = 0x00000477, .idmask = 0x00000FFF,
		.descr = "Unknown ARM.", .handler = nullptr},
	{.idcode = 0x06410041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, Medium density.", .handler = nullptr},
	{.idcode = 0x06412041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, Low density.", .handler = nullptr},
	{.idcode = 0x06414041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, High density.", .handler = nullptr},
	{.idcode = 0x06416041, .idmask = 0x0FFFFFFF,
		.descr = "STM32L.", .handler = nullptr},
	{.idcode = 0x06418041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, Connectivity Line.", .handler = nullptr},
	{.idcode = 0x06420041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, Value Line.", .handler = nullptr},
	{.idcode = 0x06428041, .idmask = 0x0FFFFFFF,
		.descr = "STM32, Value Line, High density.", .handler = nullptr},
	{.idcode = 0x06411041, .idmask = 0xFFFFFFFF,
		.descr = "STM32F2xx.", .handler = nullptr},
	{.idcode = 0x06422041, .idmask = 0xFFFFFFFF,
		.descr = "STM32F3xx.", .handler = nullptr},
	{.idcode = 0x06413041, .idmask = 0xFFFFFFFF,
		.descr = "STM32F4xx.", .handler = nullptr},
	{.idcode = 0x00000041, .idmask = 0x00000FFF,
		.descr = "STM32 BSD.", .handler = nullptr},
	{.idcode = 0x0BB11477, .idmask = 0xFFFFFFFF,
		.descr = "NPX: LPC11C24.", .handler = nullptr},
	{.idcode = 0x4BA00477, .idmask = 0xFFFFFFFF,
		.descr = "NXP: LPC17xx family.", .handler = nullptr},
	{.idcode = 0x00000093, .idmask = 0x00000FFF,
		.descr = "Xilinx.", .handler = nullptr},
	{.idcode = 0x0000063D, .idmask = 0x00000FFF,
		.descr = "Xambala: RVDBG013.", .handler = nullptr},
	{.idcode = 0x000007A3, .idmask = 0x00000FFF,
		.descr = "Gigadevice BSD.", .handler = nullptr},
/* Just for fun, unsupported */
	{.idcode = 0x8940303F, .idmask = 0xFFFFFFFF, .descr = "ATMega16.", .handler = nullptr},
	{.idcode = 0x0792603F, .idmask = 0xFFFFFFFF, .descr = "AT91SAM9261.", .handler = nullptr},
	{.idcode = 0x20270013, .idmask = 0xFFFFFFFF, .descr = "i80386ex.", .handler = nullptr},
	{.idcode = 0x07B7617F, .idmask = 0xFFFFFFFF, .descr = "BCM2835.", .handler = nullptr},
	{.idcode = 0x4BA00477, .idmask = 0xFFFFFFFF, .descr = "BCM2836.", .handler = nullptr},
	{.idcode = 0, .idmask = 0, .descr = "Unknown", .handler = nullptr},
};
