// Display mode engine of the WordClock24h firmware.
//
// Port of tables_fill_words() and display_temperature() from wordclock24h
// v3.1.5 (src/tables/tables.c, src/display/display.c),
// https://github.com/ukw100/wordclock24h
// Copyright (c) 2014-2026 Frank Meyer - frank(at)uclock.de
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include <stdint.h>
#include <string.h>

#include "Wc24hTablesDE.h"

namespace wc24h {

// The last table entry ("Temperatur") holds the temperature words, it is not a
// way of telling the time.
constexpr uint8_t TEMPERATURE_MODE = DISPLAY_MODES_COUNT - 1;
constexpr uint8_t CLOCK_MODES_COUNT = DISPLAY_MODES_COUNT - 1;

// Lowest/highest temperature the word table can express, in half degrees.
constexpr uint8_t TEMPERATURE_INDEX_MIN = 20; // 10.0 degrees Celsius
constexpr uint8_t TEMPERATURE_INDEX_MAX = 79; // 39.5 degrees Celsius

using WordSet = bool[WP_COUNT];

inline const char *modeName(uint8_t mode) {
    return mode < DISPLAY_MODES_COUNT ? tbl_modes[mode].description : "";
}

inline void fillWords(uint8_t mode, uint8_t hh, uint8_t mm, WordSet &words) {
    memset(words, 0, sizeof(WordSet));

    if (mode >= CLOCK_MODES_COUNT || hh >= HOUR_COUNT || mm >= MINUTE_COUNT) {
        return;
    }

    const uint8_t(&hours)[HOUR_COUNT][MAX_HOUR_WORDS] =
        tbl_hours[tbl_modes[mode].hour_idx];
    const MINUTEDISPLAY &minute = tbl_minutes[tbl_modes[mode].minute_idx][mm];

    for (uint8_t idx = 0;
         idx < MAX_MINUTE_WORDS && minute.word_idx[idx] != WP_END_OF_WORDS;
         idx++) {
        words[minute.word_idx[idx]] = true;
    }

    if (minute.flags & MDF_HOUR_OFFSET_1) {
        hh += 1;
    } else if (minute.flags & MDF_HOUR_OFFSET_2) {
        hh += 2;
    }

    const bool isMidnight = (hh == 0 || hh == 24);

    while (hh >= HOUR_COUNT) {
        hh -= HOUR_COUNT;
    }

    const uint8_t *wordIdx = hours[hh];

    for (uint8_t idx = 0;
         idx < MAX_HOUR_WORDS && wordIdx[idx] != WP_END_OF_WORDS; idx++) {
        // A marker is followed by two candidate words, e.g. "EIN" (full hour)
        // and "EINS": the first one is taken at minute 0 resp. at midnight.
        if (wordIdx[idx] == WP_IF_MINUTE_IS_0 ||
            wordIdx[idx] == WP_IF_HOUR_IS_0) {
            if (idx + 2 >= MAX_HOUR_WORDS) {
                break;
            }
            const bool takeFirst =
                wordIdx[idx] == WP_IF_MINUTE_IS_0 ? mm == 0 : isMidnight;
            words[wordIdx[takeFirst ? idx + 1 : idx + 2]] = true;
            idx += 2;
        } else {
            words[wordIdx[idx]] = true;
        }
    }
}

// temperatureIndex is the temperature in half degrees Celsius. Returns false
// when the temperature is outside of what the word table can express.
inline bool fillTemperatureWords(uint8_t temperatureIndex, WordSet &words) {
    memset(words, 0, sizeof(WordSet));

    if (temperatureIndex < TEMPERATURE_INDEX_MIN ||
        temperatureIndex > TEMPERATURE_INDEX_MAX) {
        return false;
    }

    const MINUTEDISPLAY &entry =
        tbl_minutes[tbl_modes[TEMPERATURE_MODE].minute_idx]
                   [temperatureIndex - TEMPERATURE_INDEX_MIN];

    for (uint8_t idx = 0;
         idx < MAX_MINUTE_WORDS && entry.word_idx[idx] != WP_END_OF_WORDS;
         idx++) {
        words[entry.word_idx[idx]] = true;
    }
    return true;
}

} // namespace wc24h
