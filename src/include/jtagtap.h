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

#ifndef __JTAGTAP_H
#define __JTAGTAP_H

#include "gpio_generic.h"

typedef struct jtag_proc_s {
/* Note: Signal names are as for the device under test. */

	void (*jtagtap_reset)(void);

	uint8_t (*jtagtap_next)(const uint8_t TMS, const uint8_t TDI);
/* tap_next executes one state transision in the JTAG TAP state machine:
 * - Ensure TCK is low
 * - Assert the values of TMS and TDI
 * - Assert TCK (TMS and TDO are latched on rising edge
 * - Caputure the value on TDO
 * - Release TCK.
 */

	void (*jtagtap_tms_seq)(uint32_t MS, int ticks);
	void (*jtagtap_tdi_tdo_seq)
	(uint8_t *DO, const uint8_t final_tms, const uint8_t *DI, int ticks);
/* Shift out a sequence on MS and DI, capture data to DO.
 * - This is not endian safe: First byte will always be first shifted out.
 * - DO may be NULL to ignore captured data.
 * - DO may be point to the same address as DI.
 */
	void (*jtagtap_tdi_seq)
	(const uint8_t final_tms, const uint8_t *DI, int ticks);
} jtag_proc_t;

class JtagProc_i {
  public:
/* Note: Signal names are as for the device under test. */

	virtual void jtagtap_reset(void) = 0;

	virtual uint8_t jtagtap_next(const uint8_t TMS, const uint8_t TDI) = 0;
/* tap_next executes one state transision in the JTAG TAP state machine:
 * - Ensure TCK is low
 * - Assert the values of TMS and TDI
 * - Assert TCK (TMS and TDO are latched on rising edge
 * - Caputure the value on TDO
 * - Release TCK.
 */

	virtual void jtagtap_tms_seq(uint32_t MS, int ticks) = 0;
	virtual void jtagtap_tdi_tdo_seq
	(uint8_t *DO, const uint8_t final_tms, const uint8_t *DI, int ticks) = 0;
/* Shift out a sequence on MS and DI, capture data to DO.
 * - This is not endian safe: First byte will always be first shifted out.
 * - DO may be NULL to ignore captured data.
 * - DO may be point to the same address as DI.
 */
	virtual void jtagtap_tdi_seq
	(const uint8_t final_tms, const uint8_t *DI, int ticks) = 0;
};

static inline void jtagtap_soft_reset(JtagProc_i &jtag_proc_r)
{
    jtag_proc_r.jtagtap_tms_seq(0x1F, 6);
}
//  static inline void jtagtap_shift_ir(JtagProc_i &jtag_proc_r)
//  {
//      jtag_proc_r.jtagtap_tms_seq(0x03, 4);
//  }

class JtagProcStub_i {
  public:
    virtual void jtagtap_reset() = 0;
    virtual uint8_t jtagtap_next(const uint8_t TMS, const uint8_t TDI) = 0;
    virtual void jtagtap_tms_seq(uint32_t MS, int ticks) = 0;
    virtual void jtagtap_tdi_tdo_seq
    (uint8_t *DO, const uint8_t final_tms, const uint8_t *DI, int ticks) = 0;
    virtual void jtagtap_tdi_seq
    (const uint8_t final_tms, const uint8_t *DI, int ticks) = 0;
};

template<GpioType GpioObj>
class JtagProc: public JtagProc_i
{
  public:
        JtagProc(){}
	void jtagtap_reset(void)
        {
#ifdef TRST_PORT
          if (platform_hwversion() == 0) {
            volatile int i;
            GpioObj::gpio_set_low(trst_gpio);
            for(i = 0; i < 10000; i++) asm("nop");
            GpioObj::gpio_set_high(trst_gpio);
          }
#endif
          jtagtap_soft_reset(dynamic_cast<JtagProc_i&>(*this));
        }

	uint8_t jtagtap_next(const uint8_t dTMS, const uint8_t dTDI)
        {
          uint16_t ret;
          volatile int32_t cnt;

          GpioObj::gpio_set_value(tms_gpio, dTMS);
          GpioObj::gpio_set_value(tdi_gpio, dTDI);
          GpioObj::gpio_set_high(tck_gpio);
          for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
          ret = GpioObj::gpio_get_value(tdo_gpio);
          GpioObj::gpio_set_low(tck_gpio);
          for(cnt = swd_delay_cnt - 2; cnt > 0; cnt--);

          //DEBUG("jtagtap_next(TMS = %d, TDI = %d) = %d\n", dTMS, dTDI, ret);

          return ret != 0;
          return 0;
        }

	void jtagtap_tms_seq(uint32_t MS, int ticks)
        {
          GpioObj::gpio_set_value(tdi_gpio, 1);
          int data = MS & 1;
          volatile int32_t cnt;
          if (swd_delay_cnt) {
            while(ticks) {
              GpioObj::gpio_set_value(tms_gpio, data);
              GpioObj::gpio_set_high(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
              MS >>= 1;
              data = MS & 1;
              ticks--;
              GpioObj::gpio_set_low(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
            }
          } else {
            while(ticks) {
              GpioObj::gpio_set_value(tms_gpio, data);
              GpioObj::gpio_set_high(tck_gpio);
              MS >>= 1;
              data = MS & 1;
              ticks--;
              GpioObj::gpio_set_low(tck_gpio);
            }
          }
        }

	void jtagtap_tdi_tdo_seq
	(uint8_t *DO, const uint8_t final_tms, const uint8_t *DI, int ticks)
        {
          uint8_t index = 1;
          GpioObj::gpio_set_value(tms_gpio, 0);
          uint8_t res = 0;
          volatile int32_t cnt;
          if (swd_delay_cnt) {
            while(ticks > 1) {
              GpioObj::gpio_set_value(tdi_gpio, *DI & index);
              GpioObj::gpio_set_high(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
              if (GpioObj::gpio_get_value(tdo_gpio)) {
                res |= index;
              }
              if(!(index <<= 1)) {
                *DO = res;
                res = 0;
                index = 1;
                DI++; DO++;
              }
              ticks--;
              GpioObj::gpio_set_low(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
            }
          } else {
            while(ticks > 1) {
              GpioObj::gpio_set_value(tdi_gpio, *DI & index);
              GpioObj::gpio_set_high(tck_gpio);
              if (GpioObj::gpio_get_value(tdo_gpio)) {
                res |= index;
              }
              if(!(index <<= 1)) {
                *DO = res;
                res = 0;
                index = 1;
                DI++; DO++;
              }
              ticks--;
              GpioObj::gpio_set_low(tck_gpio);
            }
          }
          GpioObj::gpio_set_value(tms_gpio, final_tms);
          GpioObj::gpio_set_value(tdi_gpio, *DI & index);
          GpioObj::gpio_set_high(tck_gpio);
          for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
          if (GpioObj::gpio_get_value(tdo_gpio)) {
            res |= index;
          }
          *DO = res;
          GpioObj::gpio_set_low(tck_gpio);
          for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
        }

	void jtagtap_tdi_seq(const uint8_t final_tms, const uint8_t *DI, int ticks)
        {
          uint8_t index = 1;
          volatile int32_t cnt;
          if (swd_delay_cnt) {
            while(ticks--) {
              GpioObj::gpio_set_value(tms_gpio, ticks? 0 : final_tms);
              GpioObj::gpio_set_value(tdi_gpio, *DI & index);
              GpioObj::gpio_set_high(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
              if(!(index <<= 1)) {
                index = 1;
                DI++;
              }
              GpioObj::gpio_set_low(tck_gpio);
              for(cnt = swd_delay_cnt -2 ; cnt > 0; cnt--);
            }
          } else {
            while(ticks--) {
              GpioObj::gpio_set_value(tms_gpio, ticks? 0 : final_tms);
              GpioObj::gpio_set_value(tdi_gpio, *DI & index);
              GpioObj::gpio_set_high(tck_gpio);
              if(!(index <<= 1)) {
                index = 1;
                DI++;
              }
              GpioObj::gpio_set_low(tck_gpio);
            }
          }
        }
  private:
        const GenericGpioType tdi_gpio{TDI_PORT,TDI_PIN};
        const GenericGpioType tms_gpio{TMS_PORT,TMS_PIN};
        const GenericGpioType tdo_gpio{TDO_PORT,TDO_PIN};
        const GenericGpioType tck_gpio{TCK_PORT,TCK_PIN};
        const GenericGpioType trst_gpio{TRST_PORT,TRST_PIN};
};

/* generic soft reset: 1, 1, 1, 1, 1, 0 */
#define jtagtap_soft_reset()	\
	jtag_proc.jtagtap_tms_seq(0x1F, 6)

/* Goto Shift-IR: 1, 1, 0, 0 */
#define jtagtap_shift_ir()		\
	jtag_proc.jtagtap_tms_seq(0x03, 4)

/* Goto Shift-DR: 1, 0, 0 */
#define jtagtap_shift_dr()		\
	jtag_proc.jtagtap_tms_seq(0x01, 3)

/* Goto Run-test/Idle: 1, 1, 0 */
#define jtagtap_return_idle()	\
	jtag_proc.jtagtap_tms_seq(0x01, 2)

# if PC_HOSTED == 1
int platform_jtagtap_init(void);
# else
int jtagtap_init(void);
# endif
#endif

