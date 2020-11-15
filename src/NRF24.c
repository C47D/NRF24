/**
* @file     NRF24.c
* @version  0.1
* @brief    Public interface.
*/

#include "NRF24.h"
#include "NRF24_HAL.h"
#include "NRF24_DEFS.h"
#include "NRF24_COMMANDS.h"
#include "NRF24_INTERFACE.h"

/*
struct _nrf_radio {
	nrf_write_ce 	write_ce_cb;
	nrf_read_irq 	read_irq_cb;
	nrf_delay_ms 	delay_ms_cb;
	nrf_spi_xfer	spi_xfer_data_cb;
};
*/

int NRF24_init(nrf_radio *radio,
	nrf_spi_xfer spi_xfer_cb, nrf_write_ce write_ce_cb,
	nrf_read_irq read_irq_cb, nrf_delay_ms delay_ms_cb)
{
    if ((NULL == radio) ||
        (NULL == spi_xfer_cb) ||
        (NULL == write_ce_cb) ||
        (NULL == delay_ms_cb)) {

        return 1;
    }
	radio->delay_ms_cb = delay_ms_cb;
	radio->read_irq_cb = read_irq_cb;
	radio->spi_xfer_data_cb = spi_xfer_cb;
	radio->write_ce_cb = write_ce_cb;

    return 0;
}

void NRF24_sleep(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_set_standby_i_mode(radio);
}

void NRF24_wakeup(nrf_radio *radio)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(radio->delay_ms_cb);

    NRF24_set_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_BIT_PWR_UP);
    /* after leaving standby-I mode the radio need a time to return to TX or
     * RX Mode */
    NRF24_hal_delay(radio, 5);
}

void NRF24_set_mode(nrf_radio *radio, const nrf_mode mode)
{
	NRF24_ASSERT(radio);

	NRF_MODE_RX == mode ? NRF24_set_rx_mode(radio) : NRF24_set_tx_mode(radio);
}

/* TODO */
nrf_mode NRF24_get_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

	return NRF_MODE_RX;
}

void NRF24_set_power_down_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_BIT_PWR_UP);
}

void NRF24_set_standby_i_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_stop_listening(radio);
    NRF24_set_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_PWR_UP);
}

void NRF24_set_standby_ii_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_start_listening(radio);
    NRF24_set_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_BIT_PWR_UP);
}

void NRF24_set_rx_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_BIT_PRIM_RX);
}

void NRF24_set_tx_mode(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_CONFIG, NRF_CONFIG_BIT_PRIM_RX);
}

void NRF24_enable_auto_ack(nrf_radio *radio, const nrf_pipe pipe)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_EN_AA, pipe);
}

void NRF24_disable_auto_ack(nrf_radio *radio, const nrf_pipe pipe)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_EN_AA, pipe);
}

void NRF24_set_channel(nrf_radio *radio, uint8_t channel)
{
	NRF24_ASSERT(radio);

    if (NRF_MAX_RF_CHANNEL < channel) {
        channel = NRF_MAX_RF_CHANNEL;
    }

    NRF24_write_reg(radio, NRF_REG_RF_CH, &channel, 1);

    NRF24_cmd_flush_rx(radio);
    NRF24_cmd_flush_tx(radio);
}

uint8_t NRF24_get_channel(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t channel;
    NRF24_read_reg(radio, NRF_REG_RF_CH, &channel, 1);
    return channel;
}

void NRF24_set_address_width(nrf_radio *radio, const nrf_addr_width addr_width)
{
	NRF24_ASSERT(radio);

    NRF24_write_reg(radio, NRF_REG_SETUP_AW, (uint8_t *) &addr_width, 1);
}

uint8_t NRF24_get_address_width(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

	uint8_t retval = 0;
	uint8_t addr_width = 0;
	NRF24_read_reg(radio, NRF_REG_SETUP_AW, (uint8_t *) &addr_width, 1);

	switch (addr_width) {
	case 1:
		retval = 3;
		break;
	case 2:
		retval = 4;
		break;
	case 3:
		retval = 5;
		break;
	default:
		NRF24_ASSERT(0);
		break;
	}

    return retval;
}

void NRF24_set_rx_pipe_address(nrf_radio *radio, const nrf_addr_rx_pipe pipe,
		const uint8_t *addr, size_t size)
{
    NRF24_ASSERT(radio);
    NRF24_ASSERT(addr);
    
    NRF24_ASSERT(NRF_PIPE_ADDR_WIDTH_5BYTES >= size);

    switch(pipe) {
    // For pipes 0 and 1 we can change up to 5 bytes of it's addresses
    case NRF_ADDR_PIPE0:
    case NRF_ADDR_PIPE1:
    	// The smaller address is 3 bytes
    	NRF24_ASSERT(NRF_PIPE_ADDR_WIDTH_3BYTES < size);

        NRF24_write_reg(radio, pipe, addr, size);
        break;
    // For pipes 2, 3, 4 and 5 we can change only the LSB of it's address
    case NRF_ADDR_PIPE2:
    case NRF_ADDR_PIPE3:
    case NRF_ADDR_PIPE4:
    case NRF_ADDR_PIPE5:
        (void) size; // don't get unused variable warning
        NRF24_write_reg(radio, pipe, addr, 1);
        break;
    default:
    	NRF24_ASSERT(0);
    	break;
    }
}

void NRF24_get_rx_pipe_address(nrf_radio *radio, const nrf_addr_rx_pipe pipe,
		uint8_t *addr, size_t size)
{
	NRF24_ASSERT(radio);
    NRF24_ASSERT(addr);
    NRF24_ASSERT(NRF_PIPE_ADDR_WIDTH_3BYTES <= size);

    /* How many bytes is the radio address configured to? */
    uint8_t conf_addr_width = NRF24_get_address_width(radio);
    
    if (conf_addr_width < size) {
        size = conf_addr_width;
    }
    
    switch(pipe) {
    /* Pipes 0 and 1 we can read up to 5 bytes of it's addresses */
    case NRF_ADDR_PIPE0:
    case NRF_ADDR_PIPE1:
        NRF24_read_reg(radio, pipe, addr, size);
        break;
    /* Pipes 2, 3, 4 and 5 the address is the same to pipe1 except the LSB */
    case NRF_ADDR_PIPE2:
    case NRF_ADDR_PIPE3:
    case NRF_ADDR_PIPE4:
    case NRF_ADDR_PIPE5:
        NRF24_read_reg(radio, NRF_REG_RX_ADDR_P1, addr, size - 1);
        NRF24_read_reg(radio, pipe, &addr[size - 1], 1);
        break;
    default:
    	NRF24_ASSERT(0);
    	break;
    }
}

void NRF24_rx_pipe_enable(nrf_radio *radio, nrf_pipe pipe, nrf_rx_pipe_enabled enable)
{
	NRF24_ASSERT(radio);

	switch (pipe) {
	case NRF_PIPE0:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P0);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P0);
		}
		break;
	case NRF_PIPE1:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P1);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P1);
		}
		break;
	case NRF_PIPE2:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P2);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P2);
		}
		break;
	case NRF_PIPE3:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P3);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P3);
		}
		break;
	case NRF_PIPE4:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P4);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P4);
		}
		break;
	case NRF_PIPE5:
		if (NRF_RX_PIPE_ENABLED == enable) {
			NRF24_set_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P5);
		} else {
			NRF24_clear_bit(radio, NRF_REG_EN_RXADDR, NRF_EN_RXADDR_BIT_ERX_P5);
		}
		break;
	default:
		NRF24_ASSERT(0);
		break;
	}
}

nrf_rx_pipe_enabled NRF24_rx_pipe_is_enabled(nrf_radio *radio, nrf_pipe pipe)
{
	NRF24_ASSERT(radio);

	return (nrf_rx_pipe_enabled) NRF24_read_bit(radio, NRF_REG_EN_RXADDR, pipe);
}

void NRF24_set_tx_address(nrf_radio *radio, const uint8_t *const addr, size_t size)
{
    NRF24_ASSERT(radio);
    NRF24_ASSERT(addr);

    uint8_t conf_addr_width = NRF24_get_address_width(radio);
    NRF24_ASSERT(conf_addr_width == size);

    NRF24_write_reg(radio, NRF_REG_TX_ADDR, addr, size);
}

void NRF24_get_tx_address(nrf_radio *radio, uint8_t* addr, size_t size)
{
	NRF24_ASSERT(radio);
    NRF24_ASSERT(addr);

    /* How many bytes is the radio address configured to? */
    uint8_t conf_addr_width = NRF24_get_address_width(radio);

    if (conf_addr_width < size) {
        size = conf_addr_width;
    }

    NRF24_read_reg(radio, NRF_REG_TX_ADDR, addr, size);
}

void NRF24_set_payload_size(nrf_radio *radio, const nrf_pld_size pipe, uint8_t size)
{
	NRF24_ASSERT(radio);

    if (NRF_PAYLOAD_SIZE_MAX < size) {
        size = NRF_PAYLOAD_SIZE_MAX;
    }

    NRF24_write_reg(radio, pipe, &size, 1);
}

uint8_t NRF24_get_payload_size(nrf_radio *radio, const nrf_pld_size pipe)
{
	NRF24_ASSERT(radio);

    uint8_t payload_size;
    NRF24_read_reg(radio, pipe, &payload_size, 1);
    return payload_size;
}

void NRF24_reuse_last_transmitted_payload(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_cmd_reuse_tx_payload(radio);
    NRF24_transmit_pulse(radio);
}

void NRF24_enable_dynamic_payload(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_write_bits(radio, NRF_REG_FEATURE,
    	(1 << NRF_FEATURE_BIT_EN_ACK_PAY) | (1 << NRF_FEATURE_BIT_EN_DPL),
    	NRF_ENABLE_DYN_PAYLOAD_LEN | NRF_ENABLE_PAYLOAD_WITH_ACK);
}

void NRF24_enable_dynamic_payload_on_pipe(nrf_radio *radio, const nrf_pipe pipe)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_EN_AA, pipe);
    NRF24_set_bit(radio, NRF_REG_DYNPD, pipe);
}

void NRF24_disable_dynamic_payload(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_write_bits(radio, NRF_REG_FEATURE,
    	(1 << NRF_FEATURE_BIT_EN_ACK_PAY) | (1 << NRF_FEATURE_BIT_EN_DPL),
    	NRF_DISABLE_DYN_PAYLOAD_LEN | NRF_DISABLE_PAYLOAD_WITH_ACK);
}

void NRF24_disable_dynamic_payload_on_pipe(nrf_radio *radio, const nrf_pipe pipe)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_EN_AA, pipe);
    NRF24_clear_bit(radio, NRF_REG_DYNPD, pipe);
}

void NRF24_enable_dynamic_payload_length(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_DPL);
}

void NRF24_disable_dynamic_payload_length(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_DPL);
}

void NRF24_enable_payload_with_ack(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_ACK_PAY);
}

void NRF24_disable_payload_with_ack(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_ACK_PAY);
}

void NRF24_enable_payload_with_no_ack(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_set_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_DYN_ACK);
}

void NRF24_disable_payload_with_no_ack(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_clear_bit(radio, NRF_REG_FEATURE, NRF_FEATURE_BIT_EN_DYN_ACK);
}

void NRF24_start_listening(nrf_radio *radio)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(radio->write_ce_cb);

	NRF24_hal_set_ce(radio, GPIO_SET);
}

void NRF24_stop_listening(nrf_radio *radio)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(radio->write_ce_cb);

	NRF24_hal_set_ce(radio, GPIO_CLEAR);
}

void NRF24_transmit_pulse(nrf_radio *radio)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(radio->write_ce_cb);
	NRF24_ASSERT(radio->delay_ms_cb);

	NRF24_hal_set_ce(radio, GPIO_SET);
	NRF24_hal_delay(radio, NRF_CE_PULSE_WIDTH_US);
	NRF24_hal_set_ce(radio, GPIO_CLEAR);
}

uint8_t NRF24_get_status(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return NRF24_cmd_nop(radio);
}

uint8_t NRF24_get_fifo_status(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t fifo_status;
    NRF24_read_reg(radio, NRF_REG_FIFO_STATUS, &fifo_status, 1);

    return fifo_status;
}

uint8_t NRF24_get_retransmissions_count(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t count;
    NRF24_read_reg(radio, NRF_REG_OBSERVE_TX, &count, 1);

    return count & NRF_OBSERVE_TX_ARC_CNT_MASK;
}

uint8_t NRF24_get_lost_packets_count(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t lostPackets;
    NRF24_read_reg(radio, NRF_REG_OBSERVE_TX, &lostPackets, 1);
    lostPackets = lostPackets & NRF_OBSERVE_TX_PLOS_CNT_MASK;

    return lostPackets >> NRF_OBSERVE_TX_BIT_PLOS_CNT;
}

void NRF24_put_in_tx_fifo(nrf_radio *radio, const uint8_t* payload, size_t payload_size)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(payload);
	NRF24_ASSERT(NRF_PAYLOAD_SIZE_MAX >= payload_size);

    NRF24_cmd_write_tx_payload(radio, payload, payload_size);
}

void NRF24_transmit(nrf_radio *radio, const uint8_t* payload, size_t payload_size)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(payload);
	NRF24_ASSERT(NRF_PAYLOAD_SIZE_MAX >= payload_size);

    NRF24_put_in_tx_fifo(radio, payload, payload_size);
    NRF24_transmit_pulse(radio);
}

uint8_t NRF24_is_data_ready(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return (uint8_t) NRF_STATUS_RX_DR_MASK & NRF24_get_status(radio);
}

void NRF24_get_rx_payload(nrf_radio *radio, uint8_t *payload, const size_t payload_size)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(radio->write_ce_cb);
	NRF24_ASSERT(payload);

	NRF24_hal_set_ce(radio, GPIO_CLEAR);
    NRF24_cmd_read_rx_payload(radio, payload, payload_size);
    NRF24_hal_set_ce(radio, GPIO_SET);
}

void NRF24_tx_transmit_no_ack(nrf_radio *radio, const uint8_t *payload, size_t payload_size)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(payload);
	NRF24_ASSERT(NRF_PAYLOAD_SIZE_MAX > payload_size);

    NRF24_cmd_payload_without_ack(radio, payload, payload_size);
    NRF24_transmit_pulse(radio);
}

void NRF24_rx_write_payload(nrf_radio *radio, const nrf_pipe pipe,
		const uint8_t *payload, size_t payload_size)
{
	NRF24_ASSERT(radio);
	NRF24_ASSERT(payload);

    if (NRF_PAYLOAD_SIZE_MAX < payload_size) {
        payload_size = NRF_PAYLOAD_SIZE_MAX;
    }

    NRF24_cmd_payload_write_ack(radio, pipe, payload, payload_size);
}

uint8_t NRF24_get_data_pipe_with_payload(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t pipe;
    NRF24_read_reg(radio, NRF_REG_STATUS, &pipe, 1);
    return (pipe & NRF_STATUS_PIPES_MASK) >> NRF_STATUS_PIPES_SHIFT;
}

uint8_t NRF24_received_power_detector(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return NRF24_read_bit(radio, NRF_REG_RPD, NRF_RPD_BIT_RPD);
}

uint8_t NRF24_is_tx_fifo_full(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return NRF24_read_bit(radio, NRF_REG_FIFO_STATUS, NRF_FIFO_STATUS_BIT_TX_FULL);
}

uint8_t NRF24_is_rx_fifo_empty(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return NRF24_read_bit(radio, NRF_REG_FIFO_STATUS, NRF_FIFO_STATUS_BIT_RX_EMPTY);
}

uint8_t NRF24_test_carrier(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    return NRF24_read_bit(radio, NRF_REG_RPD, NRF_RPD_BIT_RPD);
}

void NRF24_clear_all_irqs(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t mask = NRF_ALL_IRQ_MASK;
    NRF24_write_reg(radio, NRF_REG_STATUS, &mask, 1);
}

void NRF24_clear_irq_flag(nrf_radio *radio, const nrf_irq irq_flag)
{
	NRF24_ASSERT(radio);

    uint8_t status = NRF24_get_status(radio);
    status |= (uint8_t) irq_flag;
    
    NRF24_write_reg(radio, NRF_REG_STATUS, &status, 1);
}

nrf_irq NRF24_get_irq_flag(nrf_radio *radio)
{
    NRF24_ASSERT(radio);

    return (nrf_irq) NRF_ALL_IRQ_MASK & NRF24_cmd_nop(radio);
}

nrf_irq NRF24_poll_interrupt(nrf_radio *radio)
{
	nrf_irq irq = NRF_NONE_IRQ;
	NRF24_ASSERT(radio);

	/* TODO */

	return irq;
}

uint8_t NRF24_get_status_clear_irq(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    uint8_t mask = NRF_ALL_IRQ_MASK;
    return NRF24_write_reg(radio, NRF_REG_STATUS, &mask, 1);
}

// command wrappers
void NRF24_flush_rx(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_cmd_flush_rx(radio);
}

void NRF24_flush_tx(nrf_radio *radio)
{
	NRF24_ASSERT(radio);

    NRF24_cmd_flush_tx(radio);
}

