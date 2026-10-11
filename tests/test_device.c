#include <stdio.h>
#include "device.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "Failed: %s at line %d\n", #expr, __LINE__); return 1; } } while (0)
int main(void)
{
    DeviceController dev;
    CHECK(device_init(&dev) == DEV_OK);
    CHECK(dev.state == STATE_READY);
    CHECK(dev.target_voltage == 12.0f);
    CHECK(device_start(&dev) == DEV_OK);
    CHECK(dev.state == STATE_RUNNING);
    CHECK(device_start(&dev) == DEV_ERR_STATE);
    CHECK(dev.state == STATE_RUNNING);
    CHECK(device_stop(&dev) == DEV_OK);
    CHECK(dev.state == STATE_READY);
    CHECK(device_stop(&dev) == DEV_ERR_STATE);
    CHECK(device_init(NULL) == DEV_ERR_ARGUMENT);
    CHECK(device_start(NULL) == DEV_ERR_ARGUMENT);
    CHECK(device_stop(NULL) == DEV_ERR_ARGUMENT);
    dev.state = STATE_FAULT;
    CHECK(device_start(&dev) == DEV_ERR_STATE);
    CHECK(device_stop(&dev) == DEV_ERR_STATE);
    CHECK(dev.state == STATE_FAULT);
    dev.state = STATE_INIT;
    CHECK(device_start(&dev) == DEV_ERR_STATE);
    CHECK(device_stop(&dev) == DEV_ERR_STATE);
    puts("All device tests passed.");
    return 0;
}
