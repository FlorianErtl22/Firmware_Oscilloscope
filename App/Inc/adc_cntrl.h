/*
 * adc_cntrl.h
 *
 *  Created on: Dec 11, 2025
 *      Author: Florian
 */

#ifndef ADC_CNTRL_H_
#define ADC_CNTRL_H_

#include <stdint.h>

typedef struct {
	uint32_t clk_prescaler;
	uint32_t resolution;
	uint32_t number_of_samples;
} t_adc_params;

void init_adc()
{
	
    // Calibrate ADC
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) tx_buf,sizeof(tx_buf) / sizeof(tx_buf[0]));
	HAL_TIM_Base_Start(&htim3);
}


#endif /* INC_ADC_CNTRL_H_ */
