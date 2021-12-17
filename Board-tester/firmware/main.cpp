/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@gd32-dmx.org
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
#include <time.h>

#include "hardware.h"
#include "display.h"

extern "C" {
#include "gd32f10x.h"

void uart0_puts(const char *);
void uart0_putc(int);
}

// Skip not used PINs and skip USART0
static constexpr auto GPIOA_PINS = ~(GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
static constexpr auto GPIOB_PINS = ~(GPIO_PIN_1);
static constexpr auto GPIOC_PINS = ~(GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_13);
static constexpr auto GPIOD_PINS = GPIO_PIN_2;

int main(void) {
	Hardware hw;
	Display display;

	display.Cls();
	display.PutString("GD32F103RC");

	uart0_puts("Board tester\nAll GPIO's are set to output HIGH");

	uart0_puts("rcu_periph_clock_enable\n");
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_AF);

	uart0_puts("gpio_init\n");
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOA_PINS);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOB_PINS);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOC_PINS);
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOD_PINS);

	uart0_puts("gpio_pin_remap_config\n");
	gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);

	uart0_puts("GPIO_BOP\n");
	GPIO_BOP(GPIOA) = GPIOA_PINS;
	GPIO_BOP(GPIOB) = GPIOB_PINS;
	GPIO_BOP(GPIOC) = GPIOC_PINS;
	GPIO_BOP(GPIOD) = GPIOD_PINS;

	uart0_puts("Running!\n");

	auto t0 = time(nullptr);
	uint32_t toggle = 0;

	while (1) {
		auto t1 = time(nullptr);
		if (t1 != t0) {
			t0 = t1;

			toggle ^= 0x1;

			if (toggle != 0) {
				uart0_puts("GPIO_BOP\n");
				GPIO_BOP(GPIOA) = GPIOA_PINS;
				GPIO_BOP(GPIOB) = GPIOB_PINS;
				GPIO_BOP(GPIOC) = GPIOC_PINS;
				GPIO_BOP(GPIOD) = GPIOD_PINS;
			} else {
				uart0_puts("GPIO_BC\n");
				GPIO_BC(GPIOA) = GPIOA_PINS;
				GPIO_BC(GPIOB) = GPIOB_PINS;
				GPIO_BC(GPIOC) = GPIOC_PINS;
				GPIO_BC(GPIOD) = GPIOD_PINS;
			}
		}
	}
}

