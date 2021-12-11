#ifndef ATMEL___H
#define ATMEL___H

#include "jtag_scan.h"
#include "target.h"

typedef struct Atmel_DP_s {
	int refcnt;

	uint32_t idcode;

	uint8_t dp_jd_index;
} AVR_DP_t;

bool avr_dp_init(AVR_DP_t *dp);

void avr_jtag_dp_handler(uint8_t jd_index, uint32_t j_idcode);
int platform_avr_jtag_dp_init(AVR_DP_t *dp);
void avr_add_flash(target *t, uint32_t start, size_t length);

bool avr_pdi_reg_write(AVR_DP_t *dp, uint8_t reg, uint8_t value);
uint8_t avr_pdi_reg_read(AVR_DP_t *dp, uint8_t reg);

#endif /*ATMEL___H*/