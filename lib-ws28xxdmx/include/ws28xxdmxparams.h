/**
 * @file ws28xxdmxparams.h
 *
 */
/* Copyright (C) 2017-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"); to deal
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

#ifndef WS28XXDMXPARAMS_H_
#define WS28XXDMXPARAMS_H_

#include <cstdint>

#include "ws28xx.h"
#include "ws28xxdmx.h"

#include "pixeldmxconfiguration.h"

namespace ws28xxdmxparams {
	static constexpr auto MAX_OUTPUTS = 8;
}  // ws28xxdmxparams name

struct TWS28xxDmxParams {
    uint32_t nSetList;										///< 4	   4
	uint8_t nType;											///< 1	   5
	uint16_t nCount;										///< 2	   7
	uint16_t nDmxStartAddress;								///< 2	   9
	uint8_t NotUsed0;										///< 1	  10
	uint32_t nSpiSpeedHz;									///< 4	  14
	uint8_t nGlobalBrightness;								///< 1	  15
	uint8_t nActiveOutputs;									///< 1	  16
	uint8_t NotUsed1;										///< 1	  17
	uint16_t nGroupingCount;								///< 2	  19
	uint8_t nMap;											///< 1	  20
	uint8_t nLowCode;										///< 1	  21
	uint8_t nHighCode;										///< 1	  22
	uint16_t nStartUniverse[ws28xxdmxparams::MAX_OUTPUTS];	///< 16   38
	uint8_t nTestPattern;									///< 1    39
}__attribute__((packed));

static_assert(sizeof(struct TWS28xxDmxParams) <= 64, "struct TWS28xxDmxParams is too large");

struct WS28xxDmxParamsMask {
	static constexpr auto TYPE = (1U << 0);
	static constexpr auto COUNT = (1U << 1);
	static constexpr auto DMX_START_ADDRESS = (1U << 2);
	static constexpr auto _NOT_USED_ = (1U << 3);
	static constexpr auto SPI_SPEED = (1U << 4);
	static constexpr auto GLOBAL_BRIGHTNESS = (1U << 5);
	static constexpr auto ACTIVE_OUT = (1U << 6);
	static constexpr auto USE_SI5351A = (1U << 7);
	static constexpr auto GROUPING_COUNT = (1U << 8);
	static constexpr auto MAP = (1U << 9);
	static constexpr auto LOW_CODE = (1U << 10);
	static constexpr auto HIGH_CODE = (1U << 11);
	static constexpr auto START_UNI_PORT_1 = (1U << 12);
	static constexpr auto START_UNI_PORT_2 = (1U << 13);
	static constexpr auto START_UNI_PORT_3 = (1U << 14);
	static constexpr auto START_UNI_PORT_4 = (1U << 15);
	static constexpr auto START_UNI_PORT_5 = (1U << 16);
	static constexpr auto START_UNI_PORT_6 = (1U << 17);
	static constexpr auto START_UNI_PORT_7 = (1U << 18);
	static constexpr auto START_UNI_PORT_8 = (1U << 19);
	static constexpr auto TEST_PATTERN = (1U << 20);
};

class WS28xxDmxParamsStore {
public:
	virtual ~WS28xxDmxParamsStore() {
	}

	virtual void Update(const struct TWS28xxDmxParams *pWS28xxDmxParams)=0;
	virtual void Copy(struct TWS28xxDmxParams *pWS28xxDmxParams)=0;
};

class WS28xxDmxParams {
public:
	WS28xxDmxParams(WS28xxDmxParamsStore *pWS28xxParamsStore = nullptr);
	~WS28xxDmxParams() {
	}

	bool Load();
	void Load(const char *pBuffer, uint32_t nLength);

	void Builder(const struct TWS28xxDmxParams *ptWS28xxParams, char *pBuffer, uint32_t nLength, uint32_t& nSize);
	void Save(char *pBuffer, uint32_t nLength, uint32_t& nSize);

	void Set(PixelDmxConfiguration *pPixelDmxConfiguration);

	void Dump();

	uint16_t GetStartUniversePort(uint32_t nOutputPortIndex, bool& isSet) const {
		if (nOutputPortIndex < ws28xxdmxparams::MAX_OUTPUTS) {
			isSet = isMaskSet(WS28xxDmxParamsMask::START_UNI_PORT_1 << nOutputPortIndex);
			return m_tWS28xxParams.nStartUniverse[nOutputPortIndex];
		}

		isSet = false;
		return 0;
	}

	uint8_t GetTestPattern() const {
		return m_tWS28xxParams.nTestPattern;
	}

	static void staticCallbackFunction(void *p, const char *s);

private:
    void callbackFunction(const char *pLine);
    bool isMaskSet(uint32_t nMask) const {
    	return (m_tWS28xxParams.nSetList & nMask) == nMask;
    }

private:
    WS28xxDmxParamsStore *m_pWS28xxParamsStore;
    struct TWS28xxDmxParams m_tWS28xxParams;
};

#endif /* WS28XXDMXPARAMS_H_ */
