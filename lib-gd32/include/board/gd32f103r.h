/**
 * @file gd32f103rc.h
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

#ifndef BOARD_GD32F103R_H_
#define BOARD_GD32F103R_H_

#if !defined(BOARD_GD32F103R)
# error This file should not be included
#endif

#if defined (MCU_GD32F103_H_)
# error This file should be included later
#endif

/**
 * LEDs
 */

#define LED_BLINK_PIN       GPIO_PIN_0
#define LED_BLINK_GPIO_PORT GPIOC
#define LED_BLINK_GPIO_CLK	RCU_GPIOC

#define LED1_PIN            GPIO_PIN_2
#define LED1_GPIO_PORT      GPIOC
#define LED1_GPIO_CLK       RCU_GPIOC

#define LED2_PIN            GPIO_PIN_0
#define LED2_GPIO_PORT      GPIOE
#define LED2_GPIO_CLK       RCU_GPIOE

#define LED3_PIN            GPIO_PIN_1
#define LED3_GPIO_PORT      GPIOE
#define LED3_GPIO_CLK       RCU_GPIOE

/**
 * KEYs
 */

#define KEY1_PINx			GPIO_PIN_6
#define KEY1_GPIOx			GPIOA
#define KEY1_RCU_GPIOx		RCU_GPIOA

#define KEY2_PINx			GPIO_PIN_14
#define KEY2_GPIOx			GPIOB
#define KEY2_RCU_GPIOx		RCU_GPIOB

#define KEY3_PINx			GPIO_PIN_11
#define KEY3_GPIOx			GPIOA
#define KEY3_RCU_GPIOx		RCU_GPIOA

/**
 * I2C
 */

#define I2C_PERIPH			I2C0_PERIPH
#define I2C_RCU_CLK			I2C0_RCU_CLK
#define I2C_GPIO_SCL_PORT	I2C0_SCL_GPIOx
#define I2C_GPIO_SCL_CLK	I2C0_SCL_RCU_GPIOx
#define I2C_GPIO_SDA_PORT	I2C0_SDA_GPIOx
#define I2C_GPIO_SDA_CLK	I2C0_SDA_RCU_GPIOx
#define I2C_SCL_PIN			I2C0_SCL_GPIO_PINx
#define I2C_SDA_PIN			I2C0_SDA_GPIO_PINx

/**
 * SPI
 */

// #define SPI2_REMAP
#if defined (SPI2_REMAP)
# define SPI_REMAP			SPI2_REMAP_GPIO
#endif
#define SPI_PERIPH			SPI2_PERIPH
#define SPI_NSS_GPIOx		SPI2_NSS_GPIOx
#define SPI_NSS_RCU_GPIOx	SPI2_NSS_RCU_GPIOx
#define SPI_NSS_GPIO_PINx	SPI2_NSS_GPIO_PINx
#define SPI_RCU_CLK			SPI2_RCU_CLK
#define SPI_GPIOx			SPI2_GPIOx
#define SPI_RCU_GPIOx		SPI2_RCU_GPIOx
#define SPI_SCK_PIN			SPI2_SCK_GPIO_PINx
#define SPI_MISO_PIN		SPI2_MISO_GPIO_PINx
#define SPI_MOSI_PIN		SPI2_MOSI_GPIO_PINx
#define SPI_DMAx			SPI2_DMAx
#define SPI_DMA_CHx			SPI2_TX_DMA_CHx

/**
 * U(S)ART
 */

// #define USART0_REMAP
// #define USART1_REMAP
#define USART2_PARTIAL_REMAP
// #define UART3_REMAP

#include "mcu/gd32f103_mcu.h"
#include "gd32_gpio.h"

#define GD32_BOARD_NAME			"GD32F103R"
#define GD32_BOARD_LED1			GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 0)
#define GD32_BOARD_LED2			GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 2)
#define GD32_BOARD_LED3			GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 3)
#define GD32_BOARD_STATUS_LED	GD32_BOARD_LED1

#include "gpio_header.h"

#endif /* BOARD_GD32F103R_H_ */
