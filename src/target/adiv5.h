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

#ifndef __ADIV5_H
#define __ADIV5_H

#include "jtag_scan.h"

#include "swdptap.h"

#define ADIV5_APnDP       0x100
#define ADIV5_DP_REG(x)   (x)
#define ADIV5_AP_REG(x)   (ADIV5_APnDP | (x))

#define ADIV5_DP_BANK0    0x00
#define ADIV5_DP_BANK1    0x10
#define ADIV5_DP_BANK2    0x20
#define ADIV5_DP_BANK3    0x30
#define ADIV5_DP_BANK4    0x40

/* ADIv5 DP Register addresses */
#define ADIV5_DP_IDCODE   ADIV5_DP_REG(0x0)
#define ADIV5_DP_ABORT    ADIV5_DP_REG(0x0)
#define ADIV5_DP_CTRLSTAT ADIV5_DP_REG(0x4)
#define ADIV5_DP_TARGETID  (ADIV5_DP_BANK2 | ADIV5_DP_REG(0x4))
#define ADIV5_DP_SELECT   ADIV5_DP_REG(0x8)
#define ADIV5_DP_RDBUFF   ADIV5_DP_REG(0xC)
#define ADIV5_DP_TARGETSEL ADIV5_DP_REG(0xC)

#define ADIV5_DP_VERSION_MASK 0xf000
#define ADIV5_DPv1            0x1000
#define ADIV5_DPv2            0x2000
#define ADIV5_MINDP          0x10000

/* AP Abort Register (ABORT) */
/* Bits 31:5 - Reserved */
#define ADIV5_DP_ABORT_ORUNERRCLR	(1 << 4)
#define ADIV5_DP_ABORT_WDERRCLR		(1 << 3)
#define ADIV5_DP_ABORT_STKERRCLR	(1 << 2)
#define ADIV5_DP_ABORT_STKCMPCLR	(1 << 1)
/* Bits 5:1 - SW-DP only, reserved in JTAG-DP */
#define ADIV5_DP_ABORT_DAPABORT		(1 << 0)

/* Control/Status Register (CTRLSTAT) */
#define ADIV5_DP_CTRLSTAT_CSYSPWRUPACK	(1u << 31)
#define ADIV5_DP_CTRLSTAT_CSYSPWRUPREQ	(1u << 30)
#define ADIV5_DP_CTRLSTAT_CDBGPWRUPACK	(1u << 29)
#define ADIV5_DP_CTRLSTAT_CDBGPWRUPREQ	(1u << 28)
#define ADIV5_DP_CTRLSTAT_CDBGRSTACK	(1u << 27)
#define ADIV5_DP_CTRLSTAT_CDBGRSTREQ	(1u << 26)
/* Bits 25:24 - Reserved */
/* Bits 23:12 - TRNCNT */
#define ADIV5_DP_CTRLSTAT_TRNCNT        (1u << 12)
/* Bits 11:8 - MASKLANE */
#define ADIV5_DP_CTRLSTAT_MASKLANE
/* Bits 7:6 - Reserved in JTAG-DP */
#define ADIV5_DP_CTRLSTAT_WDATAERR	(1u << 7)
#define ADIV5_DP_CTRLSTAT_READOK	(1u << 6)
#define ADIV5_DP_CTRLSTAT_STICKYERR	(1u << 5)
#define ADIV5_DP_CTRLSTAT_STICKYCMP	(1u << 4)
#define ADIV5_DP_CTRLSTAT_TRNMODE_MASK	(3u << 2)
#define ADIV5_DP_CTRLSTAT_STICKYORUN	(1u << 1)
#define ADIV5_DP_CTRLSTAT_ORUNDETECT	(1u << 0)


/* ADIv5 MEM-AP Registers */
#define ADIV5_AP_CSW		ADIV5_AP_REG(0x00)
#define ADIV5_AP_TAR		ADIV5_AP_REG(0x04)
/* 0x08 - Reserved */
#define ADIV5_AP_DRW		ADIV5_AP_REG(0x0C)
#define ADIV5_AP_DB(x)		ADIV5_AP_REG(0x10 + (4*(x)))
/* 0x20:0xF0 - Reserved */
#define ADIV5_AP_CFG		ADIV5_AP_REG(0xF4)
#define ADIV5_AP_BASE		ADIV5_AP_REG(0xF8)
#define ADIV5_AP_IDR		ADIV5_AP_REG(0xFC)

/* Known designers seen in SYSROM-PIDR. Ignore Bit 0 from
 * the designer bits to get JEDEC Ids with bit 7 ignored.*/
#define AP_DESIGNER_FREESCALE    0x00e
#define AP_DESIGNER_TEXAS        0x017
#define AP_DESIGNER_ATMEL        0x01f
#define AP_DESIGNER_STM          0x020
#define AP_DESIGNER_CYPRESS      0x034
#define AP_DESIGNER_INFINEON     0x041
#define AP_DESIGNER_NORDIC       0x244
#define AP_DESIGNER_ARM          0x43b
/*LPC845 with designer 501. Strange!? */
#define AP_DESIGNER_SPECULAR     0x501
#define AP_DESIGNER_CS           0x555
#define AP_DESIGNER_ENERGY_MICRO 0x673
#define AP_DESIGNER_GIGADEVICE   0x751
#define AP_DESIGNER_RASPBERRY    0x927

/* AP Control and Status Word (CSW) */
#define ADIV5_AP_CSW_DBGSWENABLE	(1u << 31)
/* Bits 30:24 - Prot, Implementation defined, for Cortex-M3: */
#define ADIV5_AP_CSW_MASTERTYPE_DEBUG	(1u << 29)
#define ADIV5_AP_CSW_HPROT1		(1u << 25)
#define ADIV5_AP_CSW_SPIDEN		(1u << 23)
/* Bits 22:12 - Reserved */
/* Bits 11:8 - Mode, must be zero */
#define ADIV5_AP_CSW_TRINPROG		(1u << 7)
#define ADIV5_AP_CSW_DEVICEEN		(1u << 6)
#define ADIV5_AP_CSW_ADDRINC_NONE	(0u << 4)
#define ADIV5_AP_CSW_ADDRINC_SINGLE	(1u << 4)
#define ADIV5_AP_CSW_ADDRINC_PACKED	(2u << 4)
#define ADIV5_AP_CSW_ADDRINC_MASK	(3u << 4)
/* Bit 3 - Reserved */
#define ADIV5_AP_CSW_SIZE_BYTE		(0u << 0)
#define ADIV5_AP_CSW_SIZE_HALFWORD	(1u << 0)
#define ADIV5_AP_CSW_SIZE_WORD		(2u << 0)
#define ADIV5_AP_CSW_SIZE_MASK		(7u << 0)

/* AP Debug Base Address Register (BASE) */
#define ADIV5_AP_BASE_BASEADDR		(0xFFFFF000u)
#define ADIV5_AP_BASE_PRESENT		(1u << 0)


/* ADIv5 Class 0x1 ROM Table Registers */
#define ADIV5_ROM_MEMTYPE			0xFCC
#define ADIV5_ROM_MEMTYPE_SYSMEM	(1u << 0)
#define ADIV5_ROM_ROMENTRY_PRESENT  (1u << 0)
#define ADIV5_ROM_ROMENTRY_OFFSET	(0xFFFFF000u)


/* Constants to make RnW parameters more clear in code */
#define ADIV5_LOW_WRITE		0
#define ADIV5_LOW_READ		1

using AckBits = uint32_t;/*std::bitset<32>*/
#define SWDP_ACK_OK    0x01
#define SWDP_ACK_WAIT  0x02
#define SWDP_ACK_FAULT 0x04

enum align {
	ALIGN_BYTE     = 0,
	ALIGN_HALFWORD = 1,
	ALIGN_WORD     = 2,
	ALIGN_DWORD    = 3
};

typedef struct ADIv5_AP_s ADIv5_AP_t;

class ADI_v5_AP;

/* Try to keep this somewhat absract for later adding SW-DP */
typedef struct ADIv5_DP_s {
	int refcnt;

	uint32_t idcode;
	uint32_t targetid;  /* Contains IDCODE for DPv2 devices.*/

        // ##########    low level sequence control    ############
	void (*seq_out)(uint32_t MS, int ticks);
	void (*seq_out_parity)(uint32_t MS, int ticks);
	uint32_t (*seq_in)(int ticks);
	bool (*seq_in_parity)(uint32_t *ret, int ticks);
	/* dp_low_write returns true if no OK resonse, but ignores errors */
	bool (*dp_low_write)(struct ADIv5_DP_s *dp, uint16_t addr,
						 const uint32_t data);
	uint32_t (*dp_read)(struct ADIv5_DP_s *dp, uint16_t addr);
	uint32_t (*error)(struct ADIv5_DP_s *dp);
	uint32_t (*low_access)(struct ADIv5_DP_s *dp, uint8_t RnW,
                               uint16_t addr, uint32_t value);
	void (*abort)(struct ADIv5_DP_s *dp, uint32_t abort);

#if PC_HOSTED == 1
	bmp_type_t dp_bmp_type;
	bool (*ap_setup)(int i);
	void (*ap_cleanup)(int i);
        void (*ap_regs_read)(ADIv5_AP_t *ap, void *data);
        uint32_t(*ap_reg_read)(ADIv5_AP_t *ap, int num);
        void (*ap_reg_write)(ADIv5_AP_t *ap, int num, uint32_t value);
	void (*read_block)(uint32_t addr, uint8_t *data, int size);
	void (*dap_write_block_sized)(uint32_t addr, uint8_t *data,
								  int size, enum align align);
#endif
	uint32_t (*ap_read)(ADIv5_AP_t *ap, uint16_t addr);
	void (*ap_write)(ADIv5_AP_t *ap, uint16_t addr, uint32_t value);

	void (*mem_read)(ADIv5_AP_t *ap, void *dest, uint32_t src, size_t len);
	void (*mem_write_sized)(ADIv5_AP_t *ap, uint32_t dest, const void *src,
							size_t len, enum align align);
	uint8_t dp_jd_index;
	uint8_t fault;

        ADI_v5_AP *ap = nullptr;
} ADIv5_DP_t;

class AdiV5Swdp
{
  public:
    uint32_t error();
    uint32_t low_access(uint8_t RnW, uint16_t addr, uint32_t value);
    void dp_write(uint16_t addr, uint32_t value);
    uint32_t dp_read(uint16_t addr);  //  this is replaced by compile time symbol when Hosted is used;
    bool dp_low_write(uint16_t addr, const uint32_t data);
    unsigned int make_packet_request(uint8_t RnW, uint16_t addr);
    void abort(uint32_t abort);
    int swdp_scan(uint32_t targetid);

    void ref_inc()
    {
	refcnt_++;
    }
    void ref_dec()
    {
	if (--(refcnt_) == 0)
        {
            //  free(dp);
        }
    }

    int refcnt_;
    uint32_t idcode_;
    uint8_t fault_;

    uint32_t targetid_;  /* Contains IDCODE for DPv2 devices.*/
  private:
    void dp_line_reset();
    uint32_t swdp_low_access(uint8_t RnW, uint16_t addr, uint32_t value);


    ADIv5DpHwLayer_i& swdp_tap = SwdpTapInstance();
    ADIv5_DP_t *initial_dp = nullptr;
};

struct ADIv5_AP_s {
	int refcnt;

	ADIv5_DP_t *dp;
	uint8_t apsel;

	uint32_t idr;
	uint32_t base;
	uint32_t csw;
	uint32_t ap_cortexm_demcr; /* Copy of demcr when starting */
	uint32_t ap_storage; /* E.g to hold STM32F7 initial DBGMCU_CR value.*/
	uint16_t ap_designer;
	uint16_t ap_partno;
};

class ADI_v5_AP_Interface
{
  public:
    ADI_v5_AP_Interface (AdiV5Swdp& dp): dp_class_(dp) {}

    void ap_write(uint16_t addr, uint32_t value);
    uint32_t ap_read(uint16_t addr);
    void mem_write(uint16_t dest, const void *src, size_t len);
    void mem_write_sized(uint32_t dest, const void *src, size_t len, enum align align);
    void mem_read(void *dest, uint32_t src, size_t len);
    void mem_access_setup(uint32_t addr, enum align align);
  private:
    AdiV5Swdp& dp_class_;
};

class ADI_v5_AP
{
  public:
    ADI_v5_AP (AdiV5Swdp& dp, uint8_t ap_sel): apsel_(ap_sel), dp_class_(dp) {}

    ADI_v5_AP (AdiV5Swdp& dp, ADIv5_AP_t &tmp):
      apsel_(tmp.apsel),
      idr(tmp.idr),
      base(tmp.base),
      csw(tmp.csw),
      dp_class_(dp)
  {}

    void ap_write(uint16_t addr, uint32_t value);
    uint32_t ap_read(uint16_t addr);
    void mem_write(uint32_t dest, const void *src, size_t len);
    void mem_write_sized(uint32_t dest, const void *src, size_t len, enum align align);
    void mem_read(void *dest, uint32_t src, size_t len);
    uint32_t mem_read32(uint32_t src);
    uint32_t ap_read_id(uint32_t addr);
    uint64_t ap_read_pidr(uint32_t addr);
    void mem_access_setup(uint32_t addr, enum align align);

    AdiV5Swdp& get_dp() const
    {
        return dp_class_;
    }

    void ref_inc();
    void ref_dec()
    {
	if (--(refcnt) == 0) {
                dp_class_.ref_dec();
		//free(ap);
	}
    }

    int refcnt = 0;

    //TODO: use reference to DP
    //ADIv5_DP_t *dp;
    uint8_t apsel_;

    uint32_t idr;
    uint32_t base;
    uint32_t csw;
    uint32_t ap_cortexm_demcr; /* Copy of demcr when starting */
    uint32_t ap_storage; /* E.g to hold STM32F7 initial DBGMCU_CR value.*/
    uint16_t ap_designer;
    uint16_t ap_partno;

    uint32_t update_idr(uint16_t addr)
    {
        idr = ap_read(addr);
	return idr;
    }
    uint32_t update_base(uint16_t addr)
    {
        base = ap_read(addr);
	return base;
    }
    uint32_t update_csw()
    {
        csw = ap_read(ADIV5_AP_CSW) &
		~(ADIV5_AP_CSW_SIZE_MASK | ADIV5_AP_CSW_ADDRINC_MASK);
	return csw;
    }
    void component_probe(uint32_t addr, int recursion, int num_entry);
  private:

    AdiV5Swdp& dp_class_;
};

#if PC_HOSTED == 0
static inline uint32_t adiv5_dp_read(ADIv5_DP_t *dp, uint16_t addr)
{
	return dp->dp_read(dp, addr);
}

static inline uint32_t adiv5_dp_error(ADIv5_DP_t *dp)
{
	return dp->error(dp);
}

static inline uint32_t adiv5_dp_low_access(struct ADIv5_DP_s *dp, uint8_t RnW,
                                           uint16_t addr, uint32_t value)
{
	return dp->low_access(dp, RnW, addr, value);
}

static inline void adiv5_dp_abort(struct ADIv5_DP_s *dp, uint32_t abort)
{
	return dp->abort(dp, abort);
}

static inline uint32_t adiv5_ap_read(ADIv5_AP_t *ap, uint16_t addr)
{
	return ap->dp->ap_read(ap, addr);
}

static inline void adiv5_ap_write(ADIv5_AP_t *ap, uint16_t addr, uint32_t value)
{
	return ap->dp->ap_write(ap, addr, value);
}

static inline void adiv5_mem_read(ADIv5_AP_t *ap, void *dest, uint32_t src,
								  size_t len)
{
	return ap->dp->mem_read(ap, dest, src, len);
}

static inline void adiv5_mem_write_sized(
	ADIv5_AP_t *ap, uint32_t dest, const void *src, size_t len,
	enum align align)
{
	return ap->dp->mem_write_sized(ap, dest, src, len, align);
}

static inline void adiv5_dp_write(ADIv5_DP_t *dp, uint16_t addr, uint32_t value)
{
	dp->low_access(dp, ADIV5_LOW_WRITE, addr, value);
}

#else
uint32_t adiv5_dp_read(ADIv5_DP_t *dp, uint16_t addr);
uint32_t adiv5_dp_error(ADIv5_DP_t *dp);
uint32_t adiv5_dp_low_access(struct ADIv5_DP_s *dp, uint8_t RnW,
							 uint16_t addr, uint32_t value);
void adiv5_dp_abort(struct ADIv5_DP_s *dp, uint32_t abort);
uint32_t adiv5_ap_read(ADIv5_AP_t *ap, uint16_t addr);
void adiv5_ap_write(ADIv5_AP_t *ap, uint16_t addr, uint32_t value);
void adiv5_mem_read(ADIv5_AP_t *ap, void *dest, uint32_t src, size_t len);
void adiv5_mem_write_sized(ADIv5_AP_t *ap, uint32_t dest,
						   const void *src, size_t len, enum align align);
void adiv5_dp_write(ADIv5_DP_t *dp, uint16_t addr, uint32_t value);
#endif

void adiv5_dp_init(ADIv5_DP_t *dp);
void adiv5_dp_init(ADIv5_DP_t *dp, AdiV5Swdp& dp_class);
void platform_adiv5_dp_defaults(ADIv5_DP_t *dp);
void remote_jtag_dev(const jtag_dev_t *jtag_dev);
void adiv5_ap_ref(ADIv5_AP_t *ap);
void adiv5_ap_unref(ADIv5_AP_t *ap);
void platform_add_jtag_dev(const int dev_index, const jtag_dev_t *jtag_dev);

void adiv5_jtag_dp_handler(uint8_t jd_index);
int platform_jtag_dp_init(ADIv5_DP_t *dp);
int swdptap_init(ADIv5_DP_t *dp);

void adiv5_mem_write(ADIv5_AP_t *ap, uint32_t dest, const void *src, size_t len);
uint64_t adiv5_ap_read_pidr(ADIv5_AP_t *ap, uint32_t addr);
void * extract(void *dest, uint32_t src, uint32_t val, enum align align);

void firmware_mem_write_sized(ADIv5_AP_t *ap, uint32_t dest, const void *src,
							  size_t len, enum align align);
void firmware_mem_read(ADIv5_AP_t *ap, void *dest, uint32_t src,
					   size_t len);
void firmware_ap_write(ADIv5_AP_t *ap, uint16_t addr, uint32_t value);
uint32_t firmware_ap_read(ADIv5_AP_t *ap, uint16_t addr);
uint32_t firmware_swdp_low_access(ADIv5_DP_t *dp, uint8_t RnW,
								  uint16_t addr, uint32_t value);
uint32_t fw_adiv5_jtagdp_low_access(ADIv5_DP_t *dp, uint8_t RnW,
					uint16_t addr, uint32_t value);
uint32_t firmware_swdp_read(ADIv5_DP_t *dp, uint16_t addr);
uint32_t fw_adiv5_jtagdp_read(ADIv5_DP_t *dp, uint16_t addr);

//  uint32_t firmware_swdp_error(ADIv5_DP_t *dp);

void firmware_swdp_abort(ADIv5_DP_t *dp, uint32_t abort);
void adiv5_jtagdp_abort(ADIv5_DP_t *dp, uint32_t abort);
#endif
