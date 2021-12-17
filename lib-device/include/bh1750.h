/**
 * @file bh1750.h
 *
 */
/* Copyright (C) 2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef BH1750_H_
#define BH1750_H_

#include <cstdint>

#include "hal_i2c.h"

namespace sensor {
namespace bh1750 {
static constexpr char DESCRIPTION[] = "Ambient Light";
static constexpr auto RANGE_MIN = 0;
static constexpr auto RANGE_MAX = 65535;
}  // namespace bh1750

class BH170: HAL_I2C {
public:
	BH170(uint8_t nAddress = 0);

	bool Initialize() {
		return m_bIsInitialized;
	}

	uint16_t Get();

private:
	bool m_bIsInitialized = false;
};

}  // namespace sensor

#endif /* BH1750_H_ */
