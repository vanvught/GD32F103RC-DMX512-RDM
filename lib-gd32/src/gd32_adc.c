/**
 * @file gd32_adc.c
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <stdint.h>

#include "gd32.h"

void gd32_adc_init(void) {
	rcu_periph_clock_enable(RCU_ADC0);
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV12);

	/* ADC SCAN function enable */
	adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
	/* ADC trigger config */
	adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);
	/* ADC data alignment config */
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	/* ADC mode config */
	adc_mode_config(ADC_MODE_FREE);
	/* ADC channel length config */
	adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);

	/* ADC temperature sensor channel config */
	adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
	/* ADC internal reference voltage channel config */
	adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

	/* ADC external trigger enable */
	adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);

	/* ADC temperature and Vrefint enable */
	adc_tempsensor_vrefint_enable();

	/* enable ADC interface */
	adc_enable(ADC0);
	udelay(1000);
	/* ADC calibration and reset calibration */
	adc_calibration_enable(ADC0);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
}

float gd32_adc_gettemp(void) {
    /* value convert  */
    const float temperature = (1.43 - ADC_IDATA0(ADC0) * 3.3 / 4096) * 1000 / 4.3 + 25;
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
    return temperature;
}

float gd32_adc_getvref(void) {
	const float vref_value = (ADC_IDATA1(ADC0) * 3.3 / 4096);
	 adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
	return vref_value;
}
