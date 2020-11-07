#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
/* Add mock_c */
#include "CppUTestExt/MockSupport_c.h"

#include "NRF24.h"
#include "NRF24_HAL.h"

#include "user_callbacks.h"

#include "fff.h"
DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(int, myfake, int)
}

TEST_GROUP(NRF24)
{
    /* Shared resources */
    nrf_radio radio;

    void setup(void)
    {
        /* Reset fakes */
        myfake_reset();

        /* Reset common fff internal structures */
        FFF_RESET_HISTORY();

	radio.write_ce_cb = NULL;
	radio.read_irq_cb = NULL;
	radio.delay_ms_cb = NULL;
	radio.spi_xfer_data_cb = NULL;
    }

    void teardown(void)
    {
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
    int success = NRF24_init(&radio, mock_spi_xfer,
        mock_ce_write,
        NULL,
        mock_delay_cb);
    
    CHECK_EQUAL(0, success);
}

TEST(NRF24, testceWrite)
{
    mock().expectOneCall("mock_ce_write")
            .withParameter("state", GPIO_SET);

    nrf_radio test_radio;

    int success = NRF24_init(&test_radio, mock_spi_xfer,
        mock_ce_write,
        NULL,
        mock_delay_cb);
    
    (void) success;

    /* When calling test_radio.write_ce_cb we fulfill the expectations */
    // test_radio.write_ce_cb(GPIO_SET);

    /* When calling NRF24_hal_set_ce we do not fulfill the expectations */
    NRF24_hal_set_ce(&test_radio, GPIO_SET);
}
