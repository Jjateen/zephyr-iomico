#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "led_sensor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_SENSOR_NODE DT_NODELABEL(led_sensor0)
static const struct device *led_dev = DEVICE_DT_GET(LED_SENSOR_NODE);

int main(void)
{
	if (!device_is_ready(led_dev)) {
		LOG_ERR("LED sensor not ready");
		return -1;
	}

	LOG_INF("LED sensor ready: %s", led_dev->name);

	while (1) {
		struct sensor_value val;

		sensor_sample_fetch(led_dev);
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

		sensor_channel_get(led_dev, SENSOR_CHAN_PROX, &val);
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

		LOG_INF("LED state was: %d", val.val1);
	}

	return 0;
}
