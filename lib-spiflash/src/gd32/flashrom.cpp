/**
 * @file flashrom.cpp
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <stdio.h>
#include <cassert>

#include "flashrom.h"

#include "gd32.h"

#include "debug.h"

namespace flashrom {
static constexpr auto FLASH_SECTOR_SIZE = 4096U;
/* The flash page size is 2KB for bank0 */
static constexpr auto FLASH_PAGE = (2 * 1024);

enum class State {
	IDLE,
	ERASE_BUSY,
	ERASE_PROGAM,
	WRITE_BUSY,
	WRITE_PROGRAM,
	ERROR
};

static State s_State = State::IDLE;
static uint32_t s_nPage;
static uint32_t s_nLength;;
static uint32_t s_nAddress;
static uint32_t *s_pData;
}  // namespace flashrom

using namespace flashrom;

using namespace flashrom;

FlashRom *FlashRom::s_pThis;

FlashRom::FlashRom() {
	DEBUG_ENTRY
	assert(s_pThis == nullptr);
	s_pThis = this;

	m_IsDetected = true;

	printf("Detected %s with sector size %d total %d bytes\n", GetName(), GetSectorSize(), GetSize());
	DEBUG_EXIT
}

const char *FlashRom::GetName() {
	return "GD32";
}

uint32_t FlashRom::GetSize() {
	return *(volatile uint16_t*)(0x1FFFF7E0) * 1024;
}

uint32_t FlashRom::GetSectorSize() {
	return FLASH_SECTOR_SIZE;
}

bool FlashRom::Read(uint32_t nOffset, uint32_t nLength, uint8_t *pBuffer, flashrom::result& nResult) {
	DEBUG_ENTRY
	DEBUG_PRINTF("offset=%p[%d], len=%u[%d], data=%p[%d]", nOffset, (((uint32_t)(nOffset) & 0x3) == 0), nLength, (((uint32_t)(nLength) & 0x3) == 0), pBuffer, (((uint32_t)(pBuffer) & 0x3) == 0));

	const auto *pSrc = reinterpret_cast<uint32_t *>(nOffset + FLASH_BASE);
	auto *pDst = reinterpret_cast<uint32_t *>(pBuffer);

	while (nLength > 0) {
		*pDst++ = *pSrc++;
		nLength -= 4;
	}

	nResult = result::OK;

	DEBUG_EXIT
	return true;
}

bool FlashRom::Erase(uint32_t nOffset, uint32_t nLength, flashrom::result& nResult) {
	DEBUG_ENTRY
	DEBUG_PRINTF("State=%d", static_cast<int>(s_State));

	nResult = result::OK;

	switch (s_State) {
	case State::IDLE:
		s_nPage = nOffset + FLASH_BASE;
		s_nLength = nLength;
		fmc_bank0_unlock();
		s_State = State::ERASE_BUSY;
		DEBUG_EXIT
		return false;
		break;
	case State::ERASE_BUSY:
		if (FMC_BUSY == fmc_bank0_state_get()) {
			DEBUG_EXIT
			return false;
		}

		FMC_CTL0 &= ~FMC_CTL0_PER;

		if (s_nLength == 0) {
			fmc_bank0_lock();
			s_State = State::IDLE;
			DEBUG_EXIT
			return true;
		}

		s_State = State::ERASE_PROGAM;
		DEBUG_EXIT
		return false;
		break;
	case State::ERASE_PROGAM:
		if (s_nLength > 0) {
			DEBUG_PRINTF("s_nPage=%p", s_nPage);

			FMC_CTL0 |= FMC_CTL0_PER;
			FMC_ADDR0 = s_nPage;
			FMC_CTL0 |= FMC_CTL0_START;

			s_nLength -= FLASH_PAGE;
			s_nPage += FLASH_PAGE;
		}
		s_State = State::ERASE_BUSY;
		DEBUG_EXIT
		return false;
		break;
	case State::WRITE_BUSY:
		FMC_CTL0 &= ~FMC_CTL0_PG;
		/* no break */
	case State::WRITE_PROGRAM:
		s_State = State::IDLE;
		DEBUG_EXIT
		return false;
		break;
	default:
		assert(0);
		break;
	}

	assert(0);
	return true;
}

bool FlashRom::Write(uint32_t nOffset, uint32_t nLength, const uint8_t *pBuffer, flashrom::result& nResult) {
	nResult = result::OK;

	switch (s_State) {
		case State::IDLE:
			DEBUG_PRINTF("State=%d", static_cast<int>(s_State));
			s_nAddress = nOffset + FLASH_BASE;
			s_pData = const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(pBuffer));
			s_nLength = nLength;
			fmc_bank0_unlock();
			s_State = State::WRITE_BUSY;
			DEBUG_EXIT
			return false;
			break;
		case State::WRITE_BUSY:
			if (FMC_BUSY == fmc_bank0_state_get()) {
				DEBUG_EXIT
				return false;
			}

			FMC_CTL0 &= ~FMC_CTL0_PG;

			if (s_nLength == 0) {
				fmc_bank0_lock();
				s_State = State::IDLE;
				DEBUG_EXIT
				return true;
			}

			s_State = State::WRITE_PROGRAM;
			return false;
			break;
		case State::WRITE_PROGRAM:
			if (s_nLength >= 4) {
	            FMC_CTL0 |= FMC_CTL0_PG;
	            REG32(s_nAddress) = *s_pData;

	            s_pData++;
	        	s_nAddress += 4;
	        	s_nLength -= 4;
			} else if (s_nLength > 0) {
	            FMC_CTL0 |= FMC_CTL0_PG;
	            REG32(s_nAddress) = *s_pData;
			}
			s_State = State::WRITE_BUSY;
			return false;
			break;
		case State::ERASE_BUSY:
			FMC_CTL0 &= ~FMC_CTL0_PER;
			/* no break */
		case State::ERASE_PROGAM:
			s_State = State::IDLE;
			DEBUG_EXIT
			return false;
			break;
		default:
			assert(0);
			break;
	}

	assert(0);
	return true;
}
