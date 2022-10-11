/**
 * @file displayudfhandler.h
 *
 */
/* Copyright (C) 2022 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef NODE_DISPLAYUDFHANDLER_H_
#define NODE_DISPLAYUDFHANDLER_H_

#include <cstdint>

#include "displayudf.h"

#include "nodedisplay.h"

#include "lightset.h"

#include "debug.h"

class DisplayUdfHandler final: public NodeDisplay {
public:
	DisplayUdfHandler() {
		DEBUG_ENTRY

		DEBUG_EXIT
	}

	~DisplayUdfHandler() {
		DEBUG_ENTRY

		DEBUG_EXIT
	}

	void ShowShortName(__attribute__((unused)) const char *pShortName) override {
		DisplayUdf::Get()->ShowNodeName();
	}

	void ShowLongName(__attribute__((unused)) const char *pLongName) override {
	}

	void ShowUniverseSwitch(__attribute__((unused))  uint32_t nPortIndex, __attribute__((unused))  uint8_t nAddress) override {
		DisplayUdf::Get()->ShowUniverse();
	}

	void ShowNetSwitch(__attribute__((unused))  uint8_t nAddress) override {
		DisplayUdf::Get()->ShowUniverse();
	}

	void ShowSubnetSwitch(__attribute__((unused))  uint8_t nAddress) override {
		DisplayUdf::Get()->ShowUniverse();
	}

	void ShowMergeMode(__attribute__((unused))  uint32_t nPortIndex, __attribute__((unused))  lightset::MergeMode mergeMode) override {
		DisplayUdf::Get()->ShowUniverse();
	}

	void ShowPortProtocol(__attribute__((unused))  uint32_t nPortIndex, __attribute__((unused))  artnet::PortProtocol tPortProtocol) override {
		DisplayUdf::Get()->ShowUniverse();
	}

	void ShowRdmEnabled(__attribute__((unused)) uint32_t nPortIndex, __attribute__((unused)) bool isEnabled) {
		//TODO ShowRdmEnabled
	}

	void ShowFailSafe(__attribute__((unused)) uint8_t nFailsafe) {
		//TODO ShowFailSafe
	}
};

#endif /* NODE_DISPLAYUDFHANDLER_H_ */
