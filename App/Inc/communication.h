#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "usart.h"

uint8_t rx_buf[6] = { };
uint16_t tx_buf[1024] = { };

#endif /* COMMUNICATION_H_ */