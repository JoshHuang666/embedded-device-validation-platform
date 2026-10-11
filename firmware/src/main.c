#include <stddef.h>
#include <stdio.h>
#include "device.h"
static const char *status(DeviceResult result)
{
    switch (result) {
    case DEV_OK: return "PASS";
    case DEV_ERR_VOLTAGE: return "REGULATION FAIL";
    case DEV_ERR_OCP: return "FAULT (OCP)";
    case DEV_ERR_OPP: return "FAULT (OPP)";
    case DEV_ERR_OVP: return "FAULT (OVP)";
    default: return "ERROR";
    }
}
int main(void)
{
    const float targets[] = {12.0f, 12.0f, 12.0f, 12.0f, 24.0f, 24.0f};
    const float loads[] = {0.0f, 3.0f, 5.0f, 5.1f, 4.0f, 4.5f};
    const DeviceResult expected[] = {DEV_OK, DEV_OK, DEV_ERR_VOLTAGE, DEV_ERR_OCP, DEV_OK, DEV_ERR_OPP};
    for (size_t i = 0; i < sizeof(loads) / sizeof(loads[0]); ++i) {
        DeviceController dev;
        if (device_init(&dev) != DEV_OK || device_set_voltage(&dev, targets[i]) != DEV_OK ||
            device_start(&dev) != DEV_OK) return 1;
        DeviceResult result = device_update(&dev, loads[i]);
        printf("Target: %.2f V, Load: %.2f A, Vout: %.2f V, Power: %.2f W -> %s\n",
            dev.target_voltage, loads[i], dev.measured_voltage, dev.measured_power, status(result));
        if (result != expected[i]) return 1;
    }
    puts("FAULT measurements above are pre-protection snapshots.");
    return 0;
}
