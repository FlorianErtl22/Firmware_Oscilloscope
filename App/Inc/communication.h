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

typedef struct
{
    uint32_t trigger_index;
    uint8_t trigger_detected;
} t_com_params;

extern t_com_params com_params;

#endif /* COMMUNICATION_H_ */