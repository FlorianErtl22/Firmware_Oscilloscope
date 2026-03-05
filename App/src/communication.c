#include "communication.h"

uint8_t  rx_buf[RX_BUFFER_SIZE] __attribute__((section(".RAM_D2")));
uint16_t tx_buf[TX_BUFFER_SIZE] __attribute__((section(".RAM_D2"))) __attribute__((aligned(32)));

t_com_params com_params;