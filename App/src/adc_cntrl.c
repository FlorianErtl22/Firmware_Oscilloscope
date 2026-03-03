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

/**
 * @brief  Initializes, calibrates, and starts a timer-triggered ADC using DMA.
 * * @details This function safely halts any ongoing ADC and Timer operations before
  * forcing a new hardware calibration. It then primes the DMA controller
 * with the provided buffer and restarts the peripherals in the correct 
 * sequence (ADC first, then Timer trigger).
 * * @param  hadc Pointer to the ADC handle (e.g., &hadc1).
 * @param  htim Pointer to the Timer handle used as the ADC trigger (e.g., &htim2).
 * @param  dma_buf Pointer to the memory buffer where DMA will store the ADC readings.
 * @param  len_tx_buf The number of data items to transfer via DMA.
 * * @note   The ADC is calibrated for single-ended offset. If differential 
 * calibration is needed, this function must be modified.
 * @warning If HAL_ADCEx_Calibration_Start fails, the system will halt in Error_Handler().
 */
void init_adc(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint16_t *dma_buf, uint32_t len_tx_buf)
{
    HAL_TIM_Base_Stop(htim);
    HAL_ADC_Stop_DMA(hadc);

    if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    // If the hardware rejects the configuration, halt immediately!
    if (HAL_ADC_Start_DMA(hadc, (uint32_t*)dma_buf, len_tx_buf) != HAL_OK)
    {
        Error_Handler(); 
    }

    if (HAL_TIM_Base_Start(htim) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  Updates the ADC and Timer hardware configuration at runtime.
 * * @details Safely stops the hardware peripherals, applies new resolution, prescaler, 
 * and timer period values from the provided parameter structure, and 
 * re-initializes the STM32 HAL structs.
 * * @param  params Pointer to the struct containing the new configuration values.
 * @param  hadc Pointer to the ADC handle to be updated.
 * @param  htim Pointer to the Timer handle to be updated.
 * * @note   This function contains NULL pointer safety checks and will return 
 * immediately if any pointer is invalid.
 * @warning This function leaves the ADC and Timer in a STOPPED state. You must 
 * call init_adc() (or start them manually) after this function returns 
 * so the ADC is recalibrated with the new clock/resolution settings.
 */
void update_adc(t_adc_params *params, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
    if (params == NULL || hadc == NULL || htim == NULL)
    {
        return; 
    }
    HAL_TIM_Base_Stop(htim);
    HAL_ADC_Stop_DMA(hadc);

    switch (params->resolution) {
        case 16: hadc->Init.Resolution = ADC_RESOLUTION_16B; break;
        case 14: hadc->Init.Resolution = ADC_RESOLUTION_14B; break;
        case 12: hadc->Init.Resolution = ADC_RESOLUTION_12B; break;
        case 10: hadc->Init.Resolution = ADC_RESOLUTION_10B; break;
        case 8:  hadc->Init.Resolution = ADC_RESOLUTION_8B; break;
        default: hadc->Init.Resolution = ADC_RESOLUTION_16B; break; // Safe fallback
    }

    switch (params->adc_prescaler) {
        case 1:   hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1; break;
        case 2:   hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2; break;
        case 4:   hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4; break;
        case 6:   hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV6; break;
        case 8:   hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8; break;
        case 10:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10; break;
        case 12:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV12; break;
        case 16:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16; break;
        case 32:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32; break;
        case 64:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV64; break;
        case 128: hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128; break;
        case 256: hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV256; break;
        default:  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV64; break; // Safe fallback
    }

    // Re-initialize the ADC with the new, valid macro settings
    if (HAL_ADC_Init(hadc) != HAL_OK)
    {
        Error_Handler();
    }

    htim->Init.Period = params->timer_arr;
    htim->Init.Prescaler = params->timer_prescaler; 

    // Re-initialize the Timer
    if (HAL_TIM_Base_Init(htim) != HAL_OK)
    {
        Error_Handler();
    }
}