cmake_minimum_required(VERSION 3.20.0)

list(APPEND DTC_OVERLAY_FILE app.overlay)

find_package(Zephyr REQUIRED)
project(my_zephyr_app)

target_sources(app PRIVATE
    src/main.c
    src/shell_cmds.c
    drivers/led_sensor/led_sensor.c
)
target_include_directories(app PRIVATE drivers/led_sensor)
