// Overlays of the WordClock24h firmware: icons, date, temperature or a text
// shown instead of the time every few minutes, optionally only on some days.
//
// Port of the overlay selection in main.c and the date codes of base.c from
// wordclock24h v3.1.5, https://github.com/ukw100/wordclock24h
// Copyright (c) 2014-2026 Frank Meyer - frank(at)uclock.de
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include <stdint.h>

namespace wc24h {

constexpr uint8_t MAX_OVERLAYS = 8;
constexpr uint8_t OVERLAY_TEXT_LENGTH = 24;

// Values match the overlay types of the original firmware.
enum OverlayType : uint8_t {
    OVERLAY_NONE = 0,
    OVERLAY_ICON = 1,
    OVERLAY_DATE = 2,
    OVERLAY_TEMPERATURE = 3,
    OVERLAY_WEATHER_ICON = 4,
    OVERLAY_TICKER = 6,
    OVERLAY_TEMPERATURE_DIGITS = 10,
};

enum OverlayDateCode : uint8_t {
    DATE_ALWAYS = 0,
    DATE_FIXED = 1,
    DATE_CARNIVAL_MONDAY = 2,
    DATE_EASTER_SUNDAY = 3,
    DATE_ADVENT1 = 4,
    DATE_ADVENT2 = 5,
    DATE_ADVENT3 = 6,
    DATE_ADVENT4 = 7,
    DATE_CODE_COUNT
};

constexpr uint8_t OVERLAY_FLAG_ACTIVE = 0x01;

struct Overlay {
    uint8_t type;
    uint8_t flags;
    uint8_t interval; // shown when the minute is a multiple of it
    uint8_t duration; // seconds, for icons and temperature
    uint8_t dateCode;
    uint8_t month; // start of the date range for DATE_FIXED
    uint8_t day;
    uint8_t days; // length of the date range
    uint8_t icon;
    char text[OVERLAY_TEXT_LENGTH];
};

inline bool isOverlayTypeSupported(uint32_t type) {
    switch (type) {
    case OVERLAY_ICON:
    case OVERLAY_DATE:
    case OVERLAY_TEMPERATURE:
    case OVERLAY_WEATHER_ICON:
    case OVERLAY_TICKER:
    case OVERLAY_TEMPERATURE_DIGITS:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Calendar
//------------------------------------------------------------------------------

// Days since 1970-01-01 of a proleptic Gregorian date.
inline int32_t daysFromCivil(int32_t year, uint8_t month, uint8_t day) {
    year -= month <= 2;
    const int32_t era = (year >= 0 ? year : year - 399) / 400;
    const uint32_t yearOfEra = static_cast<uint32_t>(year - era * 400);
    const uint32_t dayOfYear =
        (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    const uint32_t dayOfEra =
        yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
    return era * 146097 + static_cast<int32_t>(dayOfEra) - 719468;
}

// 0 = Sunday ... 6 = Saturday
inline uint8_t weekdayFromDays(int32_t days) {
    return static_cast<uint8_t>(days >= -4 ? (days + 4) % 7
                                           : (days + 5) % 7 + 6);
}

// Easter Sunday after Gauss, as in get_easter() of the original firmware.
inline int32_t easterSunday(int32_t year) {
    const int32_t a = year % 19;
    const int32_t b = year % 4;
    const int32_t c = year % 7;
    const int32_t k = year / 100;
    const int32_t m = (8 * k + 13) / 25 - 2;
    const int32_t s = k - year / 400 - 2;
    const int32_t bigM = (15 + s - m) % 30;
    const int32_t bigN = (6 + s) % 7;
    const int32_t d = (bigM + 19 * a) % 30;
    const int32_t bigD = (d == 29) ? 28 : (d == 28 && a >= 11) ? 27 : d;
    const int32_t e = (2 * b + 4 * c + 6 * bigD + bigN) % 7;
    // March 22 + D + e
    return daysFromCivil(year, 3, 22) + bigD + e;
}

// First day of the overlay's date range in the given year, in days since
// 1970-01-01. Returns false for overlays without a date range.
inline bool overlayRangeStart(const Overlay &overlay, int32_t year,
                              int32_t &start) {
    switch (overlay.dateCode) {
    case DATE_FIXED:
        if (overlay.month < 1 || overlay.month > 12 || overlay.day < 1 ||
            overlay.day > 31) {
            return false;
        }
        start = daysFromCivil(year, overlay.month, overlay.day);
        return true;
    case DATE_CARNIVAL_MONDAY:
        start = easterSunday(year) - 48;
        return true;
    case DATE_EASTER_SUNDAY:
        start = easterSunday(year);
        return true;
    case DATE_ADVENT1:
    case DATE_ADVENT2:
    case DATE_ADVENT3:
    case DATE_ADVENT4: {
        // The fourth Advent is the last Sunday before Christmas Day.
        const int32_t christmasEve = daysFromCivil(year, 12, 24);
        const int32_t advent4 = christmasEve - weekdayFromDays(christmasEve);
        start = advent4 - 7 * (DATE_ADVENT4 - overlay.dateCode);
        return true;
    }
    default:
        return false;
    }
}

inline bool overlayCoversDate(const Overlay &overlay, int32_t year,
                              uint8_t month, uint8_t day) {
    if (overlay.dateCode == DATE_ALWAYS) {
        return true;
    }
    const int32_t today = daysFromCivil(year, month, day);
    const int32_t length = overlay.days > 0 ? overlay.days : 1;

    // A range may start in the previous year and reach into this one.
    for (int32_t y = year - 1; y <= year; y++) {
        int32_t start;
        if (overlayRangeStart(overlay, y, start) && today >= start &&
            today < start + length) {
            return true;
        }
    }
    return false;
}

// Picks the overlay to show at this minute: among the active, due overlays
// covering today the one with the longest interval wins, on a tie the one
// bound to a date. Returns -1 when none is due.
inline int8_t selectOverlay(const Overlay *overlays, uint8_t count,
                            int32_t year, uint8_t month, uint8_t day,
                            uint8_t minute) {
    int8_t selected = -1;
    uint8_t selectedInterval = 0;

    for (uint8_t i = 0; i < count; i++) {
        const Overlay &overlay = overlays[i];
        if (!(overlay.flags & OVERLAY_FLAG_ACTIVE) ||
            !isOverlayTypeSupported(overlay.type) || overlay.interval == 0 ||
            minute % overlay.interval != 0 ||
            !overlayCoversDate(overlay, year, month, day)) {
            continue;
        }
        if (overlay.interval > selectedInterval ||
            (overlay.interval == selectedInterval &&
             overlay.dateCode != DATE_ALWAYS)) {
            selected = static_cast<int8_t>(i);
            selectedInterval = overlay.interval;
        }
    }
    return selected;
}

} // namespace wc24h
