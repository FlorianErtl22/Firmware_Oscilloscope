#include "trigger_detection.h"
#include "main.h"
#include "communication.h"
#include "adc.h"

t_trigger_params trigger_params = {
    .type = TRIGGER_NONE,
    .max_threshold = 65535,     // Safe default max
    .low_threshold = 0,         // Safe default min
    .pre_trigger_samples = 100, // Default time scale
    .post_trigger_samples = 924 // Assuming a 1024 buffer
};

void set_trigger_params(t_trigger_params *trigger_params, ADC_HandleTypeDef *hadc, ADC_AnalogWDGConfTypeDef *AnalogWDGConfig)
{
    uint32_t awd1_high, awd1_low;
    uint32_t awd2_high, awd2_low;

    AnalogWDGConfig->ITMode = DISABLE; // KEEP DISABLED IN CONFIG
    switch (trigger_params->type)
    {
    case TRIGGER_LEVEL_HIGH:
        // AWD1 catches > max. AWD2 is unused.
        awd1_high = trigger_params->max_threshold;
        awd1_low = 0x0000;

        awd2_high = 0xFFFF; // Dummy
        awd2_low = 0x0000;  // Dummy
        break;

    case TRIGGER_LEVEL_LOW:
        // AWD1 catches < low. AWD2 is unused.
        awd1_high = 0xFFFF;
        awd1_low = trigger_params->low_threshold;

        awd2_high = 0xFFFF; // Dummy
        awd2_low = 0x0000;  // Dummy
        break;

    case TRIGGER_EDGE_RISING:
        // AWD1 (Arming): Must drop BELOW low_threshold first
        awd1_high = 0xFFFF;
        awd1_low = trigger_params->low_threshold;

        // AWD2 (Triggering): Must spike ABOVE max_threshold second
        awd2_high = trigger_params->max_threshold;
        awd2_low = 0x0000;
        break;

    case TRIGGER_EDGE_FALLING:
        // AWD1 (Arming): Must spike ABOVE max_threshold first
        awd1_high = trigger_params->max_threshold;
        awd1_low = 0x0000;

        // AWD2 (Triggering): Must drop BELOW low_threshold second
        awd2_high = 0xFFFF;
        awd2_low = trigger_params->low_threshold;
        break;

    case TRIGGER_NONE:
    default:
        // Everything off
        trigger_params->type = TRIGGER_NONE;
        awd1_high = 0xFFFF;
        awd1_low = 0x0000;

        awd2_high = 0xFFFF;
        awd2_low = 0x0000;
        break;
    }

    AnalogWDGConfig->WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig->HighThreshold = awd1_high;
    AnalogWDGConfig->LowThreshold = awd1_low;
    if (HAL_ADC_AnalogWDGConfig(hadc, AnalogWDGConfig) != HAL_OK)
    {
        Error_Handler();
    }

    AnalogWDGConfig->WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig->HighThreshold = awd2_high;
    AnalogWDGConfig->LowThreshold = awd2_low;
    if (HAL_ADC_AnalogWDGConfig(hadc, AnalogWDGConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
    // --- CASE A: SIMPLE LEVEL TRIGGERS ---
    if (trigger_params.type == TRIGGER_LEVEL_HIGH || trigger_params.type == TRIGGER_LEVEL_LOW)
    {
        uint32_t ndtr = __HAL_DMA_GET_COUNTER(hadc->DMA_Handle);

        com_params.trigger_index = TX_BUFFER_SIZE - ndtr;

        // Handle wrap-around safety
        if (com_params.trigger_index >= TX_BUFFER_SIZE)
        {
            com_params.trigger_index = 0;
        }
        com_params.trigger_detected = 1;

        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
    }

    // --- CASE B: EDGE DETECTION ARMING ---
    else if (trigger_params.type == TRIGGER_EDGE_RISING)
    {
        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
        __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_AWD2);
    }
    else if (trigger_params.type == TRIGGER_EDGE_FALLING)
    {
        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD1);
        __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_AWD2);
    }
}

void HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef *hadc)
{
    if (trigger_params.type == TRIGGER_EDGE_RISING || trigger_params.type == TRIGGER_EDGE_FALLING)
    {
        uint32_t ndtr = __HAL_DMA_GET_COUNTER(hadc->DMA_Handle);
        com_params.trigger_index = TX_BUFFER_SIZE - ndtr;

        // Handle wrap-around safety
        if (com_params.trigger_index >= TX_BUFFER_SIZE)
        {
            com_params.trigger_index = 0;
        }
        com_params.trigger_detected = 1;

        __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_AWD2);
    }
}