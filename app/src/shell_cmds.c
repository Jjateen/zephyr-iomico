#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include "led_sensor.h"

#define LED_SENSOR_NODE DT_NODELABEL(led_sensor0)

static const struct device *get_dev(void)
{
	return DEVICE_DT_GET(LED_SENSOR_NODE);
}

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	int ret = sensor_sample_fetch(get_dev());

	if (ret) {
		shell_error(sh, "sensor_sample_fetch failed: %d", ret);
	} else {
		shell_print(sh, "fetch ok (LED ON)");
	}
	return ret;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	struct sensor_value val;
	int ret = sensor_channel_get(get_dev(), SENSOR_CHAN_PROX, &val);

	if (ret) {
		shell_error(sh, "sensor_channel_get failed: %d", ret);
	} else {
		shell_print(sh, "LED state: %d (LED OFF)", val.val1);
	}
	return ret;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	const struct device *dev = get_dev();

	shell_print(sh, "device : %s", dev->name);
	shell_print(sh, "ready  : %s", device_is_ready(dev) ? "yes" : "no");
	shell_print(sh, "rate   : %u ms", led_sensor_get_blink_rate(dev));
	return 0;
}

/* L7T2: set blink_rate_ms via extension API */
static int cmd_set(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2) {
		shell_error(sh, "usage: sensor set <rate_ms>");
		return -EINVAL;
	}

	char *end;
	long rate_ms = strtol(argv[1], &end, 10);

	if (*end != '\0' || rate_ms < 100 || rate_ms > 10000) {
		shell_error(sh, "rate_ms must be in range [100, 10000]");
		return -EINVAL;
	}

	int ret = led_sensor_set_blink_rate(get_dev(), (uint32_t)rate_ms);

	if (ret) {
		shell_error(sh, "set_blink_rate failed: %d", ret);
	} else {
		shell_print(sh, "blink_rate set to %ld ms", rate_ms);
	}
	return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmds,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch() — turns LED on", cmd_fetch),
	SHELL_CMD(read,  NULL, "Call sensor_channel_get() — prints state, turns LED off", cmd_read),
	SHELL_CMD(info,  NULL, "Print device name, ready state, and blink rate", cmd_info),
	SHELL_CMD_ARG(set, NULL, "Set blink_rate_ms <100-10000>", cmd_set, 2, 0),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmds, "LED sensor shell commands", NULL);
