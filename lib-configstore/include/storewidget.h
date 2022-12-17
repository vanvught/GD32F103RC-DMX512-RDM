/**
 * @file storewidget.h
 *
 */
/* Copyright (C) 2019-2022 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef STOREWIDGET_H_
#define STOREWIDGET_H_

#include "widgetparams.h"
#include "widgetstore.h"

#include "configstore.h"

class StoreWidget final: public WidgetParamsStore, public WidgetStore {
public:
	StoreWidget();

	void Update(const struct TWidgetParams* pWidgetParams) {
		ConfigStore::Get()->Update(configstore::Store::WIDGET, pWidgetParams, sizeof(struct TWidgetParams));
	}

	void Copy(struct TWidgetParams* pWidgetParams) {
		ConfigStore::Get()->Copy(configstore::Store::WIDGET, pWidgetParams, sizeof(struct TWidgetParams));
	}

	void UpdateBreakTime(uint8_t nBreakTime) {
		ConfigStore::Get()->Update(configstore::Store::WIDGET, __builtin_offsetof(struct TWidgetParams, nBreakTime), &nBreakTime, sizeof(uint8_t), WidgetParamsMask::BREAK_TIME);
	}

	void UpdateMabTime(uint8_t nMabTime) {
		ConfigStore::Get()->Update(configstore::Store::WIDGET, __builtin_offsetof(struct TWidgetParams, nMabTime), &nMabTime, sizeof(uint8_t), WidgetParamsMask::MAB_TIME);
	}

	void UpdateRefreshRate(uint8_t nRefreshRate) {
		ConfigStore::Get()->Update(configstore::Store::WIDGET, __builtin_offsetof(struct TWidgetParams, nRefreshRate), &nRefreshRate, sizeof(uint8_t), WidgetParamsMask::REFRESH_RATE);
	}

	static StoreWidget* Get() {
		return s_pThis;
	}

private:
	static StoreWidget *s_pThis;
};

#endif /* STOREWIDGET_H_ */
