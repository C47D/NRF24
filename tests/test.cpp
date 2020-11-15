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

    uint8_t retval = 0x0F;
    const uint8_t param = NRF_CMD_NOP;
    mock().expectOneCall("mock_spi_xfer")
            .withMemoryBufferParameter("in", &param, 1)
            .withOutputParameterReturning("out", &retval, 1)
            .withParameter("xfer_size", 1);
    
    uint8_t status = NRF24_get_status(&radio);
    printf("Status: 0x%02X", status);

    mock().checkExpectations();
    mock().clear();
}

TEST(NRF24, detectAndClearIrqFlags)
{
    int success = NRF24_init(&radio, mock_spi_xfer,
        mock_ce_write,
        mock_irq_read,
        mock_delay_cb);
    
    (void) success;
    
    uint8_t retval = 0x0F;
    const uint8_t param = NRF_CMD_NOP;
    mock().expectOneCall("mock_spi_xfer")
            .withMemoryBufferParameter("in", &param, 1)
            .withOutputParameterReturning("out", &retval, 1)
            .withParameter("xfer_size", 1);
    
    uint8_t status = NRF24_get_status(&radio);
    (void) status;

    mock().checkExpectations();
    mock().clear();

    uint8_t irq_shifted = (1U << NRF_RX_DR_IRQ);
    
    uint8_t irq_retval = irq_shifted | 0x0E;
    mock().expectOneCall("mock_spi_xfer")
            .withMemoryBufferParameter("in", &param, 1)
            .withOutputParameterReturning("out", &irq_retval, 1)
            .withParameter("xfer_size", 1);
    
    nrf_irq flag = NRF24_get_irq_flag(&radio);
    printf("Flag: %x\r\n", (int) flag);

    mock().checkExpectations();
    mock().clear();
    
    // NRF_MAX_RT_IRQ  = 4, // 0x10
    // NRF_TX_DS_IRQ   = 5, // 0x20
    // NRF_RX_DR_IRQ   = 6, // 0x40
    
    if ((1 << NRF_MAX_RT_IRQ) & flag) {
        printf("NRF_MAX_RT_IRQ\r\n");
    }

    if ((1 << NRF_TX_DS_IRQ) & flag) {
        printf("NRF_TX_DS_IRQ\r\n");
    }

    if ((1 << NRF_RX_DR_IRQ) & flag) {
        printf("NRF_RX_DR_IRQ\r\n");
    }

    CHECK_EQUAL(irq_shifted, flag);
}
