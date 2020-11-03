#include "NRF24.h"
#include "NRF24_HAL.h"

void NRF24_hal_spi_xfer(nrf_radio *radio, const void *send, void *rcv, size_t xfer_len)
{
    (void) radio;
    (void) send;
    (void) rcv;
    (void) xfer_len;
}

void NRF24_hal_set_ce(nrf_radio *radio, nrf_gpio state)
{
    (void) radio;
    (void) state;
}

nrf_gpio NRF24_hal_get_irq(nrf_radio *radio)
{
    (void) radio;

    return GPIO_SET;
}

void NRF24_hal_delay(nrf_radio *radio, uint32_t ms)
{
    (void) radio;
    (void) ms;
}
