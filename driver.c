#include "driver.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#define LCD_CMD 0
#define LCD_CHR 1

void gpio_init_pin(int GPIO) {
	gpio_request(GPIO, "sysfs");
	gpio_direction_output(GPIO, 1);
	gpio_export(GPIO, false);
}

void gpio_cleanup_pin(int GPIO) {
	gpio_set_value(GPIO, 0);
	gpio_unexport(GPIO);
	gpio_free(GPIO);
}

void gpio_init(void) {
	gpio_init_pin(7);
	gpio_init_pin(8);
	gpio_init_pin(25);
	gpio_init_pin(24);
	gpio_init_pin(23);
	gpio_init_pin(18);
}

void gpio_cleanup(void) {
	gpio_cleanup_pin(7);
	gpio_cleanup_pin(8);
	gpio_cleanup_pin(25);
	gpio_cleanup_pin(24);
	gpio_cleanup_pin(23);
	gpio_cleanup_pin(18);
}

void driver_toggle_enable(void) {
	msleep(1);
	gpio_set_value(8, 1);
	msleep(1);
	gpio_set_value(8, 0);
	msleep(1);
}

void driver_send_byte(char bits, int mode) {
	/*
	 * RS - 7
	 * E - 8
	 * D4 - 25
	 * D5 - 24
	 * D6 - 23
	 * D7 - 18
	*/

	gpio_set_value(7, mode);

	gpio_set_value(25, 0);
	gpio_set_value(24, 0);
	gpio_set_value(23, 0);
	gpio_set_value(18, 0);

	if (bits & 0x10) {
		gpio_set_value(25, 1);
	}
	if (bits & 0x20) {
		gpio_set_value(24, 1);
	}
	if (bits & 0x40) {
		gpio_set_value(23, 1);
	}
	if (bits & 0x80) {
		gpio_set_value(18, 1);
	}

	driver_toggle_enable();

	gpio_set_value(25, 0);
	gpio_set_value(24, 0);
	gpio_set_value(23, 0);
	gpio_set_value(18, 0);

	if (bits & 0x01) {
		gpio_set_value(25, 1);
	}
	if (bits & 0x02) {
		gpio_set_value(24, 1);
	}
	if (bits & 0x04) {
		gpio_set_value(23, 1);
	}
	if (bits & 0x08) {
		gpio_set_value(18, 1);
	}

	driver_toggle_enable();
}

void driver_init(void) {
	gpio_init();

	driver_send_byte(0x33, LCD_CMD);
	driver_send_byte(0x32, LCD_CMD);
	driver_send_byte(0x06, LCD_CMD);
	driver_send_byte(0x0C, LCD_CMD);
	driver_send_byte(0x28, LCD_CMD);
	driver_send_byte(0x01, LCD_CMD);
	msleep(1);
}

void driver_cleanup(void) {
	driver_send_byte(0x01, LCD_CMD);
	gpio_cleanup();
}

void driver_print(const char *msg) {
	driver_send_byte(0x80, LCD_CMD);

	while (*msg) {
		driver_send_byte(*(msg++), LCD_CHR);
	}
}
