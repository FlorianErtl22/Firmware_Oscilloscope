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
#include "adc_cntrl.h"

typedef struct
{
	uint8_t command;
	uint8_t command_id;
	union
	{
		uint8_t  bytes[4];
		uint32_t as_uint32;
		float 	 as_float;
	} param;
} t_protocol;

typedef union {
    uint32_t as_uint32;
    float    as_float;
} payload32_t;

extern t_adc_params adc_params;

void assign_command_message(uint8_t *buffer, t_protocol *prot);

void process_command(t_protocol *prot);

#endif /* PROTOCOL_H_ */
