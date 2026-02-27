/*
 * terminal_cntrl.h
 *
 *  Created on: 25.02.2026
 *      Author: Florian
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>
#include "protocol.h"

typedef struct
{
	uint8_t command;
	uint8_t command_id;
	uint32_t param;
} t_protocol;

void assign_command_message(uint8_t *buffer, t_protocol *prot);

void process_command(t_protocol *prot, void (*run)(void));

#endif /* PROTOCOL_H_ */
