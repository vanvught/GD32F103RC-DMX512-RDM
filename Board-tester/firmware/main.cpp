/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2022-2023 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cstdint>
#include <cstdio>
#include <time.h>

#include "hardware.h"
#include "display.h"

#include "gd32.h"

static constexpr auto GPIOA_PINS = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
static constexpr auto GPIOB_PINS = (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_15);
static constexpr auto GPIOC_PINS = (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
static constexpr auto GPIOD_PINS = GPIO_PIN_2;

int main(void) {
	Hardware hw;
	Display display;

	display.Cls();
	display.PutString(GD32_BOARD_NAME);

	puts("Board tester\nAll GPIO's are set to output HIGH");

	puts("rcu_periph_clock_enable\n");
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_AF);

	puts("gpio_init\n");
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOA_PINS);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOB_PINS);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOC_PINS);
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOD_PINS);

	puts("gpio_pin_remap_config\n");
	gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);

	puts("GPIO_BOP\n");
	GPIO_BOP(GPIOA) = GPIOA_PINS;
	GPIO_BOP(GPIOB) = GPIOB_PINS;
	GPIO_BOP(GPIOC) = GPIOC_PINS;
	GPIO_BOP(GPIOD) = GPIOD_PINS;

	puts("Running!\n");

	auto t0 = time(nullptr);
	uint32_t toggle = 0;

	while (1) {
		auto t1 = time(nullptr);
		if (t1 != t0) {
			t0 = t1;

			toggle ^= 0x1;

			if (toggle != 0) {
				puts("GPIO_BOP\n");
				GPIO_BOP(GPIOA) = GPIOA_PINS;
				GPIO_BOP(GPIOB) = GPIOB_PINS;
				GPIO_BOP(GPIOC) = GPIOC_PINS;
				GPIO_BOP(GPIOD) = GPIOD_PINS;
			} else {
				puts("GPIO_BC\n");
				GPIO_BC(GPIOA) = GPIOA_PINS;
				GPIO_BC(GPIOB) = GPIOB_PINS;
				GPIO_BC(GPIOC) = GPIOC_PINS;
				GPIO_BC(GPIOD) = GPIOD_PINS;
			}
		}
	}
}
