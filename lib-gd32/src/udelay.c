/**
 * @file udelay.c
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

#include <stdint.h>
#include <assert.h>

#include "gd32.h"

#define TICKS_PER_US		(MCU_CLOCK_FREQ / 1000000U)

void udelay_init(void) {
	assert(MCU_CLOCK_FREQ == SystemCoreClock);

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void udelay(uint32_t us) {
	const uint32_t ticks = us * TICKS_PER_US;
	uint32_t tcnt = 0;
	uint32_t tnow;
	uint32_t told = DWT->CYCCNT;

	while (1) {
		tnow = DWT->CYCCNT;

		if (tnow != told) {
			if (tnow > told) {
				tcnt += tnow - told;
			} else {
				tcnt += UINT32_MAX - told + tnow;
			}

			if (tcnt >= ticks) {
				break;
			}

			told = tnow;
		}
	}
}
