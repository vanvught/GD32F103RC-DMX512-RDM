/**
 * @file displayudfparams.cpp
 *
 */
/* Copyright (C) 2019-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#if !defined(__clang__)	// Needed for compiling on MacOS
# pragma GCC push_options
# pragma GCC optimize ("Os")
#endif

#include <cstdint>
#include <cstring>
#ifndef NDEBUG
# include <cstdio>
#endif
#include <cassert>

#if defined (NODE_ARTNET_MULTI)
# define NODE_ARTNET
#endif

#if defined (NODE_E131_MULTI)
# define NODE_E131
#endif

#include "displayudfparams.h"
#include "displayudfparamsconst.h"

#include "networkparamsconst.h"
#include "lightsetparamsconst.h"

#include "readconfigfile.h"
#include "sscan.h"

#include "propertiesbuilder.h"

#include "display.h"

#if defined (NODE_ARTNET)
# include "artnetnode.h"
# include "artnetparamsconst.h"
#endif
#if defined (NODE_E131)
# include "e131bridge.h"
#endif

using namespace displayudf;

static constexpr const char *pArray[static_cast<uint32_t>(Labels::UNKNOWN)] = {
		DisplayUdfParamsConst::TITLE,
		DisplayUdfParamsConst::BOARD_NAME,
		NetworkParamsConst::IP_ADDRESS,
		DisplayUdfParamsConst::VERSION,
		LightSetParamsConst::UNIVERSE,
		DisplayUdfParamsConst::ACTIVE_PORTS,
#if defined (NODE_ARTNET)
		ArtNetParamsConst::NODE_SHORT_NAME,
#else
		"",
#endif
		NetworkParamsConst::HOSTNAME,
		LightSetParamsConst::UNIVERSE_PORT[0],
		LightSetParamsConst::UNIVERSE_PORT[1],
		LightSetParamsConst::UNIVERSE_PORT[2],
		LightSetParamsConst::UNIVERSE_PORT[3],
		NetworkParamsConst::NET_MASK,
		LightSetParamsConst::DMX_START_ADDRESS,
#if defined (NODE_ARTNET)
		ArtNetParamsConst::DESTINATION_IP_PORT[0],
		ArtNetParamsConst::DESTINATION_IP_PORT[1],
		ArtNetParamsConst::DESTINATION_IP_PORT[2],
		ArtNetParamsConst::DESTINATION_IP_PORT[3],
#else
		"",
		"",
		"",
		"",
#endif
		NetworkParamsConst::DEFAULT_GATEWAY,
		DisplayUdfParamsConst::DMX_DIRECTION
};

DisplayUdfParams::DisplayUdfParams(DisplayUdfParamsStore *pDisplayUdfParamsStore): m_pDisplayUdfParamsStore(pDisplayUdfParamsStore) {
	memset(&m_tDisplayUdfParams, 0, sizeof(struct TDisplayUdfParams));
	m_tDisplayUdfParams.nSleepTimeout = display::Defaults::SEEP_TIMEOUT;
	m_tDisplayUdfParams.nIntensity = defaults::INTENSITY;
}

bool DisplayUdfParams::Load() {
	m_tDisplayUdfParams.nSetList = 0;

#if !defined(DISABLE_FS)
	ReadConfigFile configfile(DisplayUdfParams::staticCallbackFunction, this);

	if (configfile.Read(DisplayUdfParamsConst::FILE_NAME)) {
		if (m_pDisplayUdfParamsStore != nullptr) {
			m_pDisplayUdfParamsStore->Update(&m_tDisplayUdfParams);
		}
	} else
#endif
	if (m_pDisplayUdfParamsStore != nullptr) {
		m_pDisplayUdfParamsStore->Copy(&m_tDisplayUdfParams);
	} else {
		return false;
	}

	return true;
}

void DisplayUdfParams::Load(const char *pBuffer, uint32_t nLength) {
	assert(pBuffer != nullptr);
	assert(nLength != 0);

	assert(m_pDisplayUdfParamsStore != nullptr);

	if (m_pDisplayUdfParamsStore == nullptr) {
		return;
	}

	m_tDisplayUdfParams.nSetList = 0;

	ReadConfigFile config(DisplayUdfParams::staticCallbackFunction, this);

	config.Read(pBuffer, nLength);

	m_pDisplayUdfParamsStore->Update(&m_tDisplayUdfParams);
}

void DisplayUdfParams::callbackFunction(const char *pLine) {
	assert(pLine != nullptr);
	uint8_t value8;

	if (Sscan::Uint8(pLine, DisplayUdfParamsConst::INTENSITY, value8) == Sscan::OK) {
		m_tDisplayUdfParams.nIntensity = value8;

		if (value8 != defaults::INTENSITY) {
			m_tDisplayUdfParams.nSetList |= DisplayUdfParamsMask::INTENSITY;
		} else {
			m_tDisplayUdfParams.nSetList &= ~DisplayUdfParamsMask::INTENSITY;
		}
		return;
	}

	if (Sscan::Uint8(pLine, DisplayUdfParamsConst::SLEEP_TIMEOUT, value8) == Sscan::OK) {
		m_tDisplayUdfParams.nSleepTimeout = value8;

		if (value8 != display::Defaults::SEEP_TIMEOUT) {
			m_tDisplayUdfParams.nSetList |= DisplayUdfParamsMask::SLEEP_TIMEOUT;
		} else {
			m_tDisplayUdfParams.nSetList &= ~DisplayUdfParamsMask::SLEEP_TIMEOUT;
		}
		return;
	}

	if (Sscan::Uint8(pLine, DisplayUdfParamsConst::FLIP_VERTICALLY, value8) == Sscan::OK) {
		if (value8 != 0) {
			m_tDisplayUdfParams.nSetList |= DisplayUdfParamsMask::FLIP_VERTICALLY;
		} else {
			m_tDisplayUdfParams.nSetList &= ~DisplayUdfParamsMask::FLIP_VERTICALLY;
		}
		return;
	}

	for (uint32_t i = 0; i < static_cast<uint32_t>(Labels::UNKNOWN); i++) {
		if (Sscan::Uint8(pLine, pArray[i], value8) == Sscan::OK) {
			if ((value8 > 0) && (value8 <= LABEL_MAX_ROWS)) {
				m_tDisplayUdfParams.nLabelIndex[i] = value8;
				m_tDisplayUdfParams.nSetList |= (1U << i);
			} else {
				m_tDisplayUdfParams.nLabelIndex[i] = 0;
				m_tDisplayUdfParams.nSetList &= ~(1U << i);
			}
			return;
		}
	}
}

void DisplayUdfParams::Builder(const struct TDisplayUdfParams *ptDisplayUdfParams, char *pBuffer, uint32_t nLength, uint32_t& nSize) {
	assert(pBuffer != nullptr);

	if (ptDisplayUdfParams != nullptr) {
		memcpy(&m_tDisplayUdfParams, ptDisplayUdfParams, sizeof(struct TDisplayUdfParams));
	} else {
		m_pDisplayUdfParamsStore->Copy(&m_tDisplayUdfParams);
	}

	PropertiesBuilder builder(DisplayUdfParamsConst::FILE_NAME, pBuffer, nLength);

	builder.Add(DisplayUdfParamsConst::INTENSITY, m_tDisplayUdfParams.nIntensity , isMaskSet(DisplayUdfParamsMask::INTENSITY));
	builder.Add(DisplayUdfParamsConst::SLEEP_TIMEOUT, m_tDisplayUdfParams.nSleepTimeout , isMaskSet(DisplayUdfParamsMask::SLEEP_TIMEOUT));
	builder.Add(DisplayUdfParamsConst::FLIP_VERTICALLY, isMaskSet(DisplayUdfParamsMask::FLIP_VERTICALLY) , isMaskSet(DisplayUdfParamsMask::FLIP_VERTICALLY));

	for (uint32_t i = 0; i < static_cast<uint32_t>(Labels::UNKNOWN); i++) {
		if (pArray[i][0] != '\0') {
			builder.Add(pArray[i], m_tDisplayUdfParams.nLabelIndex[i] , isMaskSet(1U << i));
		}
	}

	nSize = builder.GetSize();
}

void DisplayUdfParams::Save(char *pBuffer, uint32_t nLength, uint32_t& nSize) {
	if (m_pDisplayUdfParamsStore == nullptr) {
		nSize = 0;
		return;
	}

	Builder(nullptr, pBuffer, nLength, nSize);
}

void DisplayUdfParams::Set(DisplayUdf *pDisplayUdf) {
	assert(pDisplayUdf != nullptr);

	if (isMaskSet(DisplayUdfParamsMask::INTENSITY)) {
		pDisplayUdf->SetContrast(m_tDisplayUdfParams.nIntensity);
	}

	if (isMaskSet(DisplayUdfParamsMask::SLEEP_TIMEOUT)) {
		pDisplayUdf->SetSleepTimeout(m_tDisplayUdfParams.nSleepTimeout);
	}

	pDisplayUdf->SetFlipVertically(isMaskSet(DisplayUdfParamsMask::FLIP_VERTICALLY));

	for (uint32_t i = 0; i < static_cast<uint32_t>(Labels::UNKNOWN); i++) {
		if (isMaskSet(1U << i)) {
			pDisplayUdf->Set(m_tDisplayUdfParams.nLabelIndex[i], static_cast<Labels>(i));
		}
	}
}

void DisplayUdfParams::staticCallbackFunction(void *p, const char *s) {
	assert(p != nullptr);
	assert(s != nullptr);

	(static_cast<DisplayUdfParams*>(p))->callbackFunction(s);
}

void DisplayUdfParams::Dump() {
#ifndef NDEBUG
	printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, DisplayUdfParamsConst::FILE_NAME);

	if (isMaskSet(DisplayUdfParamsMask::INTENSITY)) {
		printf(" %s=%d\n", DisplayUdfParamsConst::INTENSITY, m_tDisplayUdfParams.nIntensity);
	}

	if (isMaskSet(DisplayUdfParamsMask::SLEEP_TIMEOUT)) {
		printf(" %s=%d\n", DisplayUdfParamsConst::SLEEP_TIMEOUT, m_tDisplayUdfParams.nSleepTimeout);
	}

	if (isMaskSet(DisplayUdfParamsMask::FLIP_VERTICALLY)) {
		printf(" Flip vertically\n");
	}

	for (uint32_t i = 0; i < static_cast<uint32_t>(Labels::UNKNOWN); i++) {
		if (isMaskSet(1U << i)) {
			printf(" %s=%d\n", pArray[i], m_tDisplayUdfParams.nLabelIndex[i]);
		}
	}
#endif
}
