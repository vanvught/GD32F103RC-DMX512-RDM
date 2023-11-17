/**
 * @file ft245rl.cpp
 */
/* Copyright (C) 2021-2023 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <stdint.h>
#include <stdbool.h>

#include "gd32_gpio.h"
#include "gd32.h"

#define D0		GPIO_EXT_3	// PB9
#define D1		GPIO_EXT_5	// PB8
#define D2		GPIO_EXT_7	// PA6
#define D3		GPIO_EXT_26	// PA14
#define D4		GPIO_EXT_24	// PA15
#define D5		GPIO_EXT_21	// PB4
#define D6		GPIO_EXT_19	// PB5
#define D7		GPIO_EXT_23	// PB3

#define WR		GPIO_EXT_15	// PB14
#define _RD		GPIO_EXT_16	// PB15

#define _TXE	GPIO_EXT_18	// PA13
#define _RXF	GPIO_EXT_22	// PA11

#define NOP_COUNT_READ 24
#define NOP_COUNT_WRITE 2

#define GPIOA_DATA_PINS		(GPIO_PIN_6 | GPIO_PIN_14 | GPIO_PIN_15)
#define GPIOB_DATA_PINS		(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9)
/**
 * Set the GPIOs for data to output
 */
static void data_gpio_fsel_output() {
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOA_DATA_PINS);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIOB_DATA_PINS);
}

/**
 * Set the GPIOs for data to input
 */
static void data_gpio_fsel_input() {
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIOA_DATA_PINS);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIOB_DATA_PINS);

}

/**
 * Set RD#, WR to output, TXE#, RXF# to input.
 * Set RD# to high, set WR to low
 */
void FT245RL_init() {
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);

	gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);

	data_gpio_fsel_input();

	// _RD, WR output
	gd32_gpio_fsel(_RD, GPIO_FSEL_OUTPUT);
	gd32_gpio_fsel(WR, GPIO_FSEL_OUTPUT);
	// _TXE, _RXF input
	gd32_gpio_fsel(_TXE, GPIO_FSEL_INPUT);
	gd32_gpio_fsel(_RXF, GPIO_FSEL_INPUT);
	// RD#	high
	gd32_gpio_set(_RD);
	// WR	low
	gd32_gpio_clr(WR);
}

/**
 * Write 8-bits to USB
 */
void FT245RL_write_data(uint8_t data) {
	data_gpio_fsel_output();
	// Raise WR to start the write.
	gd32_gpio_set(WR);

	uint8_t i = NOP_COUNT_WRITE;
	for (; i > 0; i--) {
		__NOP();
	}

	// Put the data on the bus.

	uint32_t pin = 0;
	pin |= (data &  4) ? (GPIO_PIN_6 ) : 0;	// D2
	pin |= (data &  8) ? (GPIO_PIN_14) : 0;	// D3
	pin |= (data & 16) ? (GPIO_PIN_15) : 0;	// D4
	GPIO_BOP(GPIOA) = pin;
	pin ^= GPIOA_DATA_PINS;
	GPIO_BC(GPIOA) = pin;

	pin = 0;
	pin |= (data &   1) ? (GPIO_PIN_9) : 0;	// D0
	pin |= (data &   2) ? (GPIO_PIN_8) : 0;	// D1
	pin |= (data &  32) ? (GPIO_PIN_4) : 0;	// D5
	pin |= (data &  64) ? (GPIO_PIN_5) : 0;	// D6
	pin |= (data & 128) ? (GPIO_PIN_3) : 0;	// D7
	GPIO_BOP(GPIOB) = pin;
	pin ^= GPIOB_DATA_PINS;
	GPIO_BC(GPIOB) = pin;

	i = NOP_COUNT_WRITE;
	for (; i > 0; i--) {
		__NOP();
	}

	// Drop WR to tell the FT245 to read the data.
	gd32_gpio_clr(WR);
}

/**
 * Read 8-bits from USB
 */
uint8_t FT245RL_read_data() {
	data_gpio_fsel_input();

	gd32_gpio_clr(_RD);

	// Wait for the FT245 to respond with data.
	uint8_t i = NOP_COUNT_READ;
	for (; i > 0; i--) {
		__NOP();
	}

	// Read the data from the data port.
	uint8_t data = 0;
	const uint16_t in_gpio_a = (uint16_t)GPIO_ISTAT(GPIOA);
	const uint16_t in_gpio_b = (uint16_t)GPIO_ISTAT(GPIOB);

	data |= ((in_gpio_a & (GPIO_PIN_6 )) ?  4 : 0);
	data |= ((in_gpio_a & (GPIO_PIN_14)) ?  8 : 0);
	data |= ((in_gpio_a & (GPIO_PIN_15)) ? 16 : 0);

	data |= ((in_gpio_b & (GPIO_PIN_9)) ? 1 : 0);
	data |= ((in_gpio_b & (GPIO_PIN_8)) ? 2 : 0);
	data |= ((in_gpio_b & (GPIO_PIN_4)) ? 32 : 0);
	data |= ((in_gpio_b & (GPIO_PIN_5)) ? 64 : 0);
	data |= ((in_gpio_b & (GPIO_PIN_3)) ? 128 : 0);

	// Bring RD# back up so the FT245 can let go of the data.
	gd32_gpio_set(_RD);

	return data;
}

/**
 * Read RXF#
 */
bool FT245RL_data_available() {
	return gpio_input_bit_get(GPIOA, GPIO_PIN_11) == RESET;
}

/**
 * Read TXE#
 */
bool FT245RL_can_write() {
	return gpio_input_bit_get(GPIOA, GPIO_PIN_13) == RESET;
}
