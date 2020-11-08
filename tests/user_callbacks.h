#ifndef USER_CALLBACKS_H
#define USER_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24.h"

void mock_ce_write(nrf_gpio state);
void mock_spi_xfer(const uint8_t *in, uint8_t *out, size_t xfer_size);
void mock_delay_cb(uint32_t ms);
nrf_gpio mock_irq_read(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
