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

	/* L6T2: call extension API to set blink_rate_ms in driver data */
	led_sensor_set_blink_rate(led_dev, 300);
	LOG_INF("blink rate -> %u ms", led_sensor_get_blink_rate(led_dev));

	while (1) {
		struct sensor_value val;
		uint32_t rate_ms = led_sensor_get_blink_rate(led_dev);

		sensor_sample_fetch(led_dev);          /* LED ON  */
		k_msleep(rate_ms);

		sensor_channel_get(led_dev, SENSOR_CHAN_PROX, &val); /* LED OFF */
		k_msleep(rate_ms);

		LOG_INF("blink cycle done, state was: %d", val.val1);
	}

	return 0;
}
