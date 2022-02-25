/**
 * @file displayudfparams.h
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

#ifndef DISPLAYUDFPARAMS_H_
#define DISPLAYUDFPARAMS_H_

#include <cstdint>

#include "displayudf.h"

namespace displayudfparams {
struct Params {
    uint32_t nSetList;
    uint8_t nLabelIndex[28];
    uint8_t nSleepTimeout;
    uint8_t nIntensity;
}__attribute__((packed));

static_assert(static_cast<int>(displayudf::Labels::UNKNOWN) <= 28, "too many labels");
static_assert(sizeof(struct displayudfparams::Params) <= 48, "struct Params is too large");

struct Mask {
	static constexpr auto SLEEP_TIMEOUT = (1U << 28);
	static constexpr auto INTENSITY = (1U << 29);
	static constexpr auto FLIP_VERTICALLY = (1U << 30);
};
}  // namespace displayudfparams

class DisplayUdfParamsStore {
public:
	virtual ~DisplayUdfParamsStore() {}

	virtual void Update(const struct displayudfparams::Params *ptDisplayUdfParams)=0;
	virtual void Copy(struct displayudfparams::Params *ptDisplayUdfParams)=0;
};

class DisplayUdfParams {
public:
	DisplayUdfParams(DisplayUdfParamsStore *pDisplayUdfParamsStore = nullptr);

	bool Load();
	void Load(const char *pBuffer, uint32_t nLength);

	void Builder(const struct displayudfparams::Params *ptDisplayUdfParams, char *pBuffer, uint32_t nLength, uint32_t& nSize);
	void Save(char *pBuffer, uint32_t nLength, uint32_t& nSize);

	void Set(DisplayUdf *pDisplayUdf);

	void Dump();

    static void staticCallbackFunction(void *p, const char *s);

private:
    void callbackFunction(const char *s);
    bool isMaskSet(uint32_t nMask) const {
    	return (m_tDisplayUdfParams.nSetList & nMask) == nMask;
    }

private:
    DisplayUdfParamsStore *m_pDisplayUdfParamsStore;
    displayudfparams::Params m_tDisplayUdfParams;
};

#endif /* DISPLAYUDFPARAMS_H_ */
