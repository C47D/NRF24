# NRF24

Platform independent library for NRF24 radios.

# Purpose of this library

Work with NRF24 radios with "any" microcontroller, the user will only need to provide up to four callbacks.

# Examples

Here you can find some use examples:

## STM32 (Using HAL)

```c
/* Callback to control the CE signal */
void nRF24_ce_write(nrf_gpio state)
{
    if (GPIO_CLEAR == state) {
    	HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
    } else {
    	HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
    }
}

/* Callback to transfer/read data via SPI to/from the radio */
void nRF24_default_spi_xfer(const uint8_t *in, uint8_t *out, const size_t xfer_size)
{
    uint32_t timeout_ms = 1000 * 5;
    uint8_t *for_radio = (uint8_t *) in;

    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi1, for_radio, out, xfer_size, timeout_ms);

    while (HAL_SPI_STATE_BUSY == HAL_SPI_GetState(&hspi1)) {
    	/* Wait for the SPI data transfer to be finished. */
    }

    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
}

/* Radio "object" */
nrf_radio radio;

int main(void)
{
    /* Peripheral initialization */

    /* Registering user callbacks
     * read_irq_cb is not used so we set it to NULL */
    NRF24_radio_init(&radio, nRF24_default_spi_xfer,
	nRF24_ce_write, NULL, HAL_Delay);
    
    /* Using the library */
    uint8_t sts = NRF24_get_status(&radio);
    printf("Status: 0x%02X\r\n", sts);

    uint8_t addr_width = NRF24_get_address_width(&radio);
    printf("Address width: %d\r\n", addr_width);

    uint8_t channel = NRF24_get_channel(&radio);
    printf("Channel: %d\r\n", channel);
    
    /* More code */
}
```

# CHANGELOG

v0.1 Public release, a lot to document.
