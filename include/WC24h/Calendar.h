// Calendar helpers for the overlays "days until" and "moon phase".
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include "Overlay.h"

#include <math.h>
#include <stdint.h>

namespace wc24h {

// Days from today until the next month/day (0 = today). A date that has
// already passed this year counts towards next year.
inline int32_t daysUntil(int32_t year, uint8_t month, uint8_t day,
                         uint8_t targetMonth, uint8_t targetDay) {
    const int32_t today = daysFromCivil(year, month, day);
    int32_t target = daysFromCivil(year, targetMonth, targetDay);
    if (target < today) {
        target = daysFromCivil(year + 1, targetMonth, targetDay);
    }
    return target - today;
}

// Age of the moon as 0..1 of a synodic month: 0 = new, 0.5 = full moon.
// Reference new moon 2000-01-06 18:14 UTC; good to a few hours.
inline float moonPhase(int32_t year, uint8_t month, uint8_t day) {
    const double synodic = 29.530588853;
    const double days = daysFromCivil(year, month, day) -
                        daysFromCivil(2000, 1, 6) - 0.76 + 0.5;
    double phase = fmod(days / synodic, 1.0);
    if (phase < 0) {
        phase += 1.0;
    }
    return static_cast<float>(phase);
}

constexpr uint8_t MOON_SIZE = 12;

// Writes the moon as icon color digits ('0' dark, '7' white, '9' grey for
// the unlit part) into out[MOON_SIZE * MOON_SIZE + 1].
inline void moonIcon(float phase, char *out) {
    const float center = (MOON_SIZE - 1) / 2.f;
    const float radius = MOON_SIZE / 2.f - 0.5f;
    // Terminator position from -1 (left edge) to 1 (right edge).
    const float k = cosf(phase * 6.2831853f);
    for (uint8_t r = 0; r < MOON_SIZE; r++) {
        for (uint8_t c = 0; c < MOON_SIZE; c++) {
            const float dx = (c - center) / radius;
            const float dy = (r - center) / radius;
            char color = '0';
            if (dx * dx + dy * dy <= 1.f) {
                const float half = sqrtf(1.f - dy * dy);
                // Waxing: lit from the right; waning: from the left.
                const float x = phase < 0.5f ? dx : -dx;
                const bool lit = x >= k * half;
                color = lit ? '7' : '9';
            }
            out[r * MOON_SIZE + c] = color;
        }
    }
    out[MOON_SIZE * MOON_SIZE] = '\0';
}

} // namespace wc24h
