/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2021-2023 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <cstdio>
#include <cstdint>

#include "hardware.h"
#include "network.h"
#include "display.h"

#include "widget.h"
#include "widgetparams.h"
#include "widgetstore.h"
#include "rdmdeviceparams.h"

#include "configstore.h"

#include "storewidget.h"
#include "storerdmdevice.h"

#include "software_version.h"

#ifndef ALIGNED
# define ALIGNED __attribute__ ((aligned (4)))
#endif

static constexpr char widget_mode_names[4][12] ALIGNED = {"DMX_RDM", "DMX", "RDM" , "RDM_SNIFFER" };
static constexpr TRDMDeviceInfoData deviceLabel ALIGNED = { const_cast<char*>("GD32F103RC DMX USB Pro"), 22 };

void main() {
	Hardware hw;
	Display display; // Not supported, yet.
	ConfigStore configStore;
	Network nw;

	Widget widget;
	widget.SetPortDirection(0, dmx::PortDirection::INP, false);

	StoreWidget storeWidget;
	WidgetParams widgetParams(&storeWidget);

	if (widgetParams.Load()) {
		widgetParams.Dump();
		widgetParams.Set();
	}

	StoreRDMDevice storeRDMDevice;
	RDMDeviceParams rdmDeviceParams(&storeRDMDevice);

	widget.SetLabel(&deviceLabel);

	if (rdmDeviceParams.Load()) {
		rdmDeviceParams.Dump();
		rdmDeviceParams.Set(&widget);
	}

	widget.Init();

	const auto *pRdmDeviceUid = widget.GetUID();
	TRDMDeviceInfoData tRdmDeviceLabel;
	widget.GetLabel(&tRdmDeviceLabel);
	const auto widgetMode = widgetParams.GetMode();

	uint8_t nHwTextLength;
	printf("[V%s] %s Compiled on %s at %s\n", SOFTWARE_VERSION, hw.GetBoardName(nHwTextLength), __DATE__, __TIME__);
	printf("RDM Controller with USB [Compatible with Enttec USB Pro protocol], Widget mode : %d (%s)\n", widgetMode, widget_mode_names[static_cast<uint32_t>(widgetMode)]);
	printf("Device UUID : %.2x%.2x:%.2x%.2x%.2x%.2x, ", pRdmDeviceUid[0], pRdmDeviceUid[1], pRdmDeviceUid[2], pRdmDeviceUid[3], pRdmDeviceUid[4], pRdmDeviceUid[5]);
	printf("Label : %.*s\n", static_cast<int>(tRdmDeviceLabel.length), reinterpret_cast<const char*>(tRdmDeviceLabel.data));

	hw.WatchdogInit();

	if (widgetMode == widget::Mode::RDM_SNIFFER) {
		widget.SetPortDirection(0, dmx::PortDirection::INP, true);
		widget.SnifferFillTransmitBuffer();	// Prevent missing first frame
	}

	for (;;) {
		hw.WatchdogFeed();
		widget.Run();
		configStore.Flash();
		hw.Run();
	}
}
