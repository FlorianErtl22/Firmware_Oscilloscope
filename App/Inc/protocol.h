/*
 * terminal_cntrl.h
 *
 *  Created on: 25.02.2026
 *      Author: Florian
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

typedef struct {
	uint8_t command;
	uint8_t command_id;
	uint32_t param;
} t_protocol;

void assign_command_message(uint8_t * buffer, t_protocol *  prot)
{
	prot->command    = buffer[0];
	prot->command_id = buffer[1];
	prot->param 	 = (buffer[2] << 24) | (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];
}

void process_command(t_protocol * prot, void (*run)(void))
{
	switch (prot->command) {
		// set
		case 0x01:
			// fire set function with id and param
			break;

		// run
		case 0x02:
			// Call ADC_DMA_Start
			// Uart_transmit
			run();
			break;
		// get
		case 0x03:

			break;
		default:
			break;
	}
}
#endif /* PROTOCOL_H_ */
