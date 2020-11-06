#include "CppUTest/TestHarness.h"

#include "CppUTestExt/MockSupport.h"

#include "NRF24.h"

TEST_GROUP(NRF24)
{
    /* Shared resources */
    nrf_radio radio;

    void setup(void)
    {
	radio.write_ce_cb = NULL;
	radio.read_irq_cb = NULL;
	radio.delay_ms_cb = NULL;
	radio.spi_xfer_data_cb = NULL;
    }

    void teardown(void)
    {
	radio.write_ce_cb = NULL;
	radio.read_irq_cb = NULL;
	radio.delay_ms_cb = NULL;
	radio.spi_xfer_data_cb = NULL;

        mock().checkExpectations();
        mock().clear();
    }

    /* Helper functions */
    static void ce_write(nrf_gpio state)
    {
        (void) state;
    }

    static void spi_xfer(const uint8_t *in, uint8_t *out, size_t xfer_size)
    {
        (void) in;
        (void) out;
        (void) xfer_size;
    }

    static void delay_cb(uint32_t ms)
    {
        (void) ms;
    }
};

TEST(NRF24, GivenNoUserCallbacksThenInitFail)
{
    int success = NRF24_init(&radio, NULL, NULL, NULL, NULL);
    
    CHECK_EQUAL(1, success);
}

TEST(NRF24, GivenUserCallbacksThenInitSuccess)
{
    int success = NRF24_init(&radio, spi_xfer, ce_write, NULL, delay_cb);
    
    CHECK_EQUAL(0, success);
}
