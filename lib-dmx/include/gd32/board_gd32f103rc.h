/**
 * @file board_gd32f103rc.h
 *
 */
/* Copyright (C) 2021-2022 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef GD32_BOARD_GD32F103RC_H_
#define GD32_BOARD_GD32F103RC_H_

#include "gd32_board.h"

namespace max {
static constexpr auto OUT = 1U;
static constexpr auto IN = 1U;
}

#define DMX_MAX_PORTS  1

#define DMX_USE_USART2
static constexpr auto USART2_PORT = 0;

static constexpr auto DIR_PORT_0_GPIO_PORT = GPIOB;
static constexpr auto DIR_PORT_0_GPIO_PIN = GPIO_PIN_10;

#endif /* GD32_BOARD_GD32F103RC_H_ */
