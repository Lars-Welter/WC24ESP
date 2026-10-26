// Games and timers on the 16x18 front of the WordClock24h.
//
// The original wordclock24h firmware (src/tetris/tetris.c, snake.c) could
// play Tetris and Snake on the letter matrix. This is a compact rewrite of
// both games for the ESP32 port, plus a countdown / stopwatch. The engines
// only keep state and rules; ClockWork draws them and feeds the timing.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include <stdint.h>
#include <string.h>

namespace wc24h {

constexpr uint8_t GAME_ROWS = 16;
constexpr uint8_t GAME_COLS = 18;

enum Game : uint8_t {
    GAME_NONE = 0,
    GAME_TETRIS = 1,
    GAME_SNAKE = 2,
};

enum GameKey : uint8_t {
    GAME_KEY_NONE = 0,
    GAME_KEY_LEFT = 1,
    GAME_KEY_RIGHT = 2,
    GAME_KEY_UP = 3,
    GAME_KEY_DOWN = 4,
    GAME_KEY_ACTION = 5,
    GAME_KEY_PAUSE = 6,
};

// xorshift32, good enough for falling blocks and food.
struct GameRng {
    uint32_t state = 1;

    void seed(uint32_t value) { state = value ? value : 0x9E3779B9u; }

    uint32_t next() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }

    uint32_t below(uint32_t limit) { return limit ? next() % limit : 0; }
};

//------------------------------------------------------------------------------
// Tetris: a 10 column well centered on the 18 columns, 16 rows high.
//------------------------------------------------------------------------------

constexpr uint8_t TETRIS_COLS = 10;
constexpr uint8_t TETRIS_ROWS = GAME_ROWS;
constexpr uint8_t TETRIS_OFFSET = (GAME_COLS - TETRIS_COLS) / 2;
constexpr uint8_t TETRIS_PIECES = 7;

// Four rotations per piece as 4x4 bitmaps, bit 15 is the top left cell.
// Order: I, J, L, O, S, T, Z.
static const uint16_t TETRIS_SHAPES[TETRIS_PIECES][4] = {
    {0x0F00, 0x2222, 0x00F0, 0x4444}, {0x44C0, 0x8E00, 0x6440, 0x0E20},
    {0x4460, 0x0E80, 0xC440, 0x2E00}, {0xCC00, 0xCC00, 0xCC00, 0xCC00},
    {0x06C0, 0x8C40, 0x6C00, 0x4620}, {0x0E40, 0x4C40, 0x4E00, 0x4640},
    {0x0C60, 0x4C80, 0xC600, 0x2640},
};

// Points for 1..4 cleared lines, multiplied by the level.
static const uint16_t TETRIS_LINE_POINTS[5] = {0, 40, 100, 300, 1200};

inline bool tetrisShapeCell(uint16_t shape, uint8_t row, uint8_t col) {
    return shape & (0x8000u >> (row * 4 + col));
}

struct Tetris {
    // 0 = empty, 1..7 = piece + 1 (used as color index)
    uint8_t board[TETRIS_ROWS][TETRIS_COLS];
    uint8_t piece = 0;
    uint8_t rotation = 0;
    uint8_t nextPiece = 0;
    int8_t x = 0;
    int8_t y = 0;
    uint32_t score = 0;
    uint16_t lines = 0;
    bool over = false;
    GameRng rng;

    void start(uint32_t seed) {
        memset(board, 0, sizeof board);
        score = 0;
        lines = 0;
        over = false;
        rng.seed(seed);
        nextPiece = rng.below(TETRIS_PIECES);
        spawn();
    }

    uint8_t level() const { return lines / 10 + 1; }

    // Gravity speeds up by 50 ms every level, down to 120 ms.
    uint16_t intervalMs() const {
        const uint16_t step = (level() - 1) * 50;
        return step >= 480 ? 120 : 600 - step;
    }

    bool fits(uint8_t p, uint8_t r, int8_t px, int8_t py) const {
        const uint16_t shape = TETRIS_SHAPES[p][r];
        for (uint8_t row = 0; row < 4; row++) {
            for (uint8_t col = 0; col < 4; col++) {
                if (!tetrisShapeCell(shape, row, col)) {
                    continue;
                }
                const int8_t bx = px + col;
                const int8_t by = py + row;
                if (bx < 0 || bx >= TETRIS_COLS || by >= TETRIS_ROWS) {
                    return false;
                }
                if (by >= 0 && board[by][bx]) {
                    return false;
                }
            }
        }
        return true;
    }

    void spawn() {
        piece = nextPiece;
        nextPiece = rng.below(TETRIS_PIECES);
        rotation = 0;
        x = (TETRIS_COLS - 4) / 2;
        y = 0;
        if (!fits(piece, rotation, x, y)) {
            over = true;
        }
    }

    bool move(int8_t dx) {
        if (over || !fits(piece, rotation, x + dx, y)) {
            return false;
        }
        x += dx;
        return true;
    }

    // Rotates clockwise, nudging the piece sideways at the walls.
    bool rotate() {
        if (over) {
            return false;
        }
        static const int8_t kicks[] = {0, -1, 1, -2, 2};
        const uint8_t r = (rotation + 1) % 4;
        for (int8_t kick : kicks) {
            if (fits(piece, r, x + kick, y)) {
                rotation = r;
                x += kick;
                return true;
            }
        }
        return false;
    }

    // One row down; locks the piece when it lands. Returns false on landing.
    bool step() {
        if (over) {
            return false;
        }
        if (fits(piece, rotation, x, y + 1)) {
            y++;
            return true;
        }
        lock();
        return false;
    }

    bool softDrop() {
        const bool moved = step();
        if (moved) {
            score += 1;
        }
        return moved;
    }

    void hardDrop() {
        while (!over && fits(piece, rotation, x, y + 1)) {
            y++;
            score += 2;
        }
        if (!over) {
            lock();
        }
    }

    void lock() {
        const uint16_t shape = TETRIS_SHAPES[piece][rotation];
        for (uint8_t row = 0; row < 4; row++) {
            for (uint8_t col = 0; col < 4; col++) {
                const int8_t by = y + row;
                if (tetrisShapeCell(shape, row, col) && by >= 0) {
                    board[by][x + col] = piece + 1;
                }
            }
        }
        clearLines();
        spawn();
    }

    void clearLines() {
        uint8_t cleared = 0;
        for (int8_t row = TETRIS_ROWS - 1; row >= 0; row--) {
            bool full = true;
            for (uint8_t col = 0; col < TETRIS_COLS; col++) {
                if (!board[row][col]) {
                    full = false;
                    break;
                }
            }
            if (!full) {
                continue;
            }
            memmove(board[1], board[0], row * TETRIS_COLS);
            memset(board[0], 0, TETRIS_COLS);
            cleared++;
            row++; // the row above moved down, check it again
        }
        if (cleared) {
            score += TETRIS_LINE_POINTS[cleared] * level();
            lines += cleared;
        }
    }

    // Color index of a well cell including the falling piece, 0 = empty.
    uint8_t cell(uint8_t row, uint8_t col) const {
        if (board[row][col]) {
            return board[row][col];
        }
        if (over) {
            return 0;
        }
        const int8_t dr = row - y;
        const int8_t dc = col - x;
        if (dr >= 0 && dr < 4 && dc >= 0 && dc < 4 &&
            tetrisShapeCell(TETRIS_SHAPES[piece][rotation], dr, dc)) {
            return piece + 1;
        }
        return 0;
    }

    void input(uint8_t key) {
        switch (key) {
        case GAME_KEY_LEFT:
            move(-1);
            break;
        case GAME_KEY_RIGHT:
            move(1);
            break;
        case GAME_KEY_UP:
            rotate();
            break;
        case GAME_KEY_DOWN:
            softDrop();
            break;
        case GAME_KEY_ACTION:
            hardDrop();
            break;
        default:
            break;
        }
    }
};

//------------------------------------------------------------------------------
// Snake: uses the full 16x18 matrix, the walls end the game.
//------------------------------------------------------------------------------

constexpr uint16_t SNAKE_CELLS = GAME_ROWS * GAME_COLS;

struct Snake {
    // Ring buffer of cell numbers (row * GAME_COLS + col), head at `head`.
    uint16_t body[SNAKE_CELLS];
    uint16_t head = 0;
    uint16_t length = 0;
    uint8_t direction = GAME_KEY_RIGHT;
    uint8_t nextDirection = GAME_KEY_RIGHT;
    uint16_t food = 0;
    uint32_t score = 0;
    bool over = false;
    bool won = false;
    GameRng rng;

    void start(uint32_t seed) {
        rng.seed(seed);
        length = 3;
        head = length - 1;
        const uint8_t row = GAME_ROWS / 2;
        for (uint16_t i = 0; i < length; i++) {
            body[i] = row * GAME_COLS + 2 + i;
        }
        direction = GAME_KEY_RIGHT;
        nextDirection = GAME_KEY_RIGHT;
        score = 0;
        over = false;
        won = false;
        placeFood();
    }

    // Starts at 250 ms per step and gets faster with every 3 eaten fruits.
    uint16_t intervalMs() const {
        const uint16_t step = (length - 3) / 3 * 10;
        return step >= 150 ? 100 : 250 - step;
    }

    uint16_t segment(uint16_t i) const {
        return body[(head + SNAKE_CELLS - i) % SNAKE_CELLS];
    }

    bool occupies(uint16_t cellNumber) const {
        for (uint16_t i = 0; i < length; i++) {
            if (segment(i) == cellNumber) {
                return true;
            }
        }
        return false;
    }

    void placeFood() {
        if (length >= SNAKE_CELLS) {
            won = true;
            over = true;
            return;
        }
        // Pick the n-th free cell so this always finishes.
        uint16_t n = rng.below(SNAKE_CELLS - length);
        for (uint16_t c = 0; c < SNAKE_CELLS; c++) {
            if (occupies(c)) {
                continue;
            }
            if (n-- == 0) {
                food = c;
                return;
            }
        }
    }

    static bool opposite(uint8_t a, uint8_t b) {
        return (a == GAME_KEY_LEFT && b == GAME_KEY_RIGHT) ||
               (a == GAME_KEY_RIGHT && b == GAME_KEY_LEFT) ||
               (a == GAME_KEY_UP && b == GAME_KEY_DOWN) ||
               (a == GAME_KEY_DOWN && b == GAME_KEY_UP);
    }

    void input(uint8_t key) {
        if (key >= GAME_KEY_LEFT && key <= GAME_KEY_DOWN &&
            !opposite(direction, key)) {
            nextDirection = key;
        }
    }

    // Moves one cell. Returns true when the snake ate.
    bool step() {
        if (over) {
            return false;
        }
        direction = nextDirection;
        const uint16_t current = segment(0);
        int8_t row = current / GAME_COLS;
        int8_t col = current % GAME_COLS;
        switch (direction) {
        case GAME_KEY_LEFT:
            col--;
            break;
        case GAME_KEY_RIGHT:
            col++;
            break;
        case GAME_KEY_UP:
            row--;
            break;
        default:
            row++;
            break;
        }
        if (row < 0 || row >= GAME_ROWS || col < 0 || col >= GAME_COLS) {
            over = true;
            return false;
        }
        const uint16_t next = row * GAME_COLS + col;
        const bool eats = next == food;
        // The tail moves away this step unless the snake grows.
        for (uint16_t i = 0; i < length - (eats ? 0 : 1); i++) {
            if (segment(i) == next) {
                over = true;
                return false;
            }
        }
        head = (head + 1) % SNAKE_CELLS;
        body[head] = next;
        if (eats) {
            length++;
            score += 10;
            placeFood();
        }
        return eats;
    }
};

//------------------------------------------------------------------------------
// Countdown and stopwatch, shown as MM:SS.
//------------------------------------------------------------------------------

enum TimerMode : uint8_t {
    TIMER_OFF = 0,
    TIMER_COUNTDOWN = 1,
    TIMER_STOPWATCH = 2,
};

enum TimerAction : uint8_t {
    TIMER_ACTION_NONE = 0,
    TIMER_ACTION_COUNTDOWN = 1,
    TIMER_ACTION_STOPWATCH = 2,
    TIMER_ACTION_PAUSE = 3,
    TIMER_ACTION_STOP = 4,
};

constexpr uint32_t TIMER_MAX_SECONDS = 99 * 60 + 59;

struct CountTimer {
    uint8_t mode = TIMER_OFF;
    bool running = false;
    uint32_t targetSeconds = 0;
    uint32_t elapsedMs = 0; // until the last pause
    uint32_t startMillis = 0;

    void startCountdown(uint32_t seconds, uint32_t now) {
        mode = TIMER_COUNTDOWN;
        targetSeconds = seconds > TIMER_MAX_SECONDS ? TIMER_MAX_SECONDS
                                                    : seconds;
        elapsedMs = 0;
        startMillis = now;
        running = true;
    }

    void startStopwatch(uint32_t now) {
        mode = TIMER_STOPWATCH;
        targetSeconds = 0;
        elapsedMs = 0;
        startMillis = now;
        running = true;
    }

    void togglePause(uint32_t now) {
        if (mode == TIMER_OFF) {
            return;
        }
        if (running) {
            elapsedMs += now - startMillis;
            running = false;
        } else {
            startMillis = now;
            running = true;
        }
    }

    void stop() {
        mode = TIMER_OFF;
        running = false;
    }

    uint32_t elapsed(uint32_t now) const {
        return elapsedMs + (running ? now - startMillis : 0);
    }

    bool finished(uint32_t now) const {
        return mode == TIMER_COUNTDOWN &&
               elapsed(now) >= targetSeconds * 1000u;
    }

    // Seconds to show: remaining for a countdown (rounded up so it reads
    // 00:01 until the very end), elapsed for the stopwatch.
    uint32_t displaySeconds(uint32_t now) const {
        const uint32_t ms = elapsed(now);
        if (mode == TIMER_COUNTDOWN) {
            const uint32_t target = targetSeconds * 1000u;
            return ms >= target ? 0 : (target - ms + 999) / 1000;
        }
        const uint32_t seconds = ms / 1000;
        return seconds > TIMER_MAX_SECONDS ? TIMER_MAX_SECONDS : seconds;
    }
};

} // namespace wc24h
