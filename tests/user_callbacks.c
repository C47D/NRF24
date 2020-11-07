#include "CppUTestExt/MockSupport_c.h"

#include "NRF24.h"
#include "NRF24_HAL.h"

#include "user_callbacks.h"

void mock_ce_write(nrf_gpio state)
{
    mock_c()->actualCall("mock_ce_write")
            ->withIntParameters("state", state);
}

void mock_spi_xfer(const uint8_t *in, uint8_t *out, size_t xfer_size)
{
    (void) in;
    (void) out;
    (void) xfer_size;
}

void mock_delay_cb(uint32_t ms)
{
    (void) ms;
}
