/**
 * @file mac_address.c
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

extern int uart0_printf(const char* fmt, ...);

void mac_address_get(uint8_t paddr[]) {
	const uint32_t mac_lo = *(volatile uint32_t *) (0x1FFFF7EC);
	const uint32_t mac_hi = *(volatile uint32_t *) (0x1FFFF7F0);

	paddr[0] = 2;
	paddr[1] = (mac_lo >> 8) & 0xff;
	paddr[2] = (mac_lo >> 16) & 0xff;
	paddr[3] = (mac_lo >> 24) & 0xff;
	paddr[4] = (mac_hi >> 0) & 0xff;
	paddr[5] = (mac_hi >> 8) & 0xff;

#ifndef NDEBUG
	uart0_printf("%02x:%02x:%02x:%02x:%02x:%02x\n", paddr[0], paddr[1], paddr[2], paddr[3], paddr[4], paddr[5]);
#endif
}
