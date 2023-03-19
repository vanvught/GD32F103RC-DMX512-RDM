/**
 * @file mcp3424.cpp
 *
 */
/* Copyright (C) 2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

//TODO Implement Gain for GetVoltage

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "mcp3424.h"
#include "hal_i2c.h"

#include "debug.h"

namespace adc {
namespace mcp3424 {
static constexpr uint8_t I2C_ADDRESS = 0x68;

static constexpr uint8_t GAIN(const Gain gain) {
	return static_cast<uint8_t>(gain) & 0x03;
}

static constexpr uint8_t RESOLUTION(const Resolution resolution) {
	return (static_cast<uint8_t>(resolution) & 0x03) << 2;
}

static constexpr uint8_t CONVERSION(const Conversion conversion) {
	return (static_cast<uint8_t>(conversion) & 0x01) << 4;
}

static constexpr uint8_t CHANNEL(const uint8_t nChannel) {
	return (nChannel & 0x03) << 5;
}
}  // namespace mcp3424
}  // namespace adc

MCP3424::MCP3424(uint8_t nAddress) : HAL_I2C(nAddress == 0  ? adc::mcp3424::I2C_ADDRESS : nAddress) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nAddress=%x", nAddress);

	SetGain(adc::mcp3424::Gain::PGA_X1);
	SetResolution(adc::mcp3424::Resolution::SAMPLE_12BITS);
	SetConversion(adc::mcp3424::Conversion::CONTINUOUS);

	m_IsConnected = HAL_I2C::IsConnected();

	DEBUG_PRINTF("m_IsConnected=%u", m_IsConnected);
	DEBUG_EXIT
}

void MCP3424::SetGain(const adc::mcp3424::Gain gain) {
	m_nConfig &= static_cast<uint8_t>(~0x03);
	m_nConfig |= adc::mcp3424::GAIN(gain);
}
adc::mcp3424::Gain MCP3424::GetGain() const {
	return static_cast<adc::mcp3424::Gain>(m_nConfig & 0x3);
}

void MCP3424::SetResolution(const adc::mcp3424::Resolution resolution) {
	m_nConfig &= static_cast<uint8_t>(~((0x03) << 2));
	m_nConfig |= adc::mcp3424::RESOLUTION(resolution);

	switch (resolution) {
	case adc::mcp3424::Resolution::SAMPLE_12BITS:
		m_lsb = 0.0005;
		break;
	case adc::mcp3424::Resolution::SAMPLE_14BITS:
		m_lsb = 0.000125;
		break;
	case adc::mcp3424::Resolution::SAMPLE_16BITS:
		m_lsb = 0.00003125;
		break;
	case adc::mcp3424::Resolution::SAMPLE_18BITS:
		m_lsb = 0.0000078125;
		break;
	default:
		assert(0);
		__builtin_unreachable();
		break;
	}
}
adc::mcp3424::Resolution MCP3424::GetResolution() const {
	return static_cast<adc::mcp3424::Resolution>((m_nConfig >> 2) & 0x03);
}

void MCP3424::SetConversion(const adc::mcp3424::Conversion conversion) {
	m_nConfig &= static_cast<uint8_t>(~((0x01) << 4));
	m_nConfig |= adc::mcp3424::CONVERSION(conversion);
}

adc::mcp3424::Conversion MCP3424::GetConversion() const {
	return static_cast<adc::mcp3424::Conversion>((m_nConfig >> 4) & 0x01);
}

uint32_t MCP3424::GetRaw(const uint8_t nChannel) {
	m_nConfig &= static_cast<uint8_t>(~((0x03) << 5));
	m_nConfig |= adc::mcp3424::CHANNEL(nChannel);

	char buffer[4];
	int32_t nTimeout = 8000;
	uint32_t nBytes = 3;

	if ((m_nConfig & RESOLUTION(adc::mcp3424::Resolution::SAMPLE_18BITS)) == RESOLUTION(adc::mcp3424::Resolution::SAMPLE_18BITS)) {
		nBytes = 4;
	}

	while (true) {
		HAL_I2C::Write(static_cast<char>(m_nConfig));
		HAL_I2C::Read(buffer, nBytes);

		if (nBytes == 4) {
			if ((buffer[3] >> 7) == 0) {
				break;
			}
		} else {
			if ((buffer[2] >> 7) == 0) {
				break;
			}
		}

		if (nTimeout-- == 0) {
			return static_cast<uint32_t>(~0);
		}
	}

	switch (static_cast<adc::mcp3424::Resolution>((m_nConfig >> 2) & 0x03)) {
		case adc::mcp3424::Resolution::SAMPLE_12BITS:
			return static_cast<uint32_t>(((buffer[0] & 0x0f) << 8) | buffer[1]);
			break;
		case adc::mcp3424::Resolution::SAMPLE_14BITS:
			return static_cast<uint32_t>(((buffer[0] & 0x3f) << 8) | buffer[1]);
			break;
		case adc::mcp3424::Resolution::SAMPLE_16BITS:
			return static_cast<uint32_t>((buffer[0] << 8) | buffer[1]);
			break;
		case adc::mcp3424::Resolution::SAMPLE_18BITS:
			return static_cast<uint32_t>(((buffer[0] & 0x03) << 16) | (buffer[1] << 8) | buffer[2]);
			break;
		default:
			assert(0);
			__builtin_unreachable();
			break;
	}

	__builtin_unreachable();
	return static_cast<uint32_t>(~0);
}

double MCP3424::GetVoltage(const uint8_t nChannel) {
	const auto Vout = static_cast<double>(GetRaw(nChannel)) * 2 * m_lsb;
	return Vout;
}
