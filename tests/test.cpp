#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include <stdio.h>

/* Add mock_c */
#include "CppUTestExt/MockSupport_c.h"

#include "NRF24.h"
#include "NRF24_HAL.h"

#include "user_callbacks.h"
}

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
        mock().checkExpectations();
        mock().clear();
    }

    /* Helpers */

    /* NOTE: It seems like variables inside helper functions need to be static */
    void simulateStatus(uint8_t retval) {
        static uint8_t dummy = retval;
        static uint8_t param = NRF_CMD_NOP;

        mock().expectOneCall("mock_spi_xfer")
            .withMemoryBufferParameter("in", (unsigned char *) &param, 1)
            .withOutputParameterReturning("out", (unsigned char *) &dummy, 1)
            .withParameter("xfer_size", 1);
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

TEST(NRF24, callCeWrite)
{
    mock().expectOneCall("mock_ce_write")
            .withParameter("state", GPIO_SET);

    int success = NRF24_init(&radio, mock_spi_xfer,
        mock_ce_write,
        NULL,
        mock_delay_cb);
    
    (void) success;

    NRF24_hal_set_ce(&radio, GPIO_SET);
    
    mock().checkExpectations();
    mock().clear();
}

TEST(NRF24, readIrqSignal)
{
    mock().expectOneCall("mock_irq_read")
            .andReturnValue(GPIO_SET);

    int success = NRF24_init(&radio, mock_spi_xfer,
        mock_ce_write,
        mock_irq_read,
        mock_delay_cb);
    
    (void) success;

    nrf_gpio irq = NRF24_hal_get_irq(&radio);

    CHECK_EQUAL(GPIO_SET, irq);
    
    mock().checkExpectations();
    mock().clear();
}

TEST(NRF24, detectAndClearIrqFlags)
{
    NRF24_init(&radio, mock_spi_xfer, mock_ce_write, mock_irq_read, mock_delay_cb);

    uint8_t rx_interrupt_flag = NRF_RX_DR_IRQ | 0x0E;

    simulateStatus(rx_interrupt_flag);
    
    nrf_irq flag = NRF24_get_irq_flag(&radio);

    mock().checkExpectations();
    mock().clear();
    
    CHECK_EQUAL(NRF_RX_DR_IRQ, flag);

    /* Expect a call to NRF24_get_status */
    simulateStatus(rx_interrupt_flag);

    /* Expect an update to NRF_REG_STATUS register, the interrupt flag is
     * cleared by writing a 1 to that flag. */
    const uint8_t status_reg[] = {
        NRF_CMD_W_REGISTER | NRF_REG_STATUS,
        NRF_RX_DR_IRQ | 0x0E
    };
    uint8_t dummy[] = {0xFF, 0xFF};

    mock().expectOneCall("mock_spi_xfer")
            .withMemoryBufferParameter("in", (unsigned char *) &status_reg, 2)
            .withOutputParameterReturning("out", &dummy, 2)
            .withParameter("xfer_size", 2);

    NRF24_clear_irq_flag(&radio, flag);

    mock().checkExpectations();
    mock().clear();
}
