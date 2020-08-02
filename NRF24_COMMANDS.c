/**
* @file     NRF24_COMMANDS.c
* @version  0.1
*
* @brief    The nRF24 radio is controlled via commands, all of them are accesible
* from this interface.
*
* If you want more control over the radio (no public user API aka NRF24) between
* you and your NRF24 radio you can use this set of functions.
*/

#include "NRF24_COMMANDS.h"
#include "NRF24_INTERFACE.h"

/**
 * @brief Send the @c cmd to the NRF24.
 *
 * @param[in]	cmd: Command to be sent, see @ref nrf_cmd.
 */
static uint8_t NRF24_send_cmd(nrf_radio *radio, const nrf_cmd cmd);

uint8_t NRF24_cmd_reuse_tx_payload(nrf_radio *radio)
{
    return NRF24_send_cmd(radio, NRF_CMD_REUSE_TX_PL);
}

uint8_t NRF24_cmd_flush_rx(nrf_radio *radio)
{
    return NRF24_send_cmd(radio, NRF_CMD_FLUSH_RX);
}

uint8_t NRF24_cmd_flush_tx(nrf_radio *radio)
{
    return NRF24_send_cmd(radio, NRF_CMD_FLUSH_TX);
}

uint8_t NRF24_cmd_read_rx_payload(nrf_radio *radio, uint8_t *payload, const size_t payload_size)
{
    uint8_t nrf_data_out[payload_size + 1];

    // the nrf_data_in array is to keep the spi sending dummy bytes so the
    // radio can send us the payload, there's no need to set the array to
    // any specific value, apart of the first element being the command
    uint8_t nrf_data_in[payload_size + 1];
    
    nrf_data_in[0] = NRF_CMD_R_RX_PAYLOAD;
    
    NRF24_spi_xfer_cb(radio, nrf_data_in, nrf_data_out, sizeof nrf_data_out);
    
    for (size_t idx = 0; idx < payload_size; idx++) {
    	payload[idx] = nrf_data_out[idx + 1];
    }

    return nrf_data_out[0];
}

uint8_t NRF24_cmd_write_tx_payload(nrf_radio *radio, const uint8_t *payload, const size_t payload_size)
{
    uint8_t nrf_data_in[payload_size + 1];
    uint8_t nrf_data_out[payload_size + 1];
    
    nrf_data_in[0] = NRF_CMD_W_TX_PAYLOAD;

    for (size_t idx = 0; idx < payload_size; idx++) {
    	nrf_data_in[idx + 1] = payload[idx];
    }
    
    NRF24_spi_xfer_cb(radio, nrf_data_in, nrf_data_out, sizeof nrf_data_in);

    return nrf_data_out[0];
}

uint8_t NRF24_cmd_read_payload_width(nrf_radio *radio, uint8_t *payload_width)
{
    uint8_t nrf_data_in[2] = {
        NRF_CMD_R_RX_PL_WID, NRF_CMD_NOP
    };

    uint8_t nrf_data_out[2] = {0};
    
    NRF24_spi_xfer_cb(radio, nrf_data_in, nrf_data_out, sizeof nrf_data_in);
    *payload_width = nrf_data_out[1];

    return nrf_data_out[0];
}

uint8_t NRF24_cmd_payload_write_ack(nrf_radio *radio, const nrf_pipe pipe,
		const uint8_t* payload, const size_t payload_size)
{
    uint8_t nrf_data_in[payload_size + 1];
    uint8_t nrf_data_out[payload_size + 1];
    
    nrf_data_in[0] = NRF_CMD_W_ACK_PAYLOAD | pipe;

    for (size_t idx = 0; idx < payload_size; idx++) {
    	nrf_data_in[idx + 1] = payload[idx];
    }
    
    NRF24_spi_xfer_cb(radio, nrf_data_in, nrf_data_out, sizeof nrf_data_in);

    return nrf_data_out[0];
}

uint8_t NRF24_cmd_payload_without_ack(nrf_radio *radio, const uint8_t* payload, const size_t payload_size)
{
    uint8_t nrf_data_in[payload_size + 1];
    uint8_t nrf_data_out[payload_size + 1];
    
    nrf_data_in[0] = NRF_CMD_W_TX_PAYLOAD_NO_ACK;

    for (size_t idx = 0; idx < payload_size; idx++) {
    	nrf_data_in[idx + 1] = payload[idx];
    }
    
    NRF24_spi_xfer_cb(radio, nrf_data_in, nrf_data_out, sizeof nrf_data_in);

    return nrf_data_out[0];
}

uint8_t NRF24_cmd_nop(nrf_radio *radio)
{
    return NRF24_send_cmd(radio, NRF_CMD_NOP);
}

static uint8_t NRF24_send_cmd(nrf_radio *radio, const nrf_cmd cmd)
{
    uint8_t status = 0;
    NRF24_spi_xfer_cb(radio, &cmd, &status, 1);
    return status;
}
