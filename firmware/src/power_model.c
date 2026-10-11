#include <stddef.h>
#include <math.h>
#include "power_model.h"

PowerResult power_model_calculate(const PowerModelConfig *config,
    float target_voltage, float load_current, PowerMeasurement *result)
{
    if (config == NULL || result == NULL) return POWER_ERR_ARGUMENT;
    if (!isfinite(target_voltage) || target_voltage <= 0.0f ||
        !isfinite(load_current) || load_current < 0.0f ||
        !isfinite(config->equivalent_resistance) || config->equivalent_resistance < 0.0f ||
        !isfinite(config->max_current) || config->max_current <= 0.0f ||
        !isfinite(config->max_power) || config->max_power <= 0.0f)
        return POWER_ERR_ARGUMENT;
    float drop = load_current * config->equivalent_resistance;
    if (!isfinite(drop)) return POWER_ERR_ARGUMENT;
    /* Output cannot go below zero in this simplified DC source model. */
    PowerMeasurement next = {fmaxf(0.0f, target_voltage - drop), load_current, 0.0f};
    next.power = next.voltage * next.current;
    if (!isfinite(next.power)) return POWER_ERR_ARGUMENT;
    *result = next;
    /* OCP wins if both protection thresholds are exceeded. */
    if (next.current > config->max_current) return POWER_ERR_OCP;
    if (next.power > config->max_power) return POWER_ERR_OPP;
    return POWER_OK;
}
