#include "CppUTestExt/MockSupport_c.h"

#include "NRF24.h"
#include "NRF24_HAL.h"

#include "user_callbacks.h"

void mock_ce_write(nrf_gpio state)
{
    mock_c()->actualCall(__func__)
            ->withUnsignedIntParameters("state", state);
}

void mock_spi_xfer(const uint8_t *in, uint8_t *out, size_t xfer_size)
{
    mock_c()->actualCall(__func__)
            ->withMemoryBufferParameter("in", in, xfer_size)
            ->withOutputParameter("out", out)
            ->withUnsignedIntParameters("xfer_size", (unsigned int) xfer_size);
}

void mock_delay_cb(uint32_t ms)
{
    mock_c()->actualCall(__func__)
            ->withUnsignedIntParameters("ms", ms);
}

nrf_gpio mock_irq_read(void)
{
    mock_c()->actualCall(__func__);

    return (nrf_gpio) mock_c()->returnValue().value.unsignedIntValue;
}
