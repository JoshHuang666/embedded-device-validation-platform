#ifndef DEVICE_H
#define DEVICE_H
#include "power_model.h"
#define VOLTAGE_MIN 1.0f
#define VOLTAGE_MAX 24.0f
#define VOLTAGE_TOLERANCE 0.02f
#define OVP_LIMIT 25.0f
#define RATED_CURRENT 5.0f
#define RATED_POWER 100.0f
#define EQUIVALENT_RESISTANCE 0.05f
#define FAULT_OVP 0x01u
#define FAULT_OCP 0x02u
#define FAULT_OPP 0x04u
typedef enum { STATE_INIT, STATE_READY, STATE_RUNNING, STATE_FAULT } DeviceState;
typedef enum {
    DEV_OK = 0, DEV_ERR_STATE, DEV_ERR_ARGUMENT, DEV_ERR_RANGE,
    DEV_ERR_VOLTAGE, DEV_ERR_OVP, DEV_ERR_OCP, DEV_ERR_OPP, DEV_ERR_UNSAFE
} DeviceResult;
typedef struct {
    DeviceState state;
    float target_voltage;
    float measured_voltage;
    unsigned int fault_flags;
    float measured_current;
    float measured_power;
    float load_current;
    int load_safe;
    int output_enabled;
} DeviceController;
DeviceResult device_init(DeviceController *dev);
DeviceResult device_start(DeviceController *dev);
DeviceResult device_stop(DeviceController *dev);
DeviceResult device_set_voltage(DeviceController *dev, float voltage);
DeviceResult device_sample_voltage(DeviceController *dev, float measured_voltage);
/* In FAULT, updates the load safety check only; protection stays latched. */
DeviceResult device_update(DeviceController *dev, float load_current);
DeviceResult device_reset_fault(DeviceController *dev);
#endif
