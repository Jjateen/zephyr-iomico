#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define APP_LED_NODE DT_ALIAS(app_led)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(APP_LED_NODE, gpios);

int main(void)
{
	if (!gpio_is_ready_dt(&led)) return 0;

	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

	while (1) {
		gpio_pin_toggle_dt(&led);
		LOG_INF("LED toggled (period %d ms)", CONFIG_APP_HEARTBEAT_PERIOD_MS);
		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
	}

	return 0;
}
