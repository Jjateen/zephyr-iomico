#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct pwm_dt_spec leds[] = {
	PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0)),  /* Red   */
	PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1)),  /* Green */
	PWM_DT_SPEC_GET(DT_ALIAS(pwm_led2)),  /* Blue  */
};
static const char *const color_names[] = { "RED", "GREEN", "BLUE" };

static void set_brightness(const struct pwm_dt_spec *led, int percent)
{
	uint32_t pulse = (uint32_t)((uint64_t)led->period * percent / 100);
	pwm_set_pulse_dt(led, pulse);
}

static void all_off(void)
{
	for (int i = 0; i < 3; i++) {
		set_brightness(&leds[i], 0);
	}
}

int main(void)
{
	for (int i = 0; i < 3; i++) {
		if (!pwm_is_ready_dt(&leds[i])) {
			LOG_ERR("PWM led%d not ready", i);
			return 0;
		}
	}

	const int fade_steps   = 50;
	const int fade_step_ms = CONFIG_LED_FADE_DURATION / fade_steps;
	int color = 0;

	while (1) {
		all_off();

		/* Fade in current color */
		for (int i = 0; i <= fade_steps; i++) {
			int level = CONFIG_LED_BRIGHTNESS * i / fade_steps;
			set_brightness(&leds[color], level);
			k_msleep(fade_step_ms);
		}

		LOG_INF("%s ON at %d%%", color_names[color], CONFIG_LED_BRIGHTNESS);
		k_msleep(CONFIG_LED_BLINK_SLEEP_MS);

		/* Fade out */
		for (int i = fade_steps; i >= 0; i--) {
			int level = CONFIG_LED_BRIGHTNESS * i / fade_steps;
			set_brightness(&leds[color], level);
			k_msleep(fade_step_ms);
		}

		LOG_INF("%s OFF", color_names[color]);

		color = (color + 1) % 3;  /* Red -> Green -> Blue -> Red */
	}

	return 0;
}
