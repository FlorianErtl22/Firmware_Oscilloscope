#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "usart.h"

#define RX_BUFFER_SIZE 6
#define TX_BUFFER_SIZE 1024

extern uint8_t  rx_buf[RX_BUFFER_SIZE];
extern uint16_t tx_buf[TX_BUFFER_SIZE];

#endif /* COMMUNICATION_H_ */