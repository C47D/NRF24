#include "NRF24_HAL.h"

void NRF24_hal_spi_xfer(nrf_radio *radio, const void *send, void *rcv, size_t xfer_len)
{
    radio->spi_xfer_data_cb(send, rcv, xfer_len);
}

void NRF24_hal_set_ce(nrf_radio *radio, nrf_gpio state)
{
    radio->write_ce_cb(state);
}

nrf_gpio NRF24_hal_get_irq(nrf_radio *radio)
{
    return radio->read_irq_cb();
}

void NRF24_hal_delay(nrf_radio *radio, uint32_t ms)
{
	radio->delay_ms_cb(ms);
}
