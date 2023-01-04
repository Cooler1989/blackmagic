/*
 * This file is part of the Black Magic Debug project.
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
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

/* This file implements a transparent channel over which the GDB Remote
 * Serial Debugging protocol is implemented.  This implementation for STM32
 * uses the USB CDC-ACM device bulk endpoints to implement the channel.
 */
#include "general.h"
#include "cdcacm.h"
#include "gdb_if.h"

#include "data_out_in_count.h"
#include "data_output.h"

static uint32_t count_out = 0;
static uint32_t count_in = 0;
static uint32_t count_total_in = 0;
static uint32_t save_group_start = 0;
static uint32_t out_ptr;
static uint8_t buffer_out[CDCACM_PACKET_SIZE];
static uint8_t buffer_in[CDCACM_PACKET_SIZE];
#ifdef STM32F4
static volatile uint32_t count_new;
static uint8_t double_buffer_out[CDCACM_PACKET_SIZE];
#endif

uint32_t output_group_data_index = 1;  // odd/even starts

void gdb_if_putchar(unsigned char c, int flush)
{
  (void) flush;

  buffer_in[count_in++] = c;
  if( output_input_count[output_group_data_index] == count_in )
  {
    volatile int result = memcmp(buffer_in, &(data_serial_output[save_group_start]),
          output_input_count[output_group_data_index] );  // size
    save_group_start = count_total_in;
    while(result);
    volatile bool check_flush = !(bool)flush;
    while(check_flush);
  }

  count_total_in++;

  return;
}

//  void gdb_if_putchar(unsigned char c, int flush)
//  {
//  	buffer_in[count_in++] = c;
//  	if(flush || (count_in == CDCACM_PACKET_SIZE)) {
//  		/* Refuse to send if USB isn't configured, and
//  		 * don't bother if nobody's listening */
//  		if((cdcacm_get_config() != 1) || !cdcacm_get_dtr()) {
//  			count_in = 0;
//  			return;
//  		}
//  		while(usbd_ep_write_packet(usbdev, CDCACM_GDB_ENDPOINT,
//  			buffer_in, count_in) <= 0);
//
//  		if (flush && (count_in == CDCACM_PACKET_SIZE)) {
//  			/* We need to send an empty packet for some hosts
//  			 * to accept this as a complete transfer. */
//  			/* libopencm3 needs a change for us to confirm when
//  			 * that transfer is complete, so we just send a packet
//  			 * containing a null byte for now.
//  			 */
//  			while (usbd_ep_write_packet(usbdev, CDCACM_GDB_ENDPOINT,
//  				"\0", 1) <= 0);
//  		}
//
//  		count_in = 0;
//  	}
//  }

#ifdef STM32F4
void gdb_usb_out_cb(usbd_device *dev, uint8_t ep)
{
	(void)ep;
	usbd_ep_nak_set(dev, CDCACM_GDB_ENDPOINT, 1);
	count_new = usbd_ep_read_packet(dev, CDCACM_GDB_ENDPOINT,
	                                double_buffer_out, CDCACM_PACKET_SIZE);
	if(!count_new) {
		usbd_ep_nak_set(dev, CDCACM_GDB_ENDPOINT, 0);
	}
}
#endif

static void gdb_if_update_buf(void)
{
    return;
	while (cdcacm_get_config() != 1);
#ifdef STM32F4
	asm volatile ("cpsid i; isb");
	if (count_new) {
		memcpy(buffer_out, double_buffer_out, count_new);
		count_out = count_new;
		count_new = 0;
		out_ptr = 0;
		usbd_ep_nak_set(usbdev, CDCACM_GDB_ENDPOINT, 0);
	}
	asm volatile ("cpsie i; isb");
#else
	count_out = usbd_ep_read_packet(usbdev, CDCACM_GDB_ENDPOINT,
	                                buffer_out, CDCACM_PACKET_SIZE);
	out_ptr = 0;
#endif
}


#include "data_include_file.h"

unsigned char gdb_if_getchar(void)
{

        gdb_if_update_buf();
        static size_t data_ptr = 0;
        if( data_ptr <= sizeof(data_serial) )
        {
            static volatile int cnt;
            cnt = 1000;
            while(cnt-- > 0);

            return data_serial[data_ptr++];
        }
        else
        {
          while(1);
        }
	while (!(out_ptr < count_out)) {
		/* Detach if port closed */
		if (!cdcacm_get_dtr())
			return 0x04;

		gdb_if_update_buf();
	}

	return buffer_out[out_ptr++];
}

unsigned char gdb_if_getchar_to(int timeout)
{
        return gdb_if_getchar();

	platform_timeout t;
	platform_timeout_set(&t, timeout);

	if (!(out_ptr < count_out)) do {
		/* Detach if port closed */
		if (!cdcacm_get_dtr())
			return 0x04;

		gdb_if_update_buf();
	} while (!platform_timeout_is_expired(&t) && !(out_ptr < count_out));

	if(out_ptr < count_out)
		return gdb_if_getchar();

	return -1;
}

