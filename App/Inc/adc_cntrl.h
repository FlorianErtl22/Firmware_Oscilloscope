/*
 * adc_cntrl.h
 *
 *  Created on: Dec 11, 2025
 *      Author: Florian
 */

#ifndef ADC_CNTRL_H_
#define ADC_CNTRL_H_

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_adc.h"
#include <stdint.h>


typedef struct {
    float    amplitude;
	uint32_t resolution;
	uint32_t number_of_samples;
	
    uint32_t adc_prescaler;
    uint32_t timer_prescaler;
    uint32_t timer_arr;
} t_adc_params;

void init_adc(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint16_t *dma_buf, uint32_t len_tx_buf);

void update_adc(t_adc_params * params, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim);


#endif /* INC_ADC_CNTRL_H_ */
