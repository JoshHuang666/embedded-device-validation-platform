#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "device.h"
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "CHECK FAILED: %s:%d: %s\n", __FILE__, __LINE__, #condition); exit(EXIT_FAILURE); } } while (0)

int main(void)
{
    DeviceController dev;

    CHECK(device_init(&dev) == DEV_OK);

    // Valid voltage
    CHECK(device_set_voltage(&dev, 12.0f) == DEV_OK);

    // Invalid voltage
    CHECK(device_set_voltage(&dev, 30.0f)
           == DEV_ERR_RANGE);

    CHECK(device_set_voltage(&dev, -5.0f)
           == DEV_ERR_RANGE);

    // Start device
    CHECK(device_start(&dev) == DEV_OK);

    // Cannot change voltage while running
    CHECK(device_set_voltage(&dev, 18.0f)
           == DEV_ERR_STATE);

    // Within tolerance
    CHECK(device_sample_voltage(&dev, 12.0f)
           == DEV_OK);

    // Outside tolerance
    CHECK(device_sample_voltage(&dev, 11.7f)
           == DEV_ERR_VOLTAGE);

    // Over-voltage protection
    CHECK(device_sample_voltage(&dev, 25.5f)
           == DEV_ERR_OVP);

    CHECK(dev.state == STATE_FAULT);
    /* WEEK1_OVP_OUTPUT */
    CHECK(dev.output_enabled == 0);

    // Cannot start in FAULT state
    CHECK(device_start(&dev) == DEV_ERR_STATE);

    // Reset fault
    CHECK(device_reset_fault(&dev) == DEV_OK);
    CHECK(dev.state == STATE_READY);

    CHECK(dev.fault_flags == 0u);
    CHECK(dev.measured_voltage == 0.0f);
    CHECK(device_reset_fault(&dev) == DEV_ERR_STATE);
    CHECK(device_set_voltage(NULL, 12.0f) == DEV_ERR_ARGUMENT);
    CHECK(device_sample_voltage(NULL, 12.0f) == DEV_ERR_ARGUMENT);
    CHECK(device_reset_fault(NULL) == DEV_ERR_ARGUMENT);
    CHECK(device_set_voltage(&dev, NAN) == DEV_ERR_RANGE);
    CHECK(device_set_voltage(&dev, INFINITY) == DEV_ERR_RANGE);
    CHECK(device_set_voltage(&dev, VOLTAGE_MIN) == DEV_OK);
    CHECK(device_set_voltage(&dev, VOLTAGE_MAX) == DEV_OK);
    CHECK(device_set_voltage(&dev, 12.0f) == DEV_OK);
    CHECK(device_sample_voltage(&dev, 12.0f) == DEV_ERR_STATE);
    CHECK(device_start(&dev) == DEV_OK);
    CHECK(device_sample_voltage(&dev, NAN) == DEV_ERR_ARGUMENT);
    CHECK(device_sample_voltage(&dev, INFINITY) == DEV_ERR_ARGUMENT);
    CHECK(device_sample_voltage(&dev, -1.0f) == DEV_ERR_ARGUMENT);
    CHECK(dev.measured_voltage == 0.0f);
    CHECK(device_sample_voltage(&dev, 12.1f) == DEV_OK);
    CHECK(device_sample_voltage(&dev, 11.7f) == DEV_ERR_VOLTAGE);
    CHECK(dev.state == STATE_RUNNING);
    CHECK(dev.fault_flags == 0u);
    CHECK(device_sample_voltage(&dev, OVP_LIMIT) == DEV_ERR_VOLTAGE);
    CHECK(dev.state == STATE_RUNNING);
    CHECK(device_sample_voltage(&dev, 25.5f) == DEV_ERR_OVP);
    CHECK((dev.fault_flags & 0x01u) != 0u);
    CHECK(device_stop(&dev) == DEV_ERR_STATE);
    CHECK(device_set_voltage(&dev, 12.0f) == DEV_ERR_STATE);
    CHECK(device_sample_voltage(&dev, 12.0f) == DEV_ERR_STATE);
    CHECK(device_reset_fault(&dev) == DEV_OK);
    CHECK(device_start(&dev) == DEV_OK);
    CHECK(device_sample_voltage(&dev, 12.0f) == DEV_OK);
    CHECK(device_stop(&dev) == DEV_OK);
    CHECK(dev.measured_voltage == 0.0f);
    printf("All voltage tests passed.\n");

    return 0;
}
