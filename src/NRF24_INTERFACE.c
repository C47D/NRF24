/**
* @file     NRF24_INTERFACE.c
* @version  0.1
*
* If you want more control over the radio (no public user API aka NRF24) between
* you and your NRF24 radio you can use this set of functions.
*/

#include "NRF24.h"
#include "NRF24_INTERFACE.h"

/* Static functions */
typedef enum {
	NRF_BIT_CLEAR	= 0,
	NRF_BIT_SET		= 1
} nrf_bit;

static void NRF24_write_bit(nrf_radio *radio, const nrf_register reg,
	const uint8_t bit_pos, const nrf_bit value);

uint8_t NRF24_read_reg(nrf_radio *radio, const nrf_register reg,
    uint8_t *data, const size_t data_size)
{
    uint8_t data_out[data_size + 1];
    uint8_t data_in[data_size + 1];
    
    data_in[0] = NRF_CMD_R_REGISTER | reg;
    NRF24_hal_spi_xfer(radio, data_in, data_out, NRF_ARRAY_SIZE(data_in));

    for (size_t idx = 0; idx < data_size; idx++) {
    	data[idx] = data_out[idx + 1];
    }
    
    return data_out[0];
}

uint8_t NRF24_write_reg(nrf_radio *radio, const nrf_register reg,
    const uint8_t *data, const size_t data_size)
{
    uint8_t data_out[data_size + 1];
    uint8_t data_in[data_size + 1];
    
    data_in[0] = NRF_CMD_W_REGISTER | reg;

    for (size_t idx = 0; idx < data_size; idx++) {
    	data_in[idx + 1] = data[idx];
    }

    NRF24_hal_spi_xfer(radio, data_in, data_out, NRF_ARRAY_SIZE(data_in));
    
    return data_out[0];
}

uint8_t NRF24_read_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos)
{
	NRF24_ASSERT(8 > bit_pos);

    uint8_t reg_val;
    NRF24_read_reg(radio, reg, &reg_val, 1);
    return (reg_val & (1 << bit_pos)) != 0;
}

void NRF24_clear_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos)
{
    NRF24_ASSERT(8 > bit_pos);

    NRF24_write_bit(radio, reg, bit_pos, NRF_BIT_CLEAR);
}

void NRF24_set_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos)
{
	NRF24_ASSERT(8 > bit_pos);

    NRF24_write_bit(radio, reg, bit_pos, NRF_BIT_SET);
}

/* Value must be shifted already, so we can OR values,
 * mask can be also ORed togheter. */
void NRF24_write_bits(nrf_radio *radio, const nrf_register reg, uint8_t mask, uint8_t value)
{
    NRF24_ASSERT(radio);
    NRF24_ASSERT(value <= UINT8_MAX);

    /* Get the current register value */
    uint8_t reg_value = 0;
    NRF24_read_reg(radio, reg, &reg_value, sizeof reg_value);
    
    /* Invert the mask, when we AND it with the register value we set
     * the masked bits to zero. */
    uint8_t new_value = reg_value & (~mask);

    /* Then we can 'add' the value we want */
    new_value |= value;

    /* Write the new value back to the radio */
    NRF24_write_reg(radio, reg, &new_value, sizeof new_value);
}

/**
 * Set (1) or clear (0) the specified bit of the specified NRF24 register.
 *
 * First we read the specified register and check the content of the specified
 * bit, exit early if the bit already is the value we wanted, otherwise we set
 * the bit to the specified value.
 *
 * @param[in] reg: Register to be written, see @c nrf_register.
 * @param[in] bit_pos: Position of the bit to be written.
 * @param[in] value: Value (1 or 0) to write into the bit.
 */
static void NRF24_write_bit(nrf_radio *radio, const nrf_register reg,
		const uint8_t bit_pos, const nrf_bit value)
{
	NRF24_ASSERT(8 > bit_pos);

    uint8_t temp = 0;
    NRF24_read_reg(radio, reg, &temp, 1);
    
    const uint8_t bit_mask = 1 << bit_pos;
    
    // Calculate the new value to be written into the register
    temp = value ? temp | bit_mask : temp & ~bit_mask;

    NRF24_write_reg(radio, reg, &temp, 1);
}
