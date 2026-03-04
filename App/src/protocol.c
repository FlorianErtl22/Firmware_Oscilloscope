#include <string.h> // for memcpy

#include "adc.h"
#include "tim.h"

#include "protocol.h"
#include "communication.h"
#include "adc_cntrl.h"

#define MAX_VALID_ID (sizeof(parameter_map) / sizeof(parameter_map[0]) - 1)

void assign_command_message(uint8_t *buffer, t_protocol *prot)
{
	prot->command = buffer[0];
	prot->command_id = buffer[1];
	prot->param.bytes[0] = buffer[2];
	prot->param.bytes[1] = buffer[3];
	prot->param.bytes[2] = buffer[4];
	prot->param.bytes[3] = buffer[5];
}

void tx_data(void)
{
	// Stop the Timer first to prevent new ADC triggers
	HAL_TIM_Base_Stop(&htim3);

	// Read the DMA counter BEFORE stopping the DMA, as stopping it might clear the register
	uint32_t ndtr = __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);

	HAL_ADC_Stop_DMA(&hadc1);

	// Determine where the DMA was about to write next
	uint32_t write_index = TX_BUFFER_SIZE - ndtr;

	// Safety catch: If NDTR was exactly 0 or glitchy, wrap to index 0
	if (write_index >= TX_BUFFER_SIZE)
	{
		write_index = 0;
	}

	uint32_t elements_part1 = TX_BUFFER_SIZE - write_index;
	uint32_t elements_part2 = write_index;

	if (elements_part1 > 0)
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)&tx_buf[write_index], (elements_part1 * sizeof(uint16_t)), 10000);
	}

	if (elements_part2 > 0)
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)&tx_buf[0], (elements_part2 * sizeof(uint16_t)), 10000);
	}

	// RESTART THE HARDWARE
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)tx_buf, TX_BUFFER_SIZE);
	HAL_TIM_Base_Start(&htim3);
}

static void *const parameter_map[] = {
	[0x01] = &adc_params.amplitude,
	[0x02] = &adc_params.resolution,
	[0x03] = &adc_params.number_of_samples,
	[0x04] = &adc_params.adc_prescaler,
	[0x05] = &adc_params.timer_prescaler,
	[0x06] = &adc_params.timer_arr,
};

void process_command(t_protocol *prot)
{
	switch (prot->command)
	{
	// set
	case 0x01:
		// check if id is valid to not write to the wrong memory address
		if (prot->command_id <= MAX_VALID_ID && parameter_map[prot->command_id] != NULL)
		{
			memcpy(parameter_map[prot->command_id], &(prot->param), 4);
		}
		break;

	// run
	case 0x02:
		// Call ADC_DMA_Start
		// Uart_transmit
		tx_data();
		break;

	// run config
	case 0x03:
		update_adc(&adc_params, &hadc1, &htim3, &sConfig_adc1);
		init_adc(&hadc1, &htim3, tx_buf, sizeof(tx_buf) / sizeof(tx_buf[0]));
		printf("Hello\n");
		break;

	default:
		break;
	}
}
