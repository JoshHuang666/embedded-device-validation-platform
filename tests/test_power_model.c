#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "device.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define NEAR(a,b) CHECK(fabsf((a)-(b)) < 1e-4f)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    int tc = atoi(argv[1]);
    PowerModelConfig config = {0.05f, 5.0f, 100.0f};
    PowerMeasurement m = {0.0f, 0.0f, 0.0f};
    DeviceController dev;
    CHECK(device_init(&dev) == DEV_OK);
    /* WEEK1_OUTPUT_CHECKS: initialized output is disabled. */
    CHECK(dev.output_enabled == 0);
    CHECK(device_start(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 1);
    CHECK(dev.output_enabled == 1);
    switch (tc) {
    case 1:
        CHECK(power_model_calculate(&config, 12.0f, 0.0f, &m) == POWER_OK);
        NEAR(m.voltage, 12.0f); NEAR(m.current, 0.0f); NEAR(m.power, 0.0f); break;
    case 2:
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, &m) == POWER_OK);
        NEAR(m.voltage, 11.85f); NEAR(m.power, 35.55f);
        CHECK(device_update(&dev, 3.0f) == DEV_OK); break;
    case 3:
        CHECK(power_model_calculate(&config, 12.0f, 5.0f, &m) == POWER_OK);
        NEAR(m.voltage, 11.75f);
        CHECK(device_update(&dev, 5.0f) == DEV_ERR_VOLTAGE);
        CHECK(dev.state == STATE_RUNNING && dev.fault_flags == 0u); break;
    case 4:
        CHECK(power_model_calculate(&config, 12.0f, 5.1f, &m) == POWER_ERR_OCP);
        CHECK(device_update(&dev, 5.1f) == DEV_ERR_OCP);
        CHECK(dev.output_enabled == 0);
        CHECK(dev.state == STATE_FAULT && dev.fault_flags == FAULT_OCP); break;
    case 5:
        CHECK(power_model_calculate(&config, 24.0f, 4.0f, &m) == POWER_OK);
        NEAR(m.voltage, 23.8f); NEAR(m.power, 95.2f); break;
    case 6:
        CHECK(power_model_calculate(&config, 24.0f, 4.5f, &m) == POWER_ERR_OPP);
        NEAR(m.power, 106.9875f); break;
    case 7:
        m.voltage = 123.0f;
        CHECK(power_model_calculate(&config, 12.0f, -1.0f, &m) == POWER_ERR_ARGUMENT);
        NEAR(m.voltage, 123.0f);
        CHECK(device_update(&dev, -1.0f) == DEV_ERR_ARGUMENT);
        CHECK(dev.state == STATE_RUNNING); break;
    case 8:
        CHECK(power_model_calculate(&config, NAN, 3.0f, &m) == POWER_ERR_ARGUMENT);
        CHECK(power_model_calculate(&config, 12.0f, NAN, &m) == POWER_ERR_ARGUMENT);
        CHECK(power_model_calculate(&config, INFINITY, 3.0f, &m) == POWER_ERR_ARGUMENT);
        CHECK(power_model_calculate(&config, 12.0f, INFINITY, &m) == POWER_ERR_ARGUMENT); break;
    case 9:
        CHECK(power_model_calculate(NULL, 12.0f, 3.0f, &m) == POWER_ERR_ARGUMENT);
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, NULL) == POWER_ERR_ARGUMENT);
        CHECK(device_update(NULL, 3.0f) == DEV_ERR_ARGUMENT);
        config.equivalent_resistance = -0.05f;
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, &m) == POWER_ERR_ARGUMENT);
        config.equivalent_resistance = NAN;
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, &m) == POWER_ERR_ARGUMENT);
        config.equivalent_resistance = 0.05f; config.max_current = 0.0f;
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, &m) == POWER_ERR_ARGUMENT);
        config.max_current = 5.0f; config.max_power = INFINITY;
        CHECK(power_model_calculate(&config, 12.0f, 3.0f, &m) == POWER_ERR_ARGUMENT); break;
    case 10:
        CHECK(device_update(&dev, 5.1f) == DEV_ERR_OCP);
        CHECK(device_reset_fault(&dev) == DEV_ERR_UNSAFE);
        CHECK(dev.output_enabled == 0);
        CHECK(device_update(&dev, 5.1f) == DEV_ERR_UNSAFE);
        CHECK(device_update(&dev, 3.0f) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(dev.state == STATE_FAULT && dev.fault_flags == FAULT_OCP);
        NEAR(dev.measured_current, 5.1f);
        CHECK(device_update(&dev, NAN) == DEV_ERR_ARGUMENT);
        CHECK(device_reset_fault(&dev) == DEV_ERR_UNSAFE);
        CHECK(dev.output_enabled == 0);
        CHECK(device_update(&dev, 3.0f) == DEV_OK);
        CHECK(device_reset_fault(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(dev.state == STATE_READY && dev.fault_flags == 0u);
        NEAR(dev.measured_power, 0.0f);
        CHECK(device_start(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 1);
        CHECK(device_update(&dev, 3.0f) == DEV_OK); break;
    case 11:
        CHECK(device_stop(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(device_set_voltage(&dev, 24.0f) == DEV_OK);
        CHECK(device_start(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 1);
        CHECK(device_update(&dev, 4.5f) == DEV_ERR_OPP);
        CHECK(dev.fault_flags == FAULT_OPP);
        CHECK(dev.output_enabled == 0);
        CHECK(device_reset_fault(&dev) == DEV_ERR_UNSAFE);
        CHECK(dev.output_enabled == 0);
        CHECK(device_update(&dev, 4.0f) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(dev.state == STATE_FAULT);
        CHECK(device_reset_fault(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(device_start(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 1);
        CHECK(device_update(&dev, 4.0f) == DEV_OK); break;
    case 12:
        CHECK(device_stop(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 0);
        CHECK(device_set_voltage(&dev, 24.0f) == DEV_OK);
        CHECK(device_start(&dev) == DEV_OK);
        CHECK(dev.output_enabled == 1);
        CHECK(device_update(&dev, 5.1f) == DEV_ERR_OCP);
        CHECK(dev.fault_flags == (FAULT_OCP | FAULT_OPP));
        CHECK(dev.output_enabled == 0);
        CHECK(device_reset_fault(&dev) == DEV_ERR_UNSAFE);
        CHECK(dev.output_enabled == 0); break;
    case 13:
        config.equivalent_resistance = 0.0f;
        CHECK(power_model_calculate(&config, 20.0f, 5.0f, &m) == POWER_OK);
        NEAR(m.power, 100.0f);
        CHECK(power_model_calculate(&config, 20.01f, 5.0f, &m) == POWER_ERR_OPP); break;
    case 14:
        CHECK(power_model_calculate(&config, 1.0f, 100.0f, &m) == POWER_ERR_OCP);
        NEAR(m.voltage, 0.0f); NEAR(m.power, 0.0f);
        config.equivalent_resistance = FLT_MAX;
        CHECK(power_model_calculate(&config, 12.0f, 2.0f, &m) == POWER_ERR_ARGUMENT);
        config.equivalent_resistance = 0.0f;
        CHECK(power_model_calculate(&config, FLT_MAX, 2.0f, &m) == POWER_ERR_ARGUMENT); break;
    default: CHECK(0);
    }
    printf("Power model TC%02d passed.\n", tc);
    return 0;
}
