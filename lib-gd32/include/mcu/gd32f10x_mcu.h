/**
 * @file gd32f10x_mcu.h
 *
 */
/* Copyright (C) 2021-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef MCU_GD32F10X_MCU_H_
#define MCU_GD32F10X_MCU_H_

#if !(defined(GD32F10X_HD) || defined (GD32F10X_CL))
# error This file should not be included
#endif

#include <stdint.h>

#define MCU_CLOCK_FREQ		(uint32_t)(108000000)
#define AHB_CLOCK_FREQ		(uint32_t)(108000000)
#define APB1_CLOCK_FREQ		(uint32_t)(54000000)
#define APB2_CLOCK_FREQ		(uint32_t)(108000000)
#define TIMER_PSC_1MHZ		(uint16_t)(107)
#define TIMER_PSC_10KHZ		(uint16_t)(10799)

/**
 * U(S)ART
 */

#define USART0_RCU_USART0		RCU_USART0
#if defined (USART0_REMAP)
# define USART0_RCU_GPIOx		RCU_GPIOB
# define USART0_GPIOx			GPIOB
# define USART0_TX_GPIO_PINx	GPIO_PIN_6
# define USART0_RX_GPIO_PINx	GPIO_PIN_7
#else
# define USART0_RCU_GPIOx		RCU_GPIOA
# define USART0_GPIOx			GPIOA
# define USART0_TX_GPIO_PINx	GPIO_PIN_9
# define USART0_RX_GPIO_PINx	GPIO_PIN_10
#endif

#define USART1_RCU_USART1		RCU_USART1
#if defined (USART1_REMAP)
# define USART1_RCU_GPIOx		RCU_GPIOD
# define USART1_GPIOx			GPIOD
# define USART1_TX_GPIO_PINx	GPIO_PIN_5
# define USART1_RX_GPIO_PINx	GPIO_PIN_6
#else
# define USART1_RCU_GPIOx		RCU_GPIOA
# define USART1_GPIOx			GPIOA
# define USART1_TX_GPIO_PINx	GPIO_PIN_2
# define USART1_RX_GPIO_PINx	GPIO_PIN_3
#endif

#define USART2_RCU_USART2		RCU_USART2
#if defined (USART2_FULL_REMAP)
# define USART2_RCU_GPIOx		RCU_GPIOD
# define USART2_GPIOx			GPIOD
# define USART2_TX_GPIO_PINx	GPIO_PIN_8
# define USART2_RX_GPIO_PINx	GPIO_PIN_9
#elif defined (USART2_PARTIAL_REMAP)
# define USART2_RCU_GPIOx		RCU_GPIOC
# define USART2_GPIOx			GPIOC
# define USART2_TX_GPIO_PINx	GPIO_PIN_10
# define USART2_RX_GPIO_PINx	GPIO_PIN_11
#else
# define USART2_RCU_GPIOx		RCU_GPIOB
# define USART2_GPIOx			GPIOB
# define USART2_TX_GPIO_PINx	GPIO_PIN_10
# define USART2_RX_GPIO_PINx	GPIO_PIN_11
#endif

#define UART3_RCU_UART3			RCU_UART3
#define UART3_RCU_GPIOx			RCU_GPIOC
#define UART3_GPIOx				GPIOC
#define UART3_TX_GPIO_PINx		GPIO_PIN_10
#define UART3_RX_GPIO_PINx		GPIO_PIN_11

#define UART4_RCU_UART4			RCU_UART4
#define UART4_TX_RCU_GPIOx		RCU_GPIOC
#define UART4_TX_GPIOx			GPIOC
#define UART4_TX_GPIO_PINx		GPIO_PIN_12
#define UART4_RX_RCU_GPIOx		RCU_GPIOD
#define UART4_RX_GPIOx			GPIOD
#define UART4_RX_GPIO_PINx		GPIO_PIN_2

/**
 * I2C
 */

#define I2C0_PERIPH			I2C0
#define I2C0_RCU_I2C0		RCU_I2C0
#define I2C0_SCL_RCU_GPIOx	RCU_GPIOB
#define I2C0_SCL_GPIOx		GPIOB
#define I2C0_SDA_RCU_GPIOx	RCU_GPIOB
#define I2C0_SDA_GPIOx		GPIOB
#if defined (I2C0_REMAP)
# define I2C0_SCL_GPIO_PINx	GPIO_PIN_8
# define I2C0_SDA_GPIO_PINx	GPIO_PIN_9
#else
# define I2C0_SCL_GPIO_PINx	GPIO_PIN_6
# define I2C0_SDA_GPIO_PINx	GPIO_PIN_7
#endif

#define I2C1_PERIPH			I2C1
#define I2C1_RCU_I2C1		RCU_I2C1
#define I2C1_SCL_RCU_GPIOx	RCU_GPIOB
#define I2C1_SCL_GPIOx		GPIOB
#define I2C1_SCL_GPIO_PINx	GPIO_PIN_10
#define I2C1_SDA_RCU_GPIOx	RCU_GPIOB
#define I2C1_SDA_GPIOx		GPIOB
#define I2C1_SDA_GPIO_PINx	GPIO_PIN_11

/* There is no I2C1 remapping */

/**
 * SPI
 */

#define SPI0_PERIPH				SPI0
#define SPI0_RCU_SPI0			RCU_SPI0
#if defined (SPI0_REMAP)
# define SPI0_REMAP_GPIO		GPIO_SPI0_REMAP
# define SPI0_GPIOx				GPIOB
# define SPI0_RCU_GPIOx			RCU_GPIOB
# define SPI0_SCK_GPIO_PINx		GPIO_PIN_3
# define SPI0_MISO_GPIO_PINx	GPIO_PIN_4
# define SPI0_MOSI_GPIO_PINx	GPIO_PIN_5
# define SPI0_NSS_RCU_GPIOx		RCU_GPIOA
# define SPI0_NSS_GPIOx			GPIOA
# define SPI0_NSS_GPIO_PINx		GPIO_PIN_15
#else
# define SPI0_GPIOx				GPIOA
# define SPI0_RCU_GPIOx			RCU_GPIOA
# define SPI0_SCK_GPIO_PINx		GPIO_PIN_5
# define SPI0_MISO_GPIO_PINx	GPIO_PIN_6
# define SPI0_MOSI_GPIO_PINx	GPIO_PIN_7
# define SPI0_NSS_RCU_GPIOx		RCU_GPIOA
# define SPI0_NSS_GPIOx			GPIOA
# define SPI0_NSS_GPIO_PINx		GPIO_PIN_4
#endif

#define SPI1_PERIPH				SPI1
#define SPI1_RCU_SPI1			RCU_SPI1
#define SPI1_RCU_GPIOx			RCU_GPIOB
#define SPI1_GPIOx				GPIOB
#define SPI1_SCK_GPIO_PINx		GPIO_PIN_13
#define SPI1_MISO_GPIO_PINx		GPIO_PIN_14
#define SPI1_MOSI_GPIO_PINx		GPIO_PIN_15
#define SPI1_NSS_RCU_GPIOx		RCU_GPIOB
#define SPI1_NSS_GPIOx			GPIOB
#define SPI1_NSS_GPIO_PINx		GPIO_PIN_12

#define SPI2_PERIPH				SPI2
#define SPI2_RCU_SPI2			RCU_SPI2
#if defined (SPI2_REMAP)
# define SPI2_REMAP_GPIO		GPIO_SPI2_REMAP
# define SPI2_GPIOx				GPIOC
# define SPI2_RCU_GPIOx			RCU_GPIOC
# define SPI2_SCK_GPIO_PINx		GPIO_PIN_10
# define SPI2_MISO_GPIO_PINx	GPIO_PIN_11
# define SPI2_MOSI_GPIO_PINx	GPIO_PIN_12
# define SPI2_NSS_RCU_GPIOx		RCU_GPIOA
# define SPI2_NSS_GPIOx			GPIOA
# define SPI2_NSS_GPIO_PINx		GPIO_PIN_4
#else
# define SPI2_RCU_GPIOx			RCU_GPIOB
# define SPI2_GPIOx				GPIOB
# define SPI2_SCK_GPIO_PINx		GPIO_PIN_3
# define SPI2_MISO_GPIO_PINx	GPIO_PIN_4
# define SPI2_MOSI_GPIO_PINx	GPIO_PIN_5
# define SPI2_NSS_RCU_GPIOx		RCU_GPIOA
# define SPI2_NSS_GPIOx			GPIOA
# define SPI2_NSS_GPIO_PINx		GPIO_PIN_15
#endif

/**
 * TIMER GPIO
 */

#define TIMER2CH0_RCU_GPIOx	RCU_GPIOA
#define TIMER2CH0_GPIOx		GPIOA
#define TIMER2CH0_GPIO_PINx	GPIO_PIN_6

#if defined(TIMER2_FULL_REMAP)
# define TIMER2_REMAP 			GPIO_TIMER2_FULL_REMAP
# define TIMER2_CH0_RCU_GPIOx	RCU_GPIOC
# define TIMER2_CH0_GPIOx		GPIOC
# define TIMER2_CH0_GPIO_PINx	GPIO_PIN_6

# define TIMER2_CH1_RCU_GPIOx	RCU_GPIOC
# define TIMER2_CH1_GPIOx		GPIOC
# define TIMER2_CH1_GPIO_PINx	GPIO_PIN_7

# define TIMER2_CH2_RCU_GPIOx	RCU_GPIOC
# define TIMER2_CH2_GPIOx		GPIOC
# define TIMER2_CH2_GPIO_PINx	GPIO_PIN_8

# define TIMER2_CH3_RCU_GPIOx	RCU_GPIOC
# define TIMER2_CH3_GPIOx		GPIOC
# define TIMER2_CH3_GPIO_PINx	GPIO_PIN_9
#elif defined(TIMER2_PARTIAL_REMAP)
# define TIMER2_REMAP			GPIO_TIMER2_PARTIAL_REMAP
# define TIMER2_CH0_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH0_GPIOx		GPIOB
# define TIMER2_CH0_GPIO_PINx	GPIO_PIN_4

# define TIMER2_CH1_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH1_GPIOx		GPIOB
# define TIMER2_CH1_GPIO_PINx	GPIO_PIN_5

# define TIMER2_CH2_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH2_GPIOx		GPIOB
# define TIMER2_CH2_GPIO_PINx	GPIO_PIN_0

# define TIMER2_CH3_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH3_GPIOx		GPIOB
# define TIMER2_CH3_GPIO_PINx	GPIO_PIN_1
#else
# define TIMER2_CH0_RCU_GPIOx	RCU_GPIOA
# define TIMER2_CH0_GPIOx		GPIOA
# define TIMER2_CH0_GPIO_PINx	GPIO_PIN_6

# define TIMER2_CH1_RCU_GPIOx	RCU_GPIOA
# define TIMER2_CH1_GPIOx		GPIOA
# define TIMER2_CH1_GPIO_PINx	GPIO_PIN_7

# define TIMER2_CH2_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH2_GPIOx		GPIOB
# define TIMER2_CH2_GPIO_PINx	GPIO_PIN_0

# define TIMER2_CH3_RCU_GPIOx	RCU_GPIOB
# define TIMER2_CH3_GPIOx		GPIOB
# define TIMER2_CH3_GPIO_PINx	GPIO_PIN_1
#endif

/**
 * DMA
 */

#define SPI0_DMAx				DMA0
#define SPI0_TX_DMA_CHx			DMA_CH2

#define SPI1_DMAx				DMA0
#define SPI1_TX_DMA_CHx			DMA_CH4

#define SPI2_DMAx				DMA1
#define SPI2_TX_DMA_CHx			DMA_CH1

#define TIMER7_RCU_DMAx			RCU_DMA1
#define TIMER7_DMAx				DMA1
#define TIMER7_CH0_DMA_CHx		DMA_CH2
#define TIMER7_CH1_DMA_CHx		DMA_CH4
#define TIMER7_CH2_DMA_CHx		DMA_CH0

#define USART0_DMAx				DMA0
#define USART0_TX_DMA_CHx		DMA_CH3
#define USART0_RX_DMA_CHx		DMA_CH4

#define USART1_DMAx				DMA0
#define USART1_TX_DMA_CHx		DMA_CH6
#define USART1_RX_DMA_CHx		DMA_CH5

#define USART2_DMAx				DMA0
#define USART2_TX_DMA_CHx		DMA_CH1
#define USART2_RX_DMA_CHx		DMA_CH2

#define UART3_DMAx				DMA1
#define UART3_TX_DMA_CHx		DMA_CH4
#define UART3_RX_DMA_CHx		DMA_CH2

/* The USART supports DMA function for high-speed data communication, except UART4. */

#endif /* MCU_GD32F10X_MCU_H_ */
