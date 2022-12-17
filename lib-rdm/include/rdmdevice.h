/**
 * @file rdmdevice.h
 *
 */
/* Copyright (C) 2017-2022 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef RDMDEVICE_H_
#define RDMDEVICE_H_

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "rdmdevicestore.h"
#include "rdmconst.h"

#include "debug.h"

struct TRDMDeviceInfoData {
	char *data;
	uint8_t length;
};

///< http://rdm.openlighting.org/pid/display?manufacturer=0&pid=96
struct TRDMDeviceInfo {
	uint8_t protocol_major;			///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a Sub-Device.
	uint8_t protocol_minor;			///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a Sub-Device.
	uint8_t device_model[2];		///< This field identifies the Device Model ID of the Root Device or the Sub-Device. The Manufacturer shall not use the same ID to represent more than one unique model type.
	uint8_t product_category[2];	///< Devices shall report a Product Category based on the products primary function.
	uint8_t software_version[4];	///< This field indicates the Software Version ID for the device. The Software Version ID is a 32-bit value determined by the Manufacturer.
	uint8_t dmx_footprint[2];		///< If the DEVICE_INFO message is directed to a Sub-Device, then the response for this field contains the DMX512 Footprint for that Sub-Device. If the message is sent to the Root Device, it is the Footprint for the Root Device itself. If the Device or Sub-Device does not utilize Null Start Code packets for any control or functionality then it shall report a Footprint of 0x0000.
	uint8_t current_personality;	///<
	uint8_t personality_count;		///<
	uint8_t dmx_start_address[2];	///< If the Device or Sub-Device that this message is directed to has a DMX512 Footprint of 0, then this field shall be set to 0xFFFF.
	uint8_t sub_device_count[2];	///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a Sub-Device.
	uint8_t sensor_count;			///< This field indicates the number of available sensors in a Root Device or Sub-Device. When this parameter is directed to a Sub-Device, the reply shall be identical for any Sub-Device owned by a specific Root Device.
};

#include "rdm.h"

class RDMDevice {
public:
	RDMDevice();

	void Init() {
		assert(!m_IsInit);
		m_IsInit = true;

		RDMDevice::SetFactoryDefaults();
	}

	void Print();

	void SetRDMDeviceStore(RDMDeviceStore *pRDMDeviceStore) {
		m_pRDMDeviceStore = pRDMDeviceStore;
	}

	void SetFactoryDefaults() {
		DEBUG_ENTRY
		TRDMDeviceInfoData info = {m_aFactoryRootLabel, m_nFactoryRootLabelLength};

		RDMDevice::SetLabel(&info);

		m_nCheckSum = RDMDevice::CalculateChecksum();
		DEBUG_EXIT
	}

	bool GetFactoryDefaults() {
		return (m_nCheckSum == RDMDevice::CalculateChecksum());
	}

	const uint8_t *GetUID() const {
		return m_aUID;
	}

	const uint8_t *GetSN() const {
		return m_aSN;
	}

	void GetManufacturerId(struct TRDMDeviceInfoData *pInfo) {
		pInfo->data = reinterpret_cast<char *>(const_cast<uint8_t *>(RDMConst::MANUFACTURER_ID));
		pInfo->length = RDM_DEVICE_MANUFACTURER_ID_LENGTH;
	}

	void GetManufacturerName(struct TRDMDeviceInfoData *pInfo) {
		pInfo->data = m_aManufacturerName;
		pInfo->length = m_nManufacturerNameLength;
	}

	void SetLabel(const struct TRDMDeviceInfoData *pInfo) {
		const auto nLength = std::min(static_cast<uint8_t>(RDM_DEVICE_LABEL_MAX_LENGTH), pInfo->length);

		if (m_IsInit) {
			memcpy(m_aRootLabel, pInfo->data, nLength);
			m_nRootLabelLength = nLength;

			if (m_pRDMDeviceStore != nullptr) {
				m_pRDMDeviceStore->SaveLabel(m_aRootLabel, m_nRootLabelLength);
			}
		} else {
			memcpy(m_aFactoryRootLabel, pInfo->data, nLength);
			m_nFactoryRootLabelLength = nLength;
		}
	}

	void GetLabel(struct TRDMDeviceInfoData *pInfo) {
		pInfo->data = m_aRootLabel;
		pInfo->length = m_nRootLabelLength;
	}

	void SetProductCategory(uint16_t nProductCategory) {
		m_nProductCategory = nProductCategory;
	}
	uint16_t GetProductCategory() const {
		return m_nProductCategory;
	}

	void SetProductDetail(uint16_t nProductDetail) {
		m_nProductDetail = nProductDetail;
	}
	uint16_t GetProductDetail() const {
		return m_nProductDetail;
	}

private:
	uint16_t CalculateChecksum() {
		uint16_t nChecksum = m_nFactoryRootLabelLength;

		for (uint32_t i = 0; i < m_nRootLabelLength; i++) {
			nChecksum = static_cast<uint16_t>(nChecksum + m_aRootLabel[i]);
		}

		return nChecksum;
	}

private:
	bool m_IsInit { false };
	uint8_t m_aUID[RDM_UID_SIZE];
#define DEVICE_SN_LENGTH		4
	uint8_t m_aSN[DEVICE_SN_LENGTH];
	char m_aRootLabel[RDM_DEVICE_LABEL_MAX_LENGTH];
	uint8_t m_nRootLabelLength;
	char m_aManufacturerName[RDM_MANUFACTURER_LABEL_MAX_LENGTH];
	uint8_t m_nManufacturerNameLength;
	uint16_t m_nProductCategory;
	uint16_t m_nProductDetail;
	char m_aFactoryRootLabel[RDM_DEVICE_LABEL_MAX_LENGTH];
	uint8_t m_nFactoryRootLabelLength { 0 };
	uint16_t m_nCheckSum { 0 };

	RDMDeviceStore *m_pRDMDeviceStore { nullptr };
};

#endif /* RDMDEVICE_H_ */
