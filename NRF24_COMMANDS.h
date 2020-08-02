/**
* @file     NRF24_COMMANDS.h
* @version  0.1
*
* @brief    The nRF24 radio is controlled via commands, all of them are accesible
* from this interface.
*
* If you want more control over the radio (no public user API aka NRF24) between
* you and your NRF24 radio you can use this set of functions.
*/

#ifndef NRF24_COMMANDS_H
#define NRF24_COMMANDS_H
    
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "NRF24.h"
#include "NRF24_DEFS.h"

/**
 * @brief Reuse the last transmitted payload.
 *
 * Reuse last transmitted payload. TX payload reuse is active until
 * NRF_CMD_W_TX_PAYLOAD or NRF_CMD_FLUSH_TX commands are executed.
 *
 * @note Used in TX mode.
 *
 * @warning TX payload reuse must not be activated or deactivated during
 * package transmission.
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_reuse_tx_payload(nrf_radio *radio);

/**
 * @brief
 * Read RX payload: 1 - 32 bytes. A read operation always starts at byte 0.
 * Payload is deleted from FIFO after it is read.
 *
 * @note Used in RX mode.
 *
 * @param[in]	payload: payload to be read.
 * @param[in]	payload_size: Bytes of payload to be read (max 32).
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_read_rx_payload(nrf_radio *radio, uint8_t *payload, const size_t payload_size);

/**
 * @brief Write TX payload: 1 - 32 bytes.
 *
 * A write operation always starts at byte 0 used in TX payload.
 *
 * @param[in]	data: Data to be sent.
 * @param[in]	size: Bytes of data to be sent (max 32).
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_write_tx_payload(nrf_radio *radio, const uint8_t *payload, const size_t payload_size);

/**
 * @brief Flush the RX FIFO.
 *
 * @note Used in RX mode.
 *
 * @warning Should be not executed during transmission of acknowledge,
 * that is, acknowledge package will not be completed.
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_flush_rx(nrf_radio *radio);

/**
 * @brief Flush the TX FIFO.
 *
 * @note Used in TX mode.
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_flush_tx(nrf_radio *radio);

/**
 * @brief Read RX payload width for the top R_RX_PAYLOAD in the RX FIFO.
 *
 * @note Flush RX FIFO if payload_width is larger than 32 bytes.
 *
 * @param[in]	radio:
 * @param[out]	payload_width: width of the payload on the top of the RX FIFO.
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_read_payload_width(nrf_radio *radio, uint8_t *payload_width);

/**
 * @brief Write Payload to be transmitted together with ACK packet.
 *
 * Write Payload to be transmitted together with ACK packet on PIPE PPP
 * (PPP valid in the range from 000 to 101). Maximum three ACK packet
 * payloads can be pending. Payloads with same PPP are handled using
 * first in - first out principle.
 * Write payload: 1 - 32 bytes.
 * A write operation always starts at byte 0.
 *
 * @note Used in RX mode.
 *
 * @param[in]	pipe: Pipe to use, see @ref nrf_pipe.
 * @param[in]	data: Data to be sent.
 * @param[in]	size: Bytes of data to be sent (max 32)
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_payload_write_ack(nrf_radio *radio, const nrf_pipe pipe, const uint8_t* payload, const size_t payload_size);

/**
 * @brief Disable AUTOACK on this packet.
 *
 * @note Used in TX mode.
 *
 * @param[in]	data: Data to be sent.
 * @param[in]	size: Bytes of data to be sent (max 32).
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_payload_without_ack(nrf_radio *radio, const uint8_t *payload, const size_t payload_size);

/**
 * @brief NOP (No OPeration) Command. Useful to read the STATUS register.
 *
 * @return STATUS register.
 */
uint8_t NRF24_cmd_nop(nrf_radio *radio);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NRF24_COMMANDS_H */
