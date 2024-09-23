/**
 * @file time.cpp
 *
 */
/* Copyright (C) 2016-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <cstddef>
#include <cstdint>
#include <time.h>

namespace global {
int32_t *gp_nUtcOffset;
}  // namespace global

static constexpr int days_of_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int isleapyear(const int year) {
	if (year % 100 == 0) {
		return year % 400 == 0;
	}

	return year % 4 == 0;
}

static int getdaysofmonth(const int month, const int year) {
	if ((month == 1) && isleapyear(year)) {
		return 29;
	}

	return days_of_month[month];
}

static struct tm Tm;

extern "C" {

struct tm *localtime(const time_t *pTime) {
	if (pTime == nullptr) {
		return nullptr;
	}

	auto nTime = *pTime + *global::gp_nUtcOffset;
	return gmtime(&nTime);
}

struct tm *gmtime(const time_t *pTime) {
	if (pTime == nullptr) {
		return nullptr;
	}

	auto nTime = *pTime;

	Tm.tm_sec = nTime % 60;
	nTime /= 60;
	Tm.tm_min = nTime % 60;
	nTime /= 60;
	Tm.tm_hour = nTime % 24;
	nTime /= 24;

	Tm.tm_wday = (nTime + 4) % 7;

	int nYear = 1970;

	while (1) {
		const time_t nDaysOfYear = isleapyear(nYear) ? 366 : 365;
		if (nTime < nDaysOfYear) {
			break;
		}

		nTime -= nDaysOfYear;
		nYear++;
	}

	Tm.tm_year = nYear - 1900;
	Tm.tm_yday = nTime;

	int nMonth = 0;

	while (1) {
		const time_t nDaysOfMonth = getdaysofmonth(nMonth, nYear);
		if (nTime < nDaysOfMonth) {
			break;
		}

		nTime -= nDaysOfMonth;
		nMonth++;
	}

	Tm.tm_mon = nMonth;
	Tm.tm_mday = nTime + 1;

	return &Tm;
}

time_t mktime(struct tm *pTm) {
	time_t nResult = 0;

	if (pTm == nullptr) {
		return -1;
	}

	if (pTm->tm_year < 70 || pTm->tm_year > 139) {
		return -1;
	}

	int nYear;

	for (nYear = 1970; nYear < 1900 + pTm->tm_year; nYear++) {
		nResult += isleapyear(nYear) ? 366 : 365;
	}

	if (pTm->tm_mon < 0 || pTm->tm_mon > 11) {
		return -1;
	}

	int nMonth;

	for (nMonth = 0; nMonth < pTm->tm_mon; nMonth++) {
		nResult += getdaysofmonth(nMonth, pTm->tm_year);
	}

	if (pTm->tm_mday < 1 || pTm->tm_mday > getdaysofmonth(pTm->tm_mon, pTm->tm_year)) {
		return -1;
	}

	nResult += pTm->tm_mday - 1;
	nResult *= 24;

	if (pTm->tm_hour < 0 || pTm->tm_hour > 23) {
		return -1;
	}

	nResult += pTm->tm_hour;
	nResult *= 60;

	if (pTm->tm_min < 0 || pTm->tm_min > 59) {
		return -1;
	}

	nResult += pTm->tm_min;
	nResult *= 60;

	if (pTm->tm_sec < 0 || pTm->tm_sec > 59) {
		return -1;
	}

	nResult += pTm->tm_sec;

	return nResult;
}
}
