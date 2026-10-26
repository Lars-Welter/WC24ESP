// Mood animations for the 16x18 front: lava lamp, fire, snow, fireworks,
// starry sky, rain, plus a sunrise and a flash used by the alarm clock.
//
// Each effect renders a full color frame from its own state and the time;
// ClockWork scales it to the brightness and sends it to the LEDs.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include "Games.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

namespace wc24h {

enum AmbientEffect : uint8_t {
    AMBIENT_NONE = 0,
    AMBIENT_LAVA = 1,
    AMBIENT_FIRE = 2,
    AMBIENT_SNOW = 3,
    AMBIENT_FIREWORKS = 4,
    AMBIENT_STARS = 5,
    AMBIENT_RAIN = 6,
    AMBIENT_SUNRISE = 7, // alarm: slowly brighter, red to warm white
    AMBIENT_FLASH = 8,   // alarm: blinking
    AMBIENT_COUNT
};

struct Rgb {
    uint8_t r, g, b;
};

struct RgbFrame {
    Rgb px[GAME_ROWS][GAME_COLS];

    void clear() { memset(px, 0, sizeof px); }

    void set(int16_t row, int16_t col, Rgb c) {
        if (row >= 0 && row < GAME_ROWS && col >= 0 && col < GAME_COLS) {
            px[row][col] = c;
        }
    }

    // Brightens towards c, used for overlapping sparks.
    void add(int16_t row, int16_t col, Rgb c) {
        if (row < 0 || row >= GAME_ROWS || col < 0 || col >= GAME_COLS) {
            return;
        }
        Rgb &p = px[row][col];
        p.r = p.r + c.r > 255 ? 255 : p.r + c.r;
        p.g = p.g + c.g > 255 ? 255 : p.g + c.g;
        p.b = p.b + c.b > 255 ? 255 : p.b + c.b;
    }
};

inline Rgb scaleRgb(Rgb c, float f) {
    if (f < 0.f) {
        f = 0.f;
    }
    if (f > 1.f) {
        f = 1.f;
    }
    return {static_cast<uint8_t>(c.r * f), static_cast<uint8_t>(c.g * f),
            static_cast<uint8_t>(c.b * f)};
}

// hue 0..1, full saturation and value
inline Rgb hueRgb(float h) {
    h -= floorf(h);
    const float x = h * 6.f;
    const uint8_t sector = static_cast<uint8_t>(x);
    const uint8_t up = static_cast<uint8_t>((x - sector) * 255);
    const uint8_t down = 255 - up;
    switch (sector) {
    case 0:
        return {255, up, 0};
    case 1:
        return {down, 255, 0};
    case 2:
        return {0, 255, up};
    case 3:
        return {0, down, 255};
    case 4:
        return {up, 0, 255};
    default:
        return {255, 0, down};
    }
}

class Ambient {
public:
    uint8_t effect = AMBIENT_NONE;

    void start(uint8_t newEffect, uint32_t seed, uint32_t now) {
        effect = newEffect < AMBIENT_COUNT ? newEffect
                                           : static_cast<uint8_t>(AMBIENT_LAVA);
        rng.seed(seed);
        startMs = lastMs = now;
        memset(heat, 0, sizeof heat);
        memset(drops, 0, sizeof drops);
        memset(sparks, 0, sizeof sparks);
        memset(stars, 0, sizeof stars);
        for (Star &s : stars) {
            s.row = rng.below(GAME_ROWS);
            s.col = rng.below(GAME_COLS);
            s.phase = rng.below(1000);
            s.speed = 400 + rng.below(1600);
        }
        for (Drop &d : drops) {
            d.active = false;
        }
    }

    // Frame time of the effect in ms.
    uint16_t frameMs() const {
        switch (effect) {
        case AMBIENT_FIRE:
            return 60;
        case AMBIENT_FIREWORKS:
        case AMBIENT_RAIN:
            return 50;
        case AMBIENT_SNOW:
            return 120;
        default:
            return 80;
        }
    }

    void render(RgbFrame &f, uint32_t now) {
        const uint32_t t = now - startMs;
        const uint32_t dt = now - lastMs;
        lastMs = now;
        switch (effect) {
        case AMBIENT_LAVA:
            lava(f, t);
            break;
        case AMBIENT_FIRE:
            fire(f);
            break;
        case AMBIENT_SNOW:
            snow(f, dt);
            break;
        case AMBIENT_FIREWORKS:
            fireworks(f, dt);
            break;
        case AMBIENT_STARS:
            starSky(f, t);
            break;
        case AMBIENT_RAIN:
            rain(f, dt);
            break;
        case AMBIENT_SUNRISE:
            sunrise(f, t);
            break;
        case AMBIENT_FLASH:
            flash(f, t);
            break;
        default:
            f.clear();
            break;
        }
    }

private:
    GameRng rng;
    uint32_t startMs = 0, lastMs = 0;

    // Fire
    uint8_t heat[GAME_ROWS + 1][GAME_COLS];

    // Snow, rain and fireworks particles, coordinates in 1/16 cells
    struct Drop {
        bool active;
        int16_t y, x;
        int16_t vy, vx;
        uint8_t life;
        Rgb color;
    };
    static constexpr uint8_t MAX_DROPS = 40;
    Drop drops[MAX_DROPS];
    Drop sparks[MAX_DROPS];

    struct Star {
        uint8_t row, col;
        uint16_t phase, speed;
    };
    Star stars[24];

    // Soft blobs drifting on sine paths, colors slowly cycling.
    void lava(RgbFrame &f, uint32_t t) {
        const float ts = t / 1000.f;
        struct Blob {
            float x, y, r;
        } blobs[3];
        for (uint8_t i = 0; i < 3; i++) {
            blobs[i].x = 8.5f + 6.f * sinf(ts * (0.21f + 0.07f * i) + i * 2.1f);
            blobs[i].y = 7.5f + 6.5f * sinf(ts * (0.17f + 0.05f * i) + i * 1.3f);
            blobs[i].r = 3.2f + 0.8f * sinf(ts * 0.4f + i);
        }
        const Rgb a = hueRgb(0.95f + 0.05f * sinf(ts * 0.05f)); // red/pink
        const Rgb b = hueRgb(0.08f + 0.04f * sinf(ts * 0.07f)); // orange
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                float sum = 0.f;
                for (const Blob &bl : blobs) {
                    const float dx = c - bl.x, dy = r - bl.y;
                    sum += bl.r * bl.r / (dx * dx + dy * dy + 1.f);
                }
                if (sum < 0.6f) {
                    f.px[r][c] = scaleRgb(a, 0.08f);
                } else {
                    const float k = sum > 2.f ? 1.f : (sum - 0.6f) / 1.4f;
                    f.px[r][c] = {
                        static_cast<uint8_t>(a.r + (b.r - a.r) * k),
                        static_cast<uint8_t>(a.g + (b.g - a.g) * k),
                        static_cast<uint8_t>(a.b + (b.b - a.b) * k)};
                }
            }
        }
    }

    // Classic heat diffusion: hot sparks at the bottom, cooling upwards.
    void fire(RgbFrame &f) {
        for (uint8_t c = 0; c < GAME_COLS; c++) {
            heat[GAME_ROWS][c] = 160 + rng.below(96);
        }
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                const uint8_t left = c ? heat[r + 1][c - 1] : heat[r + 1][c];
                const uint8_t right =
                    c + 1 < GAME_COLS ? heat[r + 1][c + 1] : heat[r + 1][c];
                const uint16_t avg =
                    (left + right + 2 * heat[r + 1][c]) / 4;
                const uint8_t cool = 8 + rng.below(18);
                heat[r][c] = avg > cool ? avg - cool : 0;
            }
        }
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                const uint8_t h = heat[r][c];
                // black -> red -> orange -> yellow
                Rgb col;
                if (h < 85) {
                    col = {static_cast<uint8_t>(h * 3), 0, 0};
                } else if (h < 170) {
                    col = {255, static_cast<uint8_t>((h - 85) * 2), 0};
                } else {
                    col = {255, static_cast<uint8_t>(170 + (h - 170)),
                           static_cast<uint8_t>((h - 170) * 2)};
                }
                f.px[r][c] = col;
            }
        }
    }

    void moveDrops(Drop *list, uint32_t dt) {
        const int16_t steps = dt / 20 ? dt / 20 : 1;
        for (uint8_t i = 0; i < MAX_DROPS; i++) {
            Drop &d = list[i];
            if (!d.active) {
                continue;
            }
            for (int16_t s = 0; s < steps; s++) {
                d.y += d.vy;
                d.x += d.vx;
            }
            if (d.y >= GAME_ROWS * 16 || d.x < -16 || d.x >= GAME_COLS * 16) {
                d.active = false;
            }
        }
    }

    void spawnDrop(Drop *list, int16_t vy, int16_t vx, Rgb color) {
        for (uint8_t i = 0; i < MAX_DROPS; i++) {
            if (!list[i].active) {
                list[i] = {true, 0, static_cast<int16_t>(rng.below(GAME_COLS) * 16),
                           vy, vx, 255, color};
                return;
            }
        }
    }

    void snow(RgbFrame &f, uint32_t dt) {
        if (rng.below(3) == 0) {
            spawnDrop(drops, 3 + rng.below(3), 0, {200, 200, 255});
        }
        moveDrops(drops, dt);
        f.clear();
        for (Drop &d : drops) {
            if (!d.active) {
                continue;
            }
            // gentle sideways drift
            if (rng.below(6) == 0) {
                d.x += rng.below(2) ? 16 : -16;
            }
            f.set(d.y / 16, d.x / 16, d.color);
        }
    }

    void rain(RgbFrame &f, uint32_t dt) {
        for (uint8_t i = 0; i < 2; i++) {
            spawnDrop(drops, 12 + rng.below(8), 0, {0, 200, 60});
        }
        moveDrops(drops, dt);
        // fading trails
        for (auto &row : f.px) {
            for (Rgb &p : row) {
                p = scaleRgb(p, 0.6f);
            }
        }
        for (const Drop &d : drops) {
            if (d.active) {
                f.set(d.y / 16, d.x / 16, {120, 255, 140});
            }
        }
    }

    void fireworks(RgbFrame &f, uint32_t dt) {
        // A rocket bursts into sparks at a random point.
        if (rng.below(18) == 0) {
            const Rgb color = hueRgb(rng.below(100) / 100.f);
            const int16_t y = (2 + rng.below(8)) * 16;
            const int16_t x = (3 + rng.below(GAME_COLS - 6)) * 16;
            for (uint8_t i = 0; i < 14; i++) {
                const float a = i * 6.2832f / 14;
                for (uint8_t j = 0; j < MAX_DROPS; j++) {
                    if (!sparks[j].active) {
                        sparks[j] = {true, y, x,
                                     static_cast<int16_t>(sinf(a) * 9),
                                     static_cast<int16_t>(cosf(a) * 9), 255,
                                     color};
                        break;
                    }
                }
            }
        }
        const int16_t steps = dt / 20 ? dt / 20 : 1;
        for (Drop &s : sparks) {
            if (!s.active) {
                continue;
            }
            for (int16_t i = 0; i < steps; i++) {
                s.y += s.vy;
                s.x += s.vx;
                s.vy += 1; // gravity
            }
            s.life = s.life > 14 ? s.life - 14 : 0;
            if (!s.life || s.y >= GAME_ROWS * 16) {
                s.active = false;
            }
        }
        for (auto &row : f.px) {
            for (Rgb &p : row) {
                p = scaleRgb(p, 0.55f);
            }
        }
        for (const Drop &s : sparks) {
            if (s.active) {
                f.add(s.y / 16, s.x / 16, scaleRgb(s.color, s.life / 255.f));
            }
        }
    }

    void starSky(RgbFrame &f, uint32_t t) {
        f.clear();
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                f.px[r][c] = {0, 0, static_cast<uint8_t>(3 + r / 2)};
            }
        }
        for (const Star &s : stars) {
            const float k =
                0.5f + 0.5f * sinf((t + s.phase) * 6.2832f / s.speed);
            f.set(s.row, s.col, scaleRgb({255, 240, 200}, 0.15f + 0.85f * k));
        }
    }

    // 3 minutes from deep red to warm white.
    void sunrise(RgbFrame &f, uint32_t t) {
        const float k = t >= 180000 ? 1.f : t / 180000.f;
        const Rgb c = {255, static_cast<uint8_t>(40 + 180 * k),
                       static_cast<uint8_t>(120 * k * k)};
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            // the sun rises from the bottom
            const float rowLight = k * 1.4f - (GAME_ROWS - 1 - r) / 16.f * 0.6f;
            for (uint8_t col = 0; col < GAME_COLS; col++) {
                f.px[r][col] = scaleRgb(c, 0.05f + rowLight);
            }
        }
    }

    void flash(RgbFrame &f, uint32_t t) {
        const bool on = (t / 400) % 2 == 0;
        const Rgb c = (t / 2400) % 2 ? Rgb{255, 255, 255} : Rgb{255, 30, 0};
        for (auto &row : f.px) {
            for (Rgb &p : row) {
                p = on ? c : Rgb{0, 0, 0};
            }
        }
    }
};

} // namespace wc24h
