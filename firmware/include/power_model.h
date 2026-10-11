#ifndef POWER_MODEL_H
#define POWER_MODEL_H
typedef struct {
    float equivalent_resistance;
    float max_current;
    float max_power;
} PowerModelConfig;
typedef struct { float voltage; float current; float power; } PowerMeasurement;
typedef enum {
    POWER_OK = 0, POWER_ERR_ARGUMENT, POWER_ERR_OCP, POWER_ERR_OPP
} PowerResult;
PowerResult power_model_calculate(const PowerModelConfig *config,
    float target_voltage, float load_current, PowerMeasurement *result);
#endif
