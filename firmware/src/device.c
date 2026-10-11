#include <stddef.h>
#include <math.h>
#include "device.h"
static const PowerModelConfig model = {EQUIVALENT_RESISTANCE, RATED_CURRENT, RATED_POWER};

DeviceResult device_init(DeviceController *dev)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    *dev = (DeviceController){.state = STATE_READY, .target_voltage = 12.0f, .load_safe = 1};
    return DEV_OK;
}
DeviceResult device_start(DeviceController *dev)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_READY) return DEV_ERR_STATE;
    dev->state = STATE_RUNNING;
    dev->output_enabled = 1;
    return DEV_OK;
}
DeviceResult device_stop(DeviceController *dev)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_RUNNING) return DEV_ERR_STATE;
    dev->state = STATE_READY;
    dev->output_enabled = 0;
    dev->measured_voltage = dev->measured_current = dev->measured_power = 0.0f;
    return DEV_OK;
}
DeviceResult device_set_voltage(DeviceController *dev, float voltage)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_READY) return DEV_ERR_STATE;
    if (!isfinite(voltage) || voltage < VOLTAGE_MIN || voltage > VOLTAGE_MAX)
        return DEV_ERR_RANGE;
    dev->target_voltage = voltage;
    return DEV_OK;
}
DeviceResult device_sample_voltage(DeviceController *dev, float measured_voltage)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_RUNNING) return DEV_ERR_STATE;
    if (!isfinite(measured_voltage) || measured_voltage < 0.0f) return DEV_ERR_ARGUMENT;
    dev->measured_voltage = measured_voltage;
    if (measured_voltage > OVP_LIMIT) {
        dev->fault_flags |= FAULT_OVP;
        dev->state = STATE_FAULT;
        dev->output_enabled = 0;
        return DEV_ERR_OVP;
    }
    if (fabsf(measured_voltage - dev->target_voltage) > dev->target_voltage * VOLTAGE_TOLERANCE)
        return DEV_ERR_VOLTAGE;
    return DEV_OK;
}
DeviceResult device_update(DeviceController *dev, float load_current)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_RUNNING && dev->state != STATE_FAULT) return DEV_ERR_STATE;
    PowerMeasurement measured;
    PowerResult result = power_model_calculate(&model, dev->target_voltage, load_current, &measured);
    if (result == POWER_ERR_ARGUMENT) {
        if (dev->state == STATE_FAULT) dev->load_safe = 0;
        return DEV_ERR_ARGUMENT;
    }
    dev->load_current = load_current;
    dev->load_safe = result == POWER_OK;
    /* In FAULT, do not replace the triggering measurements or clear any flags. */
    if (dev->state == STATE_FAULT)
        return dev->load_safe ? DEV_OK : DEV_ERR_UNSAFE;
    dev->measured_voltage = measured.voltage;
    dev->measured_current = measured.current;
    dev->measured_power = measured.power;
    if (result == POWER_ERR_OCP || result == POWER_ERR_OPP) {
        if (measured.current > RATED_CURRENT) dev->fault_flags |= FAULT_OCP;
        if (measured.power > RATED_POWER) dev->fault_flags |= FAULT_OPP;
        dev->state = STATE_FAULT;
        dev->output_enabled = 0;
        return result == POWER_ERR_OCP ? DEV_ERR_OCP : DEV_ERR_OPP;
    }
    return device_sample_voltage(dev, measured.voltage);
}
DeviceResult device_reset_fault(DeviceController *dev)
{
    if (dev == NULL) return DEV_ERR_ARGUMENT;
    if (dev->state != STATE_FAULT) return DEV_ERR_STATE;
    if ((dev->fault_flags & (FAULT_OCP | FAULT_OPP)) != 0u && !dev->load_safe)
        return DEV_ERR_UNSAFE;
    dev->fault_flags = 0u;
    dev->measured_voltage = dev->measured_current = dev->measured_power = 0.0f;
    dev->state = STATE_READY;
    dev->output_enabled = 0;
    return DEV_OK;
}
