/**
* @file     NRF24_INTERFACE.h
* @version  0.1
*
* If you want more control over the radio (no public user API aka NRF24) between
* you and your NRF24 radio you can use this set of functions.
*/

#ifndef NRF24_INTERFACE_H
#define NRF24_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "NRF24.h"
#include "NRF24_HAL.h"
#include "NRF24_DEFS.h"

/**
 * Read the specified NRF24 register (bigger than 1 byte).
 *
 * @param[in]   reg: Register to be read, see @nrf_register.
 * @param[out]  data: Where the content of the register will be stored.
 * @param[int]  data_size: Size (in bytes) of the register and data.
 *
 * @return NRF24 status register.
 */
uint8_t NRF24_read_reg(nrf_radio *radio, const nrf_register reg, uint8_t *data, const size_t data_size);

/**
 * Read the specified NRF24 register (bigger than 1 byte).
 *
 * @param[in]   reg: Register to be read, see @nrf_register.
 * @param[in]   data: Data to be written to the register.
 * @param[in] 	data_size: Size (in bytes) of the register and data.
 *
 * @return NRF24 status register.
 */
uint8_t NRF24_write_reg(nrf_radio *radio, const nrf_register reg, const uint8_t *data, const size_t data_size);

/**
 * Read the specified @p bit_pos from @p reg.
 *
 * @param[in] reg: Register to be read, see @ref nrf_register.
 * @param[in] bit_pos: Bit position to be read.
 *
 * @return Bit value.
 */
uint8_t NRF24_read_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos);

/**
 * Set to 0 the specified bit of the specified NRF24 register.
 *
 * @param[in]   reg: Register to be written, see @c nrf_register.
 * @param[in]   bit_pos: Position of the bit to be written.
 */
void NRF24_clear_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos);

/**
 * Set to 1 the specified bit of the specified NRF24 register.
 *
 * @param[in]   reg: Register to be written, see @c nrf_register.
 * @param[in]   bit_pos: Position of the bit to be written.
 */
void NRF24_set_bit(nrf_radio *radio, const nrf_register reg, const uint8_t bit_pos);

/**
 * Update a group of bits on a register.
 *
 * @param[in]	radio: Radio handle.
 * @param[in]	reg: Register to be written, see @c nrf_register.
 * @param[in]	bits_mask: Mask of bits to be updated, bits to change must be set to 1.
 * @param[in]	value: New value of the bit group.
 */
void NRF24_write_bits(nrf_radio *radio, const nrf_register reg, uint8_t bits_mask, uint8_t value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NRF24_INTERFACE_H */
