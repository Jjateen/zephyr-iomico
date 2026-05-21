#define DT_DRV_COMPAT jjateen_led_sensor

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "led_sensor.h"

LOG_MODULE_REGISTER(led_sensor, LOG_LEVEL_DBG);

#define LED_SENSOR_BLINK_RATE_MIN_MS 100U
#define LED_SENSOR_BLINK_RATE_MAX_MS 10000U

struct led_sensor_config {
	struct gpio_dt_spec led;
};

struct led_sensor_data {
	int state;           /* 1 = ON, 0 = OFF */
	uint32_t blink_rate_ms;
};

/* sample_fetch: turn the LED on and record state */
static int led_sensor_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_PROX) {
		return -ENOTSUP;
	}

	gpio_pin_set_dt(&cfg->led, 1);
	data->state = 1;
	LOG_DBG("LED ON");
	return 0;
}

/* channel_get: return last state and turn the LED off */
static int led_sensor_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	if (chan != SENSOR_CHAN_PROX) {
		return -ENOTSUP;
	}

	val->val1 = data->state;
	val->val2 = 0;

	gpio_pin_set_dt(&cfg->led, 0);
	data->state = 0;
	LOG_DBG("LED OFF, returned state %d", val->val1);
	return 0;
}

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}

	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

/* Extension API */
int led_sensor_set_blink_rate(const struct device *dev, uint32_t rate_ms)
{
	struct led_sensor_data *data = dev->data;

	if (rate_ms < LED_SENSOR_BLINK_RATE_MIN_MS ||
	    rate_ms > LED_SENSOR_BLINK_RATE_MAX_MS) {
		LOG_ERR("blink_rate %u out of range [%u, %u]",
			rate_ms,
			LED_SENSOR_BLINK_RATE_MIN_MS,
			LED_SENSOR_BLINK_RATE_MAX_MS);
		return -EINVAL;
	}

	data->blink_rate_ms = rate_ms;
	LOG_INF("blink_rate_ms set to %u", rate_ms);
	return 0;
}

uint32_t led_sensor_get_blink_rate(const struct device *dev)
{
	const struct led_sensor_data *data = dev->data;

	return data->blink_rate_ms;
}

static DEVICE_API(sensor, led_sensor_driver_api) = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get  = led_sensor_channel_get,
};

#define LED_SENSOR_DEFINE(inst)						    \
	static struct led_sensor_data led_sensor_data_##inst = {	    \
		.blink_rate_ms = 500U,					    \
	};								    \
	static const struct led_sensor_config led_sensor_cfg_##inst = {	    \
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),		    \
	};								    \
	SENSOR_DEVICE_DT_INST_DEFINE(inst,				    \
				     led_sensor_init, NULL,		    \
				     &led_sensor_data_##inst,		    \
				     &led_sensor_cfg_##inst,		    \
				     POST_KERNEL,			    \
				     CONFIG_SENSOR_INIT_PRIORITY,	    \
				     &led_sensor_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)
