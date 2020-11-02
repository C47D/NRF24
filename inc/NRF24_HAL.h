#ifndef NRF24_HAL
#define NRF24_HAL

#include "NRF24.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

void NRF24_hal_spi_xfer(nrf_radio *radio, const void *send, void *rcv, size_t xfer_len);
void NRF24_hal_set_ce(nrf_radio *radio, nrf_gpio state);
nrf_gpio NRF24_hal_get_irq(nrf_radio *radio);
void NRF24_hal_delay(nrf_radio *radio, uint32_t ms);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NRF24_HAL */
