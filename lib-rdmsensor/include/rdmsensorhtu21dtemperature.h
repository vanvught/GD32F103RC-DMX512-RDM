/**
 * @file rdmsensorhtu21dtemperature.h
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

#ifndef RDMSENSORHTU21DTEMPERATURE_H_
#define RDMSENSORHTU21DTEMPERATURE_H_

#include <cstdint>

#include "rdmsensor.h"
#include "htu21d.h"

#include "rdm_e120.h"

class RDMSensorHTU21DTemperature: public RDMSensor, sensor::HTU21D {
public:
	RDMSensorHTU21DTemperature(uint8_t nSensor, uint8_t nAddress = 0) : RDMSensor(nSensor), sensor::HTU21D(nAddress) {
		SetType(E120_SENS_TEMPERATURE);
		SetUnit(E120_UNITS_CENTIGRADE);
		SetPrefix(E120_PREFIX_NONE);
		SetRangeMin(rdm::sensor::safe_range_min(sensor::htu21d::temperature::RANGE_MIN));
		SetRangeMax(rdm::sensor::safe_range_max(sensor::htu21d::temperature::RANGE_MAX));
		SetNormalMin(rdm::sensor::safe_range_min(sensor::htu21d::temperature::RANGE_MIN));
		SetNormalMax(rdm::sensor::safe_range_max(sensor::htu21d::temperature::RANGE_MAX));
		SetDescription(sensor::htu21d::temperature::DESCRIPTION);
	}

	bool Initialize() override {
		return sensor::HTU21D::Initialize();
	}

	int16_t GetValue() override {
		return static_cast<int16_t>(sensor::HTU21D::GetTemperature(	));
	}
};

#endif /* RDMSENSORHTU21DTEMPERATURE_H_ */
