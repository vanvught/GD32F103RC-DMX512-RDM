/**
 * @file ssd1306.cpp
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
#include <cstring>
#include <cstdio>
#include <cassert>

#include "ssd1306.h"

#include "hal_i2c.h"

namespace ssd1306 {
static constexpr auto SSD1306_LCD_WIDTH = 128;
namespace mode {
static constexpr auto COMMAND = 0x00;
static constexpr auto DATA = 0x40;
}  // namespace mode
namespace cmd {
static constexpr auto SET_LOWCOLUMN = 0x00;
static constexpr auto SET_HIGHCOLUMN = 0x10;
static constexpr auto SET_MEMORYMODE = 0x20;
// static constexpr auto SET_COLUMNADDR = 0x21;
// static constexpr auto SET_PAGEADDR = 0x22;
static constexpr auto SET_STARTLINE = 0x40;
static constexpr auto SET_CONTRAST = 0x81;
static constexpr auto SET_CHARGEPUMP = 0x8D;
static constexpr auto SEGREMAP = 0xA0;
static constexpr auto OUPUT_RAM = 0xA4;
static constexpr auto DISPLAY_NORMAL = 0xA6;
// static constexpr auto DISPLAY_INVERT = 0xA7;
static constexpr auto SET_MULTIPLEX = 0xA8;
static constexpr auto DISPLAY_OFF = 0xAE;
static constexpr auto DISPLAY_ON = 0xAF;
static constexpr auto SET_STARTPAGE = 0xB0;
static constexpr auto COMSCAN_INC = 0xC0;
static constexpr auto COMSCAN_DEC = 0xC8;
static constexpr auto SET_DISPLAYOFFSET = 0xD3;
static constexpr auto SET_DISPLAYCLOCKDIV = 0xD5;
static constexpr auto SET_PRECHARGE = 0xD9;
static constexpr auto SET_COMPINS = 0xDA;
static constexpr auto SET_VCOMDETECT = 0xDB;
}  // namespace cmd
namespace oled {
namespace font8x6 {
static constexpr auto CHAR_H = 8;
static constexpr auto CHAR_W = 6;
static constexpr auto COLS = (SSD1306_LCD_WIDTH / CHAR_W);
}  // namespace font8x6
}  // namespace oled
}  // namespace ssd1306

static const uint8_t _OledFont8x6[] __attribute__((aligned(4))) = {
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00,
	0x40, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00,
	0x40, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00,
	0x40, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00,
	0x40, 0x36, 0x49, 0x56, 0x20, 0x50, 0x00,
	0x40, 0x00, 0x08, 0x07, 0x03, 0x00, 0x00,
	0x40, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00,
	0x40, 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00,
	0x40, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00,
	0x40, 0x00, 0x80, 0x70, 0x30, 0x00, 0x00,
	0x40, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00,
	0x40, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
	0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,
	0x40, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00,
	0x40, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00,
	0x40, 0x72, 0x49, 0x49, 0x49, 0x46, 0x00,
	0x40, 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00,
	0x40, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00,
	0x40, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00,
	0x40, 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00,
	0x40, 0x41, 0x21, 0x11, 0x09, 0x07, 0x00,
	0x40, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00,
	0x40, 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00,
	0x40, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x40, 0x34, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x08, 0x14, 0x22, 0x41, 0x00,
	0x40, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00,
	0x40, 0x00, 0x41, 0x22, 0x14, 0x08, 0x00,
	0x40, 0x02, 0x01, 0x59, 0x09, 0x06, 0x00,
	0x40, 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00,
	0x40, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00,
	0x40, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00,
	0x40, 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00,
	0x40, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00,
	0x40, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00,
	0x40, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00,
	0x40, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00,
	0x40, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00,
	0x40, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00,
	0x40, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00,
	0x40, 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00,
	0x40, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00,
	0x40, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00,
	0x40, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00,
	0x40, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00,
	0x40, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00,
	0x40, 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00,
	0x40, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00,
	0x40, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00,
	0x40, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00,
	0x40, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00,
	0x40, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00,
	0x40, 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00,
	0x40, 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00,
	0x40, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00,
	0x40, 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00,
	0x40, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00,
	0x40, 0x00, 0x03, 0x07, 0x08, 0x00, 0x00,
	0x40, 0x20, 0x54, 0x54, 0x78, 0x40, 0x00,
	0x40, 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x44, 0x28, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00,
	0x40, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00,
	0x40, 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00,
	0x40, 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00,
	0x40, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00,
	0x40, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00,
	0x40, 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00,
	0x40, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00,
	0x40, 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00,
	0x40, 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,
	0x40, 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00,
	0x40, 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00,
	0x40, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00,
	0x40, 0x48, 0x54, 0x54, 0x54, 0x24, 0x00,
	0x40, 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00,
	0x40, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00,
	0x40, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00,
	0x40, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00,
	0x40, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00,
	0x40, 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00,
	0x40, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00,
	0x40, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x36, 0x08, 0x00, 0x00,
	0x40, 0x02, 0x01, 0x02, 0x04, 0x02, 0x00,
	0x40, 0x3C, 0x26, 0x23, 0x26, 0x3C, 0x00
};

using namespace ssd1306;

static const uint8_t oled_128x64_init[] __attribute__((aligned(4))) = {
		cmd::DISPLAY_OFF,
		cmd::SET_DISPLAYCLOCKDIV, 0x80, 		// The suggested value
		cmd::SET_MULTIPLEX, 0x3F,			// 1/64
		cmd::SET_DISPLAYOFFSET, 0x00,		// No offset
		cmd::SET_STARTLINE | 0x00,			// line #0
		cmd::SET_CHARGEPUMP, 0x14,
		cmd::SET_MEMORYMODE, 0x00,			// Horizontal addressing
		cmd::SEGREMAP | 0x01,				// Flip horizontally
		cmd::COMSCAN_DEC,					// Flip vertically
		cmd::SET_COMPINS, 0x12,
		cmd::SET_CONTRAST, 0x7F,				// 0x00 to 0xFF
		cmd::SET_PRECHARGE, 0xF1,
		cmd::SET_VCOMDETECT, 0x40,
		cmd::OUPUT_RAM,
		cmd::DISPLAY_NORMAL };

static const uint8_t oled_128x32_init[] __attribute__((aligned(4))) = {
		cmd::DISPLAY_OFF,
		cmd::SET_DISPLAYCLOCKDIV, 0x80, 		// The suggested value
		cmd::SET_MULTIPLEX, 0x1F,			// 1/32
		cmd::SET_DISPLAYOFFSET, 0x00,		// No offset
		cmd::SET_STARTLINE | 0x00,			// line #0
		cmd::SET_CHARGEPUMP, 0x14,
		cmd::SET_MEMORYMODE, 0x00,			// Horizontal addressing
		cmd::SEGREMAP | 0x01,				// Flip horizontally
		cmd::COMSCAN_DEC,					// Flip vertically
		cmd::SET_COMPINS, 0x02,
		cmd::SET_CONTRAST, 0x7F,				// 0x00 to 0xFF
		cmd::SET_PRECHARGE, 0xF1,
		cmd::SET_VCOMDETECT, 0x40,
		cmd::OUPUT_RAM,
		cmd::DISPLAY_NORMAL };

static uint8_t _ClearBuffer[133 + 1] __attribute__((aligned(4)));

Ssd1306 *Ssd1306::s_pThis = nullptr;

Ssd1306::Ssd1306() : m_I2C(OLED_I2C_SLAVE_ADDRESS_DEFAULT) {
	assert(s_pThis == nullptr);
	s_pThis = this;

	InitMembers();
}

Ssd1306::Ssd1306(TOledPanel tOledPanel) : m_I2C(OLED_I2C_SLAVE_ADDRESS_DEFAULT), m_OledPanel(tOledPanel) {
	assert(s_pThis == nullptr);
	s_pThis = this;

	InitMembers();
}

Ssd1306::Ssd1306(uint8_t nSlaveAddress, TOledPanel tOledPanel) : m_I2C(nSlaveAddress == 0 ? OLED_I2C_SLAVE_ADDRESS_DEFAULT : nSlaveAddress), m_OledPanel(tOledPanel) {
	assert(s_pThis == nullptr);
	s_pThis = this;

	InitMembers();
}

void Ssd1306::PrintInfo() {
	printf("%s (%d,%d)\n", m_bHaveSH1106 ? "SH1106" : "SSD1306", m_nRows, m_nCols);
}

void Ssd1306::CheckSH1106() {
	// Check for columns 128-133
	SendCommand(cmd::SET_LOWCOLUMN | (128 & 0XF));
	SendCommand(cmd::SET_HIGHCOLUMN | (128));
	SendCommand(cmd::SET_STARTPAGE);

	constexpr uint8_t aTestBytes[5] = { mode::DATA, 0xAA, 0xEE, 0xAA, 0xEE };
	SendData(aTestBytes, sizeof(aTestBytes));

	// Check for columns 128-133
	SendCommand(cmd::SET_LOWCOLUMN | (128 & 0XF));
	SendCommand(cmd::SET_HIGHCOLUMN | (128));
	SendCommand(cmd::SET_STARTPAGE);

	char aResultBytes[5] = {0};

	m_I2C.Write(0x40);
	m_I2C.Read(aResultBytes, sizeof(aResultBytes));

#ifndef NDEBUG
	printf("%.2x %.2x %.2x %.2x %.2x\n", aResultBytes[0], aResultBytes[1], aResultBytes[2], aResultBytes[3], aResultBytes[4]);
#endif

	m_bHaveSH1106 = (memcmp(&aTestBytes[1], &aResultBytes[1], 4) == 0);

#ifndef NDEBUG
	printf("m_bHaveSH1106=%d\n", m_bHaveSH1106);
#endif
}

bool Ssd1306::Start() {
	if (!m_I2C.IsConnected()) {
		return false;
	}

	switch (m_OledPanel) {
	case OLED_PANEL_128x64_8ROWS:
		for (unsigned i = 0; i < sizeof(oled_128x64_init); i++) {
			SendCommand(oled_128x64_init[i]);
		}
		break;
	case OLED_PANEL_128x64_4ROWS:
		/* no break */
	case OLED_PANEL_128x32_4ROWS:
		for (unsigned i = 0; i < sizeof(oled_128x32_init); i++) {
			SendCommand(oled_128x32_init[i]);
		}
		break;
	default:
		return false;
	}

	for (unsigned i = 0; i < sizeof(_ClearBuffer); i++) {
		_ClearBuffer[i] = 0x00;
	}

	_ClearBuffer[0] = 0x40;

	CheckSH1106();

	Ssd1306::Cls();

	SendCommand(cmd::DISPLAY_ON);

	m_bIsFlippedVertically = true;

	return true;
}

void Ssd1306::Cls() {
	uint8_t nColumnAdd = 0;

	if (m_bHaveSH1106) {
		nColumnAdd = 4;
	}

	for (uint32_t nPage = 0; nPage < m_nPages; nPage++) {
		SendCommand(cmd::SET_LOWCOLUMN | (nColumnAdd & 0XF));
		SendCommand(static_cast<uint8_t>(cmd::SET_HIGHCOLUMN | (nColumnAdd)));
		SendCommand(static_cast<uint8_t>(cmd::SET_STARTPAGE | nPage));

		switch (m_OledPanel) {
		case OLED_PANEL_128x64_8ROWS:
			SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), static_cast<uint32_t>(nColumnAdd + SSD1306_LCD_WIDTH + 1));
			break;
		case OLED_PANEL_128x64_4ROWS:
			/* no break */
		case OLED_PANEL_128x32_4ROWS:
			SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), static_cast<uint32_t>(nColumnAdd + SSD1306_LCD_WIDTH + 1));
			break;
		default:
			break;
		}
	}

	SendCommand(cmd::SET_LOWCOLUMN | (nColumnAdd & 0XF));
	SendCommand(static_cast<uint8_t>(cmd::SET_HIGHCOLUMN | (nColumnAdd)));
	SendCommand(cmd::SET_STARTPAGE);

#if defined(ENABLE_CURSOR_MODE)
	m_nShadowRamIndex = 0;
	memset(m_pShadowRam, ' ', static_cast<size_t>(m_nCols * m_nRows));
#endif
}

void Ssd1306::PutChar(int c) {
	int i;

	if (c < 32 || c > 127) {
#if defined(ENABLE_CURSOR_MODE)
		c = 32;
#endif
		i = 0;
	} else {
		i = (c - 32);
	}

#if defined(ENABLE_CURSOR_MODE)
	m_pShadowRam[m_nShadowRamIndex++] = static_cast<char>(c);
#endif
	const uint8_t *base = _OledFont8x6 + (oled::font8x6::CHAR_W + 1) * i;
	SendData(base, oled::font8x6::CHAR_W + 1);
}

void Ssd1306::PutString(const char *pString) {
	const char *p = pString;

	for (uint32_t i = 0; *p != '\0'; i++) {
		Ssd1306::PutChar(static_cast<int>(*p));
		p++;
	}
}

/**
 * nLine [1..4]
 */
void Ssd1306::ClearLine(uint8_t nLine) {
	if (__builtin_expect((!((nLine > 0) && (nLine <= m_nRows))), 0)) {
		return;
	}

	Ssd1306::SetCursorPos(0, static_cast<uint8_t>(nLine - 1));
	SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), SSD1306_LCD_WIDTH + 1);
	Ssd1306::SetCursorPos(0, static_cast<uint8_t>(nLine - 1));
}

void Ssd1306::TextLine(uint8_t nLine, const char *pData, uint32_t nLength) {
	if (__builtin_expect((!((nLine > 0) && (nLine <= m_nRows))), 0)) {
		return;
	}

	Ssd1306::SetCursorPos(0, static_cast<uint8_t>(nLine - 1));
	Text(pData, nLength);
}

void Ssd1306::Text(const char *pData, uint32_t nLength) {
	if (nLength > m_nCols) {
		nLength = m_nCols;
	}

	for (uint32_t i = 0; i < nLength; i++) {
		Ssd1306::PutChar(pData[i]);
	}
}

/**
 * (0,0)
 */
void Ssd1306::SetCursorPos(uint8_t nCol, uint8_t nRow) {
	if  (__builtin_expect((!((nCol < oled::font8x6::COLS) && (nRow < m_nRows))), 0)) {
		return;
	}

	nCol = static_cast<uint8_t>(nCol * oled::font8x6::CHAR_W);

	if (m_bHaveSH1106) {
		nCol = static_cast<uint8_t>(nCol + 4);
	}

	SendCommand(cmd::SET_LOWCOLUMN | (nCol & 0XF));
	SendCommand(static_cast<uint8_t>(cmd::SET_HIGHCOLUMN | (nCol >> 4)));
	SendCommand(static_cast<uint8_t>(cmd::SET_STARTPAGE | nRow));

#if defined(ENABLE_CURSOR_MODE)
	m_nShadowRamIndex = static_cast<uint16_t>((nRow * oled::font8x6::COLS) + (nCol / oled::font8x6::CHAR_W));

	if (m_tCursorMode == display::cursor::ON) {
		SetCursorOff();
		SetCursorOn();
	} else if (m_tCursorMode == static_cast<uint32_t>((display::cursor::ON | display::cursor::BLINK_ON))) {
		SetCursorOff();
		SetCursorBlinkOn();
	}
#endif
}

void Ssd1306::SetSleep(bool bSleep) {
	if (bSleep) {
		SendCommand(cmd::DISPLAY_OFF);
	} else {
		SendCommand(cmd::DISPLAY_ON);
	}
}

void Ssd1306::SetContrast(uint8_t nContrast) {
	m_nContrast = nContrast;

	SendCommand(cmd::SET_CONTRAST);
	SendCommand(nContrast);
}


void Ssd1306::SetFlipVertically(bool doFlipVertically)  {
	m_bIsFlippedVertically = doFlipVertically;

	if (doFlipVertically) {
		SendCommand(cmd::COMSCAN_INC);
		SendCommand(cmd::SEGREMAP);
	} else {
		SendCommand(cmd::COMSCAN_DEC);
		SendCommand(cmd::SEGREMAP | 0x01);
	}
}

void Ssd1306::InitMembers() {
	switch (m_OledPanel) {
	case OLED_PANEL_128x64_8ROWS:
		m_nCols = oled::font8x6::COLS;
		m_nRows = 64 / oled::font8x6::CHAR_H;
		break;
	case OLED_PANEL_128x64_4ROWS:			// Trick : 128x32
		/* no break */
	case OLED_PANEL_128x32_4ROWS:
		m_nCols = oled::font8x6::COLS;
		m_nRows = 32 / oled::font8x6::CHAR_H;
		break;
	default:
		m_nCols = oled::font8x6::COLS;
		m_nRows = 64 / oled::font8x6::CHAR_H;
		break;
	}

	m_nPages = (m_OledPanel == OLED_PANEL_128x64_8ROWS ? 8 : 4);

#if defined(ENABLE_CURSOR_MODE)
	m_pShadowRam = new char[oled::font8x6::COLS * m_nRows];
	m_nShadowRamIndex = 0;
	memset(m_pShadowRam, ' ', static_cast<size_t>(oled::font8x6::COLS * m_nRows));
#endif
}

void Ssd1306::SendCommand(uint8_t nCmd) {
	m_I2C.WriteRegister(mode::COMMAND, nCmd);
}

void Ssd1306::SendData(const uint8_t *pData, uint32_t nLength) {
	m_I2C.Write(reinterpret_cast<const char*>(pData), nLength);
}

/**
 *  Cursor mode support
 */

#if defined(ENABLE_CURSOR_MODE)
# define UNUSED
#else
# define UNUSED __attribute__((unused))
#endif

void Ssd1306::SetCursor(UNUSED uint32_t tCursorMode) {
#if defined(ENABLE_CURSOR_MODE)
	if (tCursorMode == m_tCursorMode) {
		return;
	}

	m_tCursorMode = tCursorMode;

	switch (static_cast<int>(tCursorMode)) {
	case display::cursor::OFF:
		SetCursorOff();
		break;
	case display::cursor::ON:
		SetCursorOn();
		break;
	case display::cursor::ON | display::cursor::BLINK_ON:
		SetCursorBlinkOn();
		break;
	default:
		break;
	}
#endif
}

void Ssd1306::SetCursorOn() {
#if defined(ENABLE_CURSOR_MODE)
	m_nCursorOnCol = static_cast<uint8_t>(m_nShadowRamIndex % oled::font8x6::COLS);
	m_nCursorOnRow =  static_cast<uint8_t>(m_nShadowRamIndex / oled::font8x6::COLS);
	m_nCursorOnChar = static_cast<uint8_t>(m_pShadowRam[m_nShadowRamIndex] - 32);

	const auto *pBase = const_cast<uint8_t *>(_OledFont8x6) + 1 + (oled::font8x6::CHAR_W + 1) * m_nCursorOnChar;

	uint8_t data[oled::font8x6::CHAR_W + 1];
	data[0] = 0x40;

	for (uint32_t i = 1 ; i <= oled::font8x6::CHAR_W; i++) {
		data[i] = *pBase | 0x80;
		pBase++;
	}

	SendData(data, oled::font8x6::CHAR_W + 1);
	SetColumnRow(m_nCursorOnCol, m_nCursorOnRow);
#endif
}

void Ssd1306::SetCursorBlinkOn() {
#if defined(ENABLE_CURSOR_MODE)
	m_nCursorOnCol = static_cast<uint8_t>(m_nShadowRamIndex % oled::font8x6::COLS);
	m_nCursorOnRow =  static_cast<uint8_t>(m_nShadowRamIndex / oled::font8x6::COLS);
	m_nCursorOnChar = static_cast<uint8_t>(m_pShadowRam[m_nShadowRamIndex] - 32);

	const uint8_t *pBase = const_cast<uint8_t *>(_OledFont8x6) + 1 + (oled::font8x6::CHAR_W + 1) * m_nCursorOnChar;

	uint8_t data[oled::font8x6::CHAR_W + 1];
	data[0] = 0x40;

	for (uint32_t i = 1 ; i <= oled::font8x6::CHAR_W; i++) {
		data[i] = static_cast<uint8_t>(~*pBase);
		pBase++;
	}

	SendData(data, static_cast<uint32_t>(oled::font8x6::CHAR_W + 1));
	SetColumnRow(m_nCursorOnCol, m_nCursorOnRow);
#endif
}

void Ssd1306::SetCursorOff() {
#if defined(ENABLE_CURSOR_MODE)
	const auto nCol = static_cast<uint8_t>(m_nShadowRamIndex % oled::font8x6::COLS);
	const auto nRow = static_cast<uint8_t>(m_nShadowRamIndex / oled::font8x6::COLS);

	SetColumnRow(m_nCursorOnCol, m_nCursorOnRow);

	const uint8_t *pBase = _OledFont8x6 + (oled::font8x6::CHAR_W + 1) * m_nCursorOnChar;

	SendData(pBase, (oled::font8x6::CHAR_W + 1));
	SetColumnRow(nCol, nRow);
#endif
}

void Ssd1306::SetColumnRow(UNUSED uint8_t nColumn, UNUSED uint8_t nRow) {
#if defined(ENABLE_CURSOR_MODE)
	auto nColumnAdd = static_cast<uint8_t>(nColumn * oled::font8x6::CHAR_W);

	if (m_bHaveSH1106) {
		nColumnAdd = static_cast<uint8_t>(nColumnAdd + 4);
	}

	SendCommand(cmd::SET_LOWCOLUMN | (nColumnAdd & 0xF));
	SendCommand(cmd::SET_HIGHCOLUMN | static_cast<uint8_t>(nColumnAdd >> 4));
	SendCommand(cmd::SET_STARTPAGE | nRow);
#endif
}

void Ssd1306::DumpShadowRam() {
#if defined(ENABLE_CURSOR_MODE)
#ifndef NDEBUG
	for (uint32_t i = 0; i < m_nRows; i++) {
		printf("%d: [%.*s]\n", i, oled::font8x6::COLS, &m_pShadowRam[i * oled::font8x6::COLS]);
	}
#endif
#endif
}
