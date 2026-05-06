#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(board_init, APPLICATION, 0);
