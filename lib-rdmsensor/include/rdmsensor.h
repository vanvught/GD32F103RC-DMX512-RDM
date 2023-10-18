/**
 * @file rdmsensor.h
 *
 */
/* Copyright (C) 2018-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef RDMSENSOR_H_
#define RDMSENSOR_H_

#include <cstdint>

namespace rdm {
namespace sensor {
struct Defintion {
	uint8_t sensor;
	uint8_t type;
	uint8_t unit;
	uint8_t prefix;
	int16_t range_min;
	int16_t range_max;
	int16_t normal_min;
	int16_t normal_max;
	char description[32];
	uint8_t nLength;
	uint8_t recorded_supported;
};

struct Values {
	int16_t present;
	int16_t lowest_detected;
	int16_t highest_detected;
	int16_t recorded;
	uint8_t sensor_requested;
};
static constexpr int16_t RANGE_MIN	= -32768;
static constexpr int16_t RANGE_MAX	= +32767;
static constexpr int16_t NORMAL_MIN	= -32768;
static constexpr int16_t NORMAL_MAX = +32767;
static constexpr int16_t TEMPERATURE_ABS_ZERO	=	-273;

template<class T>
constexpr int16_t safe_range_max(const T& a)
{
    static_assert(sizeof(int16_t) <= sizeof(T), "T");

    return (a > static_cast<T>(INT16_MAX)) ? INT16_MAX : static_cast<int16_t>(a);
}

template<class T>
constexpr int16_t safe_range_min(const T& a)
{
    static_assert(sizeof(int16_t) <= sizeof(T), "T");

    return (a < static_cast<T>(INT16_MIN)) ? INT16_MIN : static_cast<int16_t>(a);
}

}  // namespace rdm
}  // namespace sensor


class RDMSensor {
public:
	RDMSensor(uint8_t nSensor);
	virtual ~RDMSensor() {}

public:
	void SetType(uint8_t nType) {
		m_tRDMSensorDefintion.type = nType;
	}

	void SetUnit(uint8_t nUnit) {
		m_tRDMSensorDefintion.unit = nUnit;
	}

	void SetPrefix(uint8_t nPrefix) {
		m_tRDMSensorDefintion.prefix = nPrefix;
	}

	void SetRangeMin(int16_t nRangeMin) {
		m_tRDMSensorDefintion.range_min = nRangeMin;
	}

	void SetRangeMax(int16_t nRangeMax) {
		m_tRDMSensorDefintion.range_max = nRangeMax;
	}

	void SetNormalMin(int16_t nNormalMin) {
		m_tRDMSensorDefintion.normal_min = nNormalMin;
	}

	void SetNormalMax(int16_t nNormalMax) {
		m_tRDMSensorDefintion.normal_max = nNormalMax;
	}

	void SetDescription(const char *pDescription);

	void Print();

	uint8_t GetSensor() const {
		return m_nSensor;
	}

	const struct rdm::sensor::Defintion* GetDefintion() {
		return &m_tRDMSensorDefintion;
	}

	const struct rdm::sensor::Values* GetValues();

	void SetValues();

	void Record();

	virtual bool Initialize()=0;
	virtual int16_t GetValue()=0;

private:
	uint8_t m_nSensor;
	rdm::sensor::Defintion m_tRDMSensorDefintion;
	rdm::sensor::Values m_tRDMSensorValues;
};

#endif /* RDMSENSOR_H_ */
