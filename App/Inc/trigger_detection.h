#ifndef TRIGGER_DETECTION_H
#define TRIGGER_DETECTION_H

// Threshold Triggering -> Hardware
// Edge Detection CPU
// Timing => Where to place the edge detection

#include <stdint.h>
#include "main.h"

typedef enum {
    TRIGGER_NONE = 0,       // Free-running mode (no trigger, continuous stream)
    TRIGGER_LEVEL_HIGH,     // Simple threshold: trigger when signal > max_threshold
    TRIGGER_LEVEL_LOW,      // Simple threshold: trigger when signal < low_threshold
    TRIGGER_EDGE_RISING,    // Schmitt trigger: arm < low_threshold, trigger > max_threshold
    TRIGGER_EDGE_FALLING    // Schmitt trigger: arm > max_threshold, trigger < low_threshold
} trigger_type_t;

typedef struct {
    trigger_type_t type;            // Which trigger mode is currently active
    uint32_t max_threshold;         // Upper raw 16-bit limit (0 to 65535)
    uint32_t low_threshold;         // Lower raw 16-bit limit (0 to 65535)
    uint32_t pre_trigger_samples;   // How many historical samples to keep before T=0 (4 bytes)
    uint32_t post_trigger_samples;
} t_trigger_params;

extern t_trigger_params trigger_params;


void set_trigger_params(t_trigger_params *trigger_params, ADC_HandleTypeDef *hadc, ADC_AnalogWDGConfTypeDef *AnalogWDGConfig);


#endif   /*TRIGGER_DETECTION_H*/ 