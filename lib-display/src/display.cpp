/**
 * @file display.cpp
 *
 */
/* Copyright (C) 2017-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <cstdio>
#include <cassert>

#include "displayset.h"
#include "display.h"

#if defined(ENABLE_TC1602)
# include "tc1602.h"
#endif
#include "ssd1306.h"
#if defined(ENABLE_SSD1311)
# include "ssd1311.h"
#endif

#include "display7segment.h"

#include "hal_i2c.h"

#if !defined(NO_HAL)
# include "hardware.h"
#endif

#if defined (BARE_METAL)
# include "console.h"
#endif

using namespace display;

Display *Display::s_pThis = nullptr;

Display::Display()
#if !defined(NO_HAL)
	: m_nMillis(Hardware::Get()->Millis())
#endif
{
	assert(s_pThis == nullptr);
	s_pThis = this;

#if defined(ENABLE_SSD1311)
	Detect(DisplayType::SSD1311);
#endif

	if (m_LcdDisplay == nullptr) {
		Detect(DisplayType::SSD1306);
	}

	PrintInfo();
}

Display::Display(uint8_t nCols, uint8_t nRows)
#if !defined(NO_HAL)
	: m_nMillis(Hardware::Get()->Millis())
#endif
{
	assert(s_pThis == nullptr);
	s_pThis = this;

	Detect(nCols, nRows);

	PrintInfo();
}

Display::Display(DisplayType tDisplayType): m_tType(tDisplayType),
#if !defined(NO_HAL)
	m_nMillis(Hardware::Get()->Millis())
#endif
{
	assert(s_pThis == nullptr);
	s_pThis = this;

	Detect(tDisplayType);

	PrintInfo();
}

void Display::Detect(DisplayType tDisplayType) {
	switch (tDisplayType) {
#if defined(ENABLE_TC1602)
		case DisplayType::PCF8574T_1602:
			m_LcdDisplay = new Tc1602(16, 2);
			assert(m_LcdDisplay != nullptr);
			break;
		case DisplayType::PCF8574T_2004:
			m_LcdDisplay = new Tc1602(20, 4);
			assert(m_LcdDisplay != nullptr);
			break;
#endif
#if defined(ENABLE_SSD1311)
		case DisplayType::SSD1311:
			m_LcdDisplay = new Ssd1311;
			assert(m_LcdDisplay != nullptr);
			break;
#endif
		case DisplayType::SSD1306:
			m_LcdDisplay = new Ssd1306(OLED_PANEL_128x64_8ROWS);
			assert(m_LcdDisplay != nullptr);
			break;
		case DisplayType::UNKNOWN:
			m_tType = DisplayType::UNKNOWN;
			/* no break */
		default:
			break;
	}

	if (m_LcdDisplay != nullptr) {
		if (!m_LcdDisplay->Start()) {
			delete m_LcdDisplay;
			m_LcdDisplay = nullptr;
			m_tType = DisplayType::UNKNOWN;
		} else {
			m_LcdDisplay->Cls();
		}
	}

	if (m_LcdDisplay == nullptr){
		m_nSleepTimeout = 0;
	}
}

void Display::Detect(__attribute__((unused)) uint8_t nCols, uint8_t nRows) {
	if (HAL_I2C::IsConnected(OLED_I2C_SLAVE_ADDRESS_DEFAULT)) {
		if (nRows <= 4) {
#if defined(ENABLE_SSD1311)
			m_LcdDisplay = new Ssd1311;
			assert(m_LcdDisplay != nullptr);

			if (m_LcdDisplay->Start()) {
				m_tType = DisplayType::SSD1311;
				Printf(1, "SSD1311");
			} else
#endif
			m_LcdDisplay = new Ssd1306(OLED_PANEL_128x64_4ROWS);
			assert(m_LcdDisplay != nullptr);
		} else {
			m_LcdDisplay = new Ssd1306(OLED_PANEL_128x64_8ROWS);
			assert(m_LcdDisplay != nullptr);
		}

		if (m_LcdDisplay->Start()) {
			m_tType = DisplayType::SSD1306;
			Printf(1, "SSD1306");
		}
	}
#if defined(ENABLE_TC1602)
	else if (HAL_I2C::IsConnected(TC1602_I2C_DEFAULT_SLAVE_ADDRESS)) {
		m_LcdDisplay = new Tc1602(nCols, nRows);
		assert(m_LcdDisplay != nullptr);

		if (m_LcdDisplay->Start()) {
			m_tType = DisplayType::PCF8574T_1602;
			Printf(1, "TC1602_PCF8574T");
		}
	}
#endif

	if (m_LcdDisplay == nullptr) {
		m_nSleepTimeout = 0;
	}
}

int Display::Printf(uint8_t nLine, const char *format, ...) {
	if (m_LcdDisplay == nullptr) {
		return 0;
	}

	char buffer[32];

	va_list arp;

	va_start(arp, format);

	auto i = vsnprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), format, arp);

	va_end(arp);

	m_LcdDisplay->TextLine(nLine, buffer, static_cast<uint16_t>(i));

	return i;
}

int Display::Write(uint8_t nLine, const char *pText) {
	if (m_LcdDisplay == nullptr) {
		return 0;
	}

	const auto *p = pText;
	int nCount = 0;

	const auto nColumns = static_cast<int>(m_LcdDisplay->GetColumns());

	while ((*p != 0) && (nCount++ < nColumns)) {
		++p;
	}

	m_LcdDisplay->TextLine(nLine, pText, static_cast<uint8_t>(nCount));

	return nCount;
}

void Display::SetCursorPos(uint8_t nCol, uint8_t nRow) {
	if (m_LcdDisplay == nullptr) {
		return;
	}

	m_LcdDisplay->SetCursorPos(nCol, nRow);
}

#if defined(ENABLE_CURSOR_MODE)
# define UNUSED
#else
# define UNUSED __attribute__((unused))
#endif

void Display::SetCursor(UNUSED uint32_t nMode) {
#if defined(ENABLE_CURSOR_MODE)
	if (m_LcdDisplay == nullptr) {
		return;
	}

	m_LcdDisplay->SetCursor(nMode);
#endif
}

void Display::TextStatus(const char *pText) {
	if (m_LcdDisplay == nullptr) {
		return;
	}

	const auto nColumns = m_LcdDisplay->GetColumns();
	const auto nRows = m_LcdDisplay->GetRows();

	assert((nColumns - 1) >= 0);
	assert((nRows - 1) >= 0);

	SetCursorPos(0, static_cast<uint8_t>(nRows - 1));

	for (uint32_t i = 0; i < static_cast<uint32_t>(nColumns - 1); i++) {
		PutChar(' ');
	}

	SetCursorPos(0, static_cast<uint8_t>(nRows - 1));

	Write(nRows, pText);
}

void Display::TextStatus(const char *pText, Display7SegmentMessage n7SegmentData, __attribute__((unused)) uint32_t nConsoleColor) {
	TextStatus(pText);
	m_Display7Segment.Status(n7SegmentData);
#if defined (BARE_METAL)
	if (nConsoleColor == UINT32_MAX) {
		return;
	}
	console_status(nConsoleColor, pText);
#endif
}

void Display::TextStatus(const char *pText, uint8_t nValue7Segment, bool bHex) {
	TextStatus(pText);
	m_Display7Segment.Status(nValue7Segment, bHex);
}

void Display::PrintInfo() {
	if (m_LcdDisplay == nullptr) {
		puts("No display found");
		return;
	}

	m_LcdDisplay->PrintInfo();
}

#if !defined(NO_HAL)
void Display::SetSleep(bool bSleep) {
	if (m_LcdDisplay == nullptr) {
		return;
	}

	m_bIsSleep = bSleep;

	m_LcdDisplay->SetSleep(bSleep);

	if (!bSleep) {
		m_nMillis = Hardware::Get()->Millis();
	}
}

void Display::Run() {
	if (m_nSleepTimeout == 0) {
		return;
	}

	if (!m_bIsSleep) {
		if (__builtin_expect(((Hardware::Get()->Millis() - m_nMillis) > m_nSleepTimeout), 0)) {
			SetSleep(true);
		}
	}
}
#endif
