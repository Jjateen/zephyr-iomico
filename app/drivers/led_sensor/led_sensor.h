#ifndef LED_SENSOR_H
#define LED_SENSOR_H

#include <zephyr/device.h>

/**
 * @brief Set the blink rate stored in the driver's dynamic data.
 *
 * @param dev     LED sensor device
 * @param rate_ms Blink rate in milliseconds
 * @return 0 on success, -EINVAL if out of range
 */
int led_sensor_set_blink_rate(const struct device *dev, uint32_t rate_ms);

/**
 * @brief Get the current blink rate from the driver's dynamic data.
 *
 * @param dev LED sensor device
 * @return Current blink rate in milliseconds
 */
uint32_t led_sensor_get_blink_rate(const struct device *dev);

#endif /* LED_SENSOR_H */
