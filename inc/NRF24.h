/**
* @file     NRF24.h
* @version  0.1
* @brief  	Public NRF24 interface.
*/

#ifndef NRF24_FUNCS_H
#define NRF24_FUNCS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24_DEFS.h"

#ifndef NRF24_DISABLE_ASSERTS
	/* Define a custom NRF24_ASSERT by defining it before including this file */
	#ifndef NRF24_ASSERT
		#define NRF24_ASSERT(x)	if(!(x)) { while(1); }
	#endif
#else
	/* Asserts disabled, do nothing */
	#define NRF24_ASSERT(x)	(x)
#endif

/* Get elements in array */
#define NRF_ARRAY_SIZE(array)	((sizeof array)/(sizeof *array))

/* Component version info */
#define NRF24_VERSION_MAJOR  0
#define NRF24_VERSION_MINOR  1

/* Communication with the radio via SPI */
typedef void (*nrf_spi_xfer)(const uint8_t *in, uint8_t *out, const size_t xfer_size);

/* Set the CE signal logic level */
typedef void (*nrf_write_ce)(nrf_gpio state);

/* Get the IRQ signal logic level */
typedef nrf_gpio (*nrf_read_irq)(void);

/* Delay ms */
typedef void (*nrf_delay_ms)(uint32_t ms);

/* Collection of user callbacks */
typedef struct _nrf_radio nrf_radio;

struct _nrf_radio {
	nrf_write_ce 	write_ce_cb;
	nrf_read_irq 	read_irq_cb;
	nrf_delay_ms 	delay_ms_cb;
	nrf_spi_xfer	spi_xfer_data_cb;
};

/**
 * @brief Initialize the radio object.
 *
 * @param[in] 	radio:
 * @param[in] 	spi_xfer_cb: . Required.
 * @param[in] 	write_ce_cb: . Required.
 * @param[in]	read_irq_cb: . Required only when using IRQ signal.
 * @param[in]	delay_ms_cb: . Required.
 */
int NRF24_init(nrf_radio *radio,
	nrf_spi_xfer spi_xfer_cb, nrf_write_ce write_ce_cb,
	nrf_read_irq read_irq_cb, nrf_delay_ms delay_ms_cb);

/**
 * @brief Sleep the radio.
 *
 * Put the NRF24 radio on Standby-I mode.
 * @note Is standby-I the lowest power mode?
 *
 * @param radio
 */
void NRF24_sleep(nrf_radio *radio);

/**
 * @brief Wake-up the radio.
 *
 * @param radio
 */
void NRF24_wakeup(nrf_radio *radio);

/**
 * @brief Configure the radio as Receiver or Transmitter.
 *
 * @param[in]	radio:
 * @param[in]	mode: The radio can be configured as Receiver (PRX)
 * 				or Transmitter (PTX).
 */
void NRF24_set_mode(nrf_radio *radio, const nrf_mode mode);

/**
 * @brief Get radio mode.
 *
 * @param[in]	radio:
 * @return
 */
nrf_mode NRF24_get_mode(nrf_radio *radio);

/**
 * @brief Set radio in Power Down Mode.
 *
 * In power down mode NRF24 is disabled using minimal current consumption.
 * All register values available are maintained and the SPI is kept active,
 * enabling change of configuration and the uploading/downloading of data
 * registers.
 * Power down mode is entered by setting the PWR_UP bit (CONFIG register) to 0.
 *
 * @param radio
 */
void NRF24_set_power_down_mode(nrf_radio *radio);

/**
 * @brief Set the radio in Standby I Mode.
 *
 * By setting the PWR_UP bit in the CONFIG register to 1, the device enters
 * standby-I mode. Standby-I mode is used to minimize average current
 * consumption while maintaining short start up times.
 * In this mode only part of the crystal oscillator is active. Change to
 * active modes only happens if CE is set high and when CE is set low,
 * the NRF24 returns to standby-I mode from both the TX and RX modes.
 *
 * @param radio
 */
void NRF24_set_standby_i_mode(nrf_radio *radio);

/**
 * @brief Set the radio in Standby II Mode.
 *
 * In standby-II mode extra clock buffers are active and more current is used
 * compared to standby-I mode. NRF24 enters standby-II mode if CE is held high
 * on a TX device with an empty TX FIFO.
 * If a new packet is uploaded to the TX FIFO, the PLL immediately starts and
 * the packet is transmitted after the normal PLL settling  delay (130us).
 *
 * @param radio
 */
void NRF24_set_standby_ii_mode(nrf_radio *radio);

/**
 * @brief Set the radio in Receiver (Rx) Mode.
 *
 * RX mode is an active mode where the NRF24 radio is used as receiver. To
 * enter this mode, the NRF24 must have the PWR_UP bit, PRIM_RX bit and the CE
 * pin set high.
 *
 * @param radio
 */
void NRF24_set_rx_mode(nrf_radio *radio);

/**
 * @brief Set the radio in Transmitted (Tx) Mode.
 *
 * TX mode is an active mode for transmitting packets. To enter this mode,
 * the NRF24 must have the PWR_UP bit set high, PRIM_RX bit set low, a payload
 * in the TX FIFO and a high pulse on the CE for more than 10us.
 *
 * @param radio
 */
void NRF24_set_tx_mode(nrf_radio *radio);

/**
 * @brief Enable AutoACK on @p pipe.
 *
 * @param radio
 * @param pipe: Enable AutoACK on @p pipe.
 */
void NRF24_enable_auto_ack(nrf_radio *radio, const nrf_pipe pipe);

/**
 * @brief Disable AutoACK on @p pipe.
 *
 * @param radio
 * @param pipe: Disable AutoACK on @p pipe.
 */
void NRF24_disable_auto_ack(nrf_radio *radio, const nrf_pipe pipe);

/**
 * @brief Set the channel where the radio will work.
 *
 * @param radio
 * @param channel: Channel where the radio will work.
 */
void NRF24_set_channel(nrf_radio *radio, uint8_t channel);

/**
 *
 * @param radio
 * @return
 */
uint8_t NRF24_get_channel(nrf_radio *radio);

/**
 * @brief Set the data pipes address width.
 *
 * @param radio
 * @param addr_width: @ref nrf_addr_width.
 */
void NRF24_set_address_width(nrf_radio *radio, const nrf_addr_width addr_width);

/**
 * @brief Get the address width of the data pipes.
 *
 * 00b - Illegal.
 * 01b - 3 bytes.
 * 10b - 4 bytes.
 * 11b - 5 bytes.
 *
 * @param radio
 * @return Address width in bytes.
 */
uint8_t NRF24_get_address_width(nrf_radio *radio);

/**
 * @brief Set the TX Address of the radio.
 *
 * @param radio
 * @param addr
 * @param size
 */
void NRF24_set_tx_address(nrf_radio *radio, const uint8_t *const addr, size_t size);

/**
 * @brief Get the TX Address of the radio.
 *
 * @param radio
 * @param addr
 * @param size
 */
void NRF24_get_tx_address(nrf_radio *radio, uint8_t *addr, size_t size);

/**
 * @brief Set the payload size on @p pipe.
 *
 * @param radio
 * @param pipe
 * @param size
 */
void NRF24_set_payload_size(nrf_radio *radio, const nrf_pld_size pipe, uint8_t size);

/**
 * Get the payload size of the given pipe.
 *
 * @param radio
 * @param pipe: Pipe to be read.
 * @return Configured payload size on @p pipe.
 */
uint8_t NRF24_get_payload_size(nrf_radio *radio, const nrf_pld_size pipe);

/**
 * @brief Enable dynamic payload on the given pipe.
 *
 * @param radio
 */
void NRF24_enable_dynamic_payload(nrf_radio *radio);

/**
 * @brief Disable dynamic payload.
 *
 * @param radio
 */
void NRF24_disable_dynamic_payload(nrf_radio *radio);

/**
 * @brief Enable dynamic payload on @p pipe.
 *
 * @param radio
 * @param pipe
 */
void NRF24_enable_dynamic_payload_on_pipe(nrf_radio *radio, const nrf_pipe pipe);

/**
 * @brief Disable dynamic payload on @p pipe..
 *
 * @param radio
 * @param pipe
 */
void NRF24_disable_dynamic_payload_on_pipe(nrf_radio *radio, const nrf_pipe pipe);

/**
 * @brief Enable dinamic payload length.
 *
 * @param radio
 */
void NRF24_enable_dynamic_payload_length(nrf_radio *radio);

/**
 * @brief Disable dynamic payload length.
 *
 * @param radio
 */
void NRF24_disable_dynamic_payload_length(nrf_radio *radio);

/**
 * @brief Enable payload with ACK.
 *
 * @param radio
 */
void NRF24_enable_payload_with_ack(nrf_radio *radio);

/**
 * @brief Disable Payload with ACK.
 *
 * @param radio
 */
void NRF24_disable_payload_with_ack(nrf_radio *radio);

/**
 * @brief Enable dynamic payload length.
 *
 * @param radio
 */
void NRF24_enable_payload_with_no_ack(nrf_radio *radio);

/**
 * @brief Disable Payload with no ACK.
 */
void NRF24_disable_payload_with_no_ack(nrf_radio *radio);

/**
 * @brief The NRF24 radio will start listening.
 *
 * Set the CE pin to logic high to enable the radio from "listening".
 */
void NRF24_start_listening(nrf_radio *radio);

/**
 * @brief The NRF24 radio will stop "listening".
 *
 * Set the CE pin to logic low to disable the radio from "listening".
 */
void NRF24_stop_listening(nrf_radio *radio);

/**
 * @brief Transmit pulse on the CE pin.
 *
 * The CE pin of the NRF24 radio will have a pulse of 15us,
 * this pulse trigger a transmission of the content of the TX FIFO.
 */
void NRF24_transmit_pulse(nrf_radio *radio);

/**
 * @brief Get the STATUS register of the NRF24.
 *
 * @return uint8_t: STATUS register of the NRF24.
 */
uint8_t NRF24_get_status(nrf_radio *radio);

/**
 * @brief Get the STATUS register of the NRF24.
 *
 * @return uint8_t: STATUS register of the NRF24.
 */
uint8_t NRF24_get_fifo_status(nrf_radio *radio);

/**
 * @return Retransmissions count.
 */
uint8_t NRF24_get_retransmissions_count(nrf_radio *radio);

/**
 * @return uint8_t: Lost packets count.
 */
uint8_t NRF24_get_lost_packets_count(nrf_radio *radio);

/**
 * Put data into the TX FIFO without sending it.
 *
 * @param[in]	payload: Data to be placed in the TX FIFO.
 * @param[in]	payload_size: Bytes of data.
 */
void NRF24_put_in_tx_fifo(nrf_radio *radio, const uint8_t *payload, size_t payload_size);

/**
 * Put data in TX FIFO and transmit it.
 *
 * @param[in]	payload: Data to be sent.
 * @param[in]	payload_size: Bytes of data.
 */
void NRF24_transmit(nrf_radio *radio, const uint8_t *payload, size_t payload_size);

/**
 * @brief
 *
 * @return Zero if data is not ready.
 */
uint8_t NRF24_is_data_ready(nrf_radio *radio);

/**
 *
 * @param radio
 * @param payload
 * @param payload_size
 */
void NRF24_get_rx_payload(nrf_radio *radio, uint8_t *payload, const size_t payload_size);

/**
 *
 * @param radio
 * @param payload
 * @param payload_size
 */
void NRF24_tx_transmit_no_ack(nrf_radio *radio, const uint8_t *payload, size_t payload_size);

/**
 *
 * @param radio
 * @param pipe
 * @param payload
 * @param payload_size
 */
void NRF24_rx_write_payload(nrf_radio *radio, const nrf_pipe pipe, const uint8_t *payload,
	size_t payload_size);

/**
 * Return the pipe number with data.
 *
 * @return uint8_t:
 */
uint8_t NRF24_get_data_pipe_with_payload(nrf_radio *radio);

/**
 * @brief
 *
 * @return
 */
uint8_t NRF24_received_power_detector(nrf_radio *radio);

/**
 * @return Non zero if the TX FIFO is full, zero if it does have available
 * locations.
 */
uint8_t NRF24_is_tx_fifo_full(nrf_radio *radio);

/**
 * @return Non zero if the RX FIFO es empty, zero otherwise.
 */
uint8_t NRF24_is_rx_fifo_empty(nrf_radio *radio);

/**
 * Received Power Detector triggers at received power levels above -64dBm that
 * are present in the RF channel you receive on. If the received power is less
 * than -64dBm, RDP = 0.
 * The RPD can be read out at any time while NRF24L01+ is in receive mode.
 */
uint8_t NRF24_test_carrier(nrf_radio *radio);

/* Pipes */

/**
 * @brief Set the address of the RX Pipe 0 in the radio.
 *
 * This function configure the address of the Rx Pipe 0 of the radio.
 *
 * @param const uint8_t* addr:
 * @param size_t size:
 */
void NRF24_set_rx_pipe_address(nrf_radio *radio, const nrf_addr_rx_pipe pipe,
	const uint8_t *addr, size_t size);

/**
 * @brief Get the address of the RX Pipe 0 in the radio.
 *
 * @param uint8_t* addr:
 * @param size_t size:
 */
void NRF24_get_rx_pipe_address(nrf_radio *radio, const nrf_addr_rx_pipe pipe,
	uint8_t *addr, size_t size);

/**
 * @brief
 *
 * @param radio
 * @param pipe
 * @param enable
 */
void NRF24_rx_pipe_enable(nrf_radio *radio, nrf_pipe pipe, nrf_rx_pipe_enabled enable);

/**
 * @brief
 *
 * @param radio
 * @param pipe
 * @return
 */
nrf_rx_pipe_enabled NRF24_rx_pipe_is_enabled(nrf_radio *radio, nrf_pipe pipe);

/* IRQ Handling */

/**
 * Clears all interrupt flags on the Status register.
 */
void NRF24_clear_all_irqs(nrf_radio *radio);

/**
 * @brief Clears the specific IRQ flag.
 *
 * Clear the flag by writing 1 to the interrupt flag bit.
 *
 * @param nrf_irq irq_flag: Interrupt flag to clear.
 */
void NRF24_clear_irq_flag(nrf_radio *radio, const nrf_irq irq_flag);

/**
 * Get the interrupt flag bits on the Status register.
 *
 * @return nrf_irq: interrupt flags.
 */
nrf_irq NRF24_get_irq_flag(nrf_radio *radio);

/**
 * Wait for any interrupt flag.
 *
 * @param radio
 * @return received interrupt flag
 */
nrf_irq NRF24_poll_interrupt(nrf_radio *radio);

/**
 * Clear all the interrupt flags and get the STATUS register value.
 *
 * @param radio
 * @return
 */
uint8_t NRF24_get_status_clear_irq(nrf_radio *radio);

/* Command wrappers */
/**
 *
 * @param radio
 */
void NRF24_flush_rx(nrf_radio *radio);

/**
 *
 * @param radio
 */
void NRF24_flush_tx(nrf_radio *radio);

/**
 * @brief Reuse last transmitted payload.
 *
 * This function issue the command ReuseTxPayload and then toggle the CE pin
 * to transmit the last transmitted payload.
 */
void NRF24_reuse_last_transmitted_payload(nrf_radio *radio);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NRF24_FUNCS_H */
