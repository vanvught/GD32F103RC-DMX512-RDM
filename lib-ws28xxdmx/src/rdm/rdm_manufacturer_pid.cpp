/**
 * @file rdm_manufacturer_pid.cpp
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

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>

#include "rdm_manufacturer_pid.h"
#include "rdmhandler.h"
#include "rdm_e120.h"

#include "pixeltype.h"

#include "debug.h"

#if !defined(OUTPUT_DMX_PIXEL)
# error
# endif

namespace rdm {
using E120_MANUFACTURER_PIXEL_TYPE = ManufacturerPid<0x8500>;
using E120_MANUFACTURER_PIXEL_COUNT = ManufacturerPid<0x8501>;
using E120_MANUFACTURER_PIXEL_GROUPING_COUNT = ManufacturerPid<0x8502>;
using E120_MANUFACTURER_PIXEL_MAP = ManufacturerPid<0x8503>;

struct PixelType {
    static constexpr char description[] = "Pixel type";
};

struct PixelCount {
    static constexpr char description[] = "Pixel count";
};

struct PixelGroupingCount {
    static constexpr char description[] = "Pixel grouping count";
};

struct PixelMap {
    static constexpr char description[] = "Pixel map";
};

constexpr char PixelType::description[];
constexpr char PixelCount::description[];
constexpr char PixelGroupingCount::description[];
constexpr char PixelMap::description[];
}  // namespace rdm

const rdm::ParameterDescription RDMHandler::PARAMETER_DESCRIPTIONS[] = {
		  { rdm::E120_MANUFACTURER_PIXEL_TYPE::code,
		    rdm::DEVICE_DESCRIPTION_MAX_LENGTH,
			E120_DS_ASCII,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			0,
			0,
			rdm::Description<rdm::PixelType, sizeof(rdm::PixelType::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelType::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_COUNT::code,
			2,
			E120_DS_UNSIGNED_DWORD,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			__builtin_bswap32(pixel::defaults::COUNT),
			__builtin_bswap32(pixel::max::ledcount::RGB),
			rdm::Description<rdm::PixelCount, sizeof(rdm::PixelCount::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelCount::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_GROUPING_COUNT::code,
			2,
			E120_DS_UNSIGNED_DWORD,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			__builtin_bswap32(pixel::defaults::COUNT),
			__builtin_bswap32(pixel::max::ledcount::RGB),
			rdm::Description<rdm::PixelGroupingCount, sizeof(rdm::PixelGroupingCount::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelGroupingCount::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_MAP::code,
			rdm::DEVICE_DESCRIPTION_MAX_LENGTH,
			E120_DS_ASCII,
			E120_CC_GET,
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			0,
			0,
			rdm::Description<rdm::PixelMap, sizeof(rdm::PixelMap::description)>::value,
			rdm::pdlParameterDescription(sizeof(rdm::PixelMap::description))
		  }
  };

uint32_t RDMHandler::GetParameterDescriptionCount() const {
	return sizeof(RDMHandler::PARAMETER_DESCRIPTIONS) / sizeof(RDMHandler::PARAMETER_DESCRIPTIONS[0]);
}

#include "ws28xxdmx.h"

namespace rdm {
bool handle_manufactureer_pid_get(const uint16_t nPid, __attribute__((unused)) const ManufacturerParamData *pIn, ManufacturerParamData *pOut, uint16_t& nReason) {
	switch (nPid) {
	case rdm::E120_MANUFACTURER_PIXEL_TYPE::code: {
		const auto *pString = ::PixelType::GetType(WS28xxDmx::Get()->GetType());
		pOut->nPdl = static_cast<uint8_t>(strlen(pString));
		memcpy(pOut->pParamData, pString, pOut->nPdl);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_COUNT::code: {
		const auto nCount = WS28xxDmx::Get()->GetCount();
		pOut->nPdl = 4;
		pOut->pParamData[0] = static_cast<uint8_t>(nCount >> 24);
		pOut->pParamData[1] = static_cast<uint8_t>(nCount >> 16);
		pOut->pParamData[2] = static_cast<uint8_t>(nCount >> 8);
		pOut->pParamData[3] = static_cast<uint8_t>(nCount);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_GROUPING_COUNT::code: {
		const auto nGroupingCount = WS28xxDmx::Get()->GetGroupingCount();
		pOut->nPdl = 4;
		pOut->pParamData[0] = static_cast<uint8_t>(nGroupingCount >> 24);
		pOut->pParamData[1] = static_cast<uint8_t>(nGroupingCount >> 16);
		pOut->pParamData[2] = static_cast<uint8_t>(nGroupingCount >> 8);
		pOut->pParamData[3] = static_cast<uint8_t>(nGroupingCount);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_MAP::code: {
		const auto *pString = ::PixelType::GetMap(WS28xxDmx::Get()->GetMap());
		pOut->nPdl = static_cast<uint8_t>(strlen(pString));
		memcpy(pOut->pParamData, pString, pOut->nPdl);
		return true;
	}
	default:
		break;
	}

	nReason = E120_NR_UNKNOWN_PID;
	return false;
}
}  // namespace rdm
