#include "app_main.h"

#include "stm32h7xx_hal.h"
#include "adc.h"
#include "tim.h"

#include <stdbool.h>

#include "protocol.h"
#include "adc_cntrl.h"
#include "communication.h"

void App_Init(void)
{
	init_adc(&hadc1, &htim3, tx_buf, sizeof(tx_buf)/sizeof(tx_buf[0]));
}

void App_Loop(void)
{
    bool command_ready = false;
	t_protocol prot;

	while (1) {

        while (!command_ready) {
			if (HAL_UART_Receive(&huart3, &rx_buf[0], 1, HAL_MAX_DELAY) == HAL_OK) {
				if ((rx_buf[0] == 0x01) || (rx_buf[0] == 0x02) || (rx_buf[0] == 0x03)) {
					if (HAL_UART_Receive(&huart3, &rx_buf[1], 5, 100) == HAL_OK) {
						command_ready = true;
					}
				}
			}
		}

		assign_command_message(rx_buf, &prot);
		process_command(&prot);
		command_ready = false;
	}
}
