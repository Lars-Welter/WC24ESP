// Night timers of the WordClock24h firmware.
//
// Port of night_check_night_times() from wordclock24h v3.1.5
// (src/night/night.c), https://github.com/ukw100/wordclock24h
// Copyright (c) 2014-2026 Frank Meyer - frank(at)uclock.de
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include <stdint.h>

namespace wc24h {

constexpr uint8_t NIGHT_FLAG_ACTIVE = 0x80;
constexpr uint8_t NIGHT_FLAG_SWITCH_ON = 0x40;
constexpr uint8_t NIGHT_FROM_DAY_MASK = 0x38;
constexpr uint8_t NIGHT_TO_DAY_MASK = 0x07;

// wday: 0 = Sunday ... 6 = Saturday. A range may wrap around the weekend,
// e.g. Saturday-Wednesday (6-3).
inline bool nightTimerCoversWeekday(uint8_t flags, uint8_t wday) {
    const uint8_t fromDay = (flags & NIGHT_FROM_DAY_MASK) >> 3;
    const uint8_t toDay = flags & NIGHT_TO_DAY_MASK;

    if (fromDay == toDay) {
        return wday == fromDay;
    }
    if (fromDay < toDay) {
        return wday >= fromDay && wday <= toDay;
    }
    return !(wday > toDay && wday < fromDay);
}

// Returns true when the timer wants to switch the display at this minute,
// i.e. it is active, due, covers the weekday and would change the state.
inline bool nightTimerFires(uint8_t flags, uint8_t hour, uint8_t minute,
                            bool powerIsOn, uint8_t wday, uint8_t nowHour,
                            uint8_t nowMinute) {
    if (!(flags & NIGHT_FLAG_ACTIVE) || hour != nowHour ||
        minute != nowMinute) {
        return false;
    }
    const bool switchOn = flags & NIGHT_FLAG_SWITCH_ON;
    if (switchOn == powerIsOn) {
        return false;
    }
    return nightTimerCoversWeekday(flags, wday);
}

} // namespace wc24h
