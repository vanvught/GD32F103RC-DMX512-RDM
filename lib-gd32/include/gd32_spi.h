/**
 * @file gd32_spi.h
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

#ifndef GD32_SPI_H_
#define GD32_SPI_H_

#include <gd32.h>

typedef enum GD32_SPI_BIT_ORDER {
	GD32_SPI_BIT_ORDER_LSBFIRST = 0,///< LSB First
	GD32_SPI_BIT_ORDER_MSBFIRST = 1	///< MSB First
} gd32_spi_bit_order_t;

typedef enum GD32_SPI_MODE {
	GD32_SPI_MODE0 = 0,				///< CPOL = 0, CPHA = 0
	GD32_SPI_MODE1 = 1,				///< CPOL = 0, CPHA = 1
	GD32_SPI_MODE2 = 2,				///< CPOL = 1, CPHA = 0
	GD32_SPI_MODE3 = 3				///< CPOL = 1, CPHA = 1
} gd32_spi_mode_t;

typedef enum GD32_SPI_CS {
	GD32_SPI_CS0 = 0,				///< Chip Select
	GD32_SPI_CS_NONE				///< No CS, control it yourself
} gd32_spi_chip_select_t;

void gd32_spi_begin();
void gd32_spi_end();

void gd32_spi_set_speed_hz(uint32_t nSpeedHz);
void gd32_spi_setDataMode(uint8_t nMode);
void gd32_spi_chipSelect(uint8_t nChipSelect);

void gd32_spi_transfernb(const char *pTxBuffer, char *pRxBuffer, uint32_t nDataLength);
void gd32_spi_transfern(char *pTxBuffer, uint32_t nDataLength);

void gd32_spi_write(const uint16_t nData);
void gd32_spi_writenb(const char *pTxBuffer, uint32_t nDataLength);

/*
 * DMA support
 */

const uint8_t *gd32_spi_dma_tx_prepare(uint32_t&  nLength);
void gd32_spi_dma_tx_start(const uint8_t *pTxBuffer, uint32_t nLength);
bool gd32_spi_dma_tx_is_active();

/**
 * SPI DMA implementation using I2S.
 * Limitation SPI is that you cannot set the speed exactly.
 * Exact speed is needed for pixels (WS2812B, etc).
 */
namespace i2s {
void gd32_spi_dma_begin();
void gd32_spi_dma_set_speed_hz(uint32_t nSpeedHz);
const uint8_t *gd32_spi_dma_tx_prepare(uint32_t *pLength);
void gd32_spi_dma_tx_start(const uint8_t *pTxBuffer, uint32_t nLength);
bool gd32_spi_dma_tx_is_active();
}  // namespace i2s

#endif /* GD32_SPI_H_ */
