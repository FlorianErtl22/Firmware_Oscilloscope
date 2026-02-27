/*
 * adc_cntrl.c
 *
 *  Created on: Dec 11, 2025
 *      Author: Florian
 */
#include "adc_cntrl.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_tim.h"
#include <stdint.h>

void init_adc(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint8_t *tx_buf, uint32_t len_tx_buf)
{

    if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_ADC_Start_DMA(hadc, tx_buf, len_tx_buf);
    HAL_TIM_Base_Start(htim);
}

void update_adc(t_adc_params *params, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{

    hadc->Init.ClockPrescaler = params->adc_prescaler;
    hadc->Init.Resolution = params->resolution;

    if (HAL_ADC_Init(hadc) != HAL_OK)
    {
        Error_Handler();
    }

    htim->Init.Period = params->timer_arr;
    htim->Init.ClockDivision = params->timer_prescaler;

    if (HAL_TIM_Base_Init(htim) != HAL_OK)
    {
        Error_Handler();
    }
}