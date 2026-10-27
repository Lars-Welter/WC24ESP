// Arcade games for the 16x18 letter matrix of the WordClock24h.
//
// Every game draws into a Canvas of palette indices; ClockWork turns that
// into LED colors, feeds the time and passes the web page input along.
// Nothing here depends on Arduino, so test/wc24h_test.cpp can play the
// games on the host.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include "Games.h"
#include "Wc24hDisplay.h"

#include <initializer_list>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace wc24h {

//------------------------------------------------------------------------------
// Game ids, keys, palette
//------------------------------------------------------------------------------

enum ArcadeGameId : uint8_t {
    // GAME_TETRIS = 1, GAME_SNAKE = 2 (Games.h)
    GAME_SIMON = 3,
    GAME_FLAPPY = 4,
    GAME_BREAKOUT = 5,
    GAME_INVADERS = 6,
    GAME_FROGGER = 7,
    GAME_PONG = 8,
    GAME_PONG_CPU = 9,
    GAME_CONNECT4 = 10,
    GAME_CONNECT4_CPU = 11,
    GAME_REACTION = 12,
    GAME_HANGMAN = 13,
    GAME_CLOCK_QUIZ = 14,
    GAME_WORD_FIND = 15,
    GAME_MATH = 16,
    GAME_PAINT = 20,
    GAME_DICE = 21,
    GAME_HOURGLASS = 22,
};

enum ArcadeKey : uint8_t {
    // GAME_KEY_LEFT .. GAME_KEY_PAUSE = 1 .. 6 (Games.h)
    GAME_KEY_COLOR1 = 11, // Simon: red, green, blue, yellow
    GAME_KEY_COLOR2 = 12,
    GAME_KEY_COLOR3 = 13,
    GAME_KEY_COLOR4 = 14,
    GAME_KEY_PAINT_SET = 20, // value = cell * 64 + color
    GAME_KEY_PAINT_FILL = 21, // value = color
    GAME_KEY_PAINT_CLEAR = 22,
    GAME_KEY_PAINT_BRIGHTNESS = 23, // value = 5..100 %
    GAME_KEY_PAINT_SAVE = 24,   // value = picture slot (stored by ClockWork)
    GAME_KEY_PAINT_LOAD = 25,   // value = picture slot
    GAME_KEY_PAINT_DELETE = 26, // value = picture slot
    GAME_KEY_PAINT_PLAY = 27,   // value = ms per picture, 0 stops the flipbook
    GAME_KEY_DICE_COUNT = 28,   // value = 1 or 2 dice
    GAME_KEY_HOURGLASS = 29,    // value = seconds, starts the hourglass
    GAME_KEY_ANSWER = 30, // value = number or hh * 100 + mm
    GAME_KEY_TAP = 31,    // value = cell (row * 18 + col)
    GAME_KEY_LETTER_A = 65, // 65..90 = A..Z
    GAME_KEY_LETTER_Z = 90,
};

enum Palette : uint8_t {
    PAL_OFF = 0,
    PAL_RED,
    PAL_ORANGE,
    PAL_YELLOW,
    PAL_GREEN,
    PAL_CYAN,
    PAL_BLUE,
    PAL_PURPLE,
    PAL_PINK,
    PAL_WHITE,
    PAL_DIM,
    PAL_DARK_RED,
    PAL_DARK_GREEN,
    PAL_DARK_BLUE,
    PAL_DARK_YELLOW,
    PAL_COUNT,
    // Extra colors for the paint board: 12 hues each bright, pastel and
    // dark, then warm white, light grey, dark grey, brown.
    PAL_PAINT_BRIGHT = PAL_COUNT,
    PAL_PAINT_PASTEL = PAL_PAINT_BRIGHT + 12,
    PAL_PAINT_DARK = PAL_PAINT_PASTEL + 12,
    PAL_PAINT_WARM_WHITE = PAL_PAINT_DARK + 12,
    PAL_PAINT_LIGHT_GREY,
    PAL_PAINT_DARK_GREY,
    PAL_PAINT_BROWN,
    PAL_EXT_COUNT
};

struct PaletteHsv {
    float h, s, v;
};

// Color of a palette index as hue/saturation/value (0..1); also used by
// the web page to draw the paint board colors.
inline PaletteHsv paletteHsv(uint8_t index) {
    static const PaletteHsv BASE[PAL_COUNT] = {
        {0.f, 0.f, 0.f},    {0.f, 1.f, 1.f},    {0.07f, 1.f, 1.f},
        {0.15f, 1.f, 1.f},  {0.33f, 1.f, 1.f},  {0.5f, 1.f, 1.f},
        {0.66f, 1.f, 1.f},  {0.78f, 1.f, 1.f},  {0.9f, 0.7f, 1.f},
        {0.f, 0.f, 1.f},    {0.f, 0.f, 0.25f},  {0.f, 1.f, 0.3f},
        {0.33f, 1.f, 0.3f}, {0.66f, 1.f, 0.35f}, {0.15f, 1.f, 0.3f},
    };
    if (index < PAL_COUNT) {
        return BASE[index];
    }
    if (index < PAL_PAINT_PASTEL) {
        return {(index - PAL_PAINT_BRIGHT) / 12.f, 1.f, 1.f};
    }
    if (index < PAL_PAINT_DARK) {
        return {(index - PAL_PAINT_PASTEL) / 12.f, 0.45f, 1.f};
    }
    if (index < PAL_PAINT_WARM_WHITE) {
        return {(index - PAL_PAINT_DARK) / 12.f, 1.f, 0.35f};
    }
    switch (index) {
    case PAL_PAINT_WARM_WHITE:
        return {0.1f, 0.3f, 1.f};
    case PAL_PAINT_LIGHT_GREY:
        return {0.f, 0.f, 0.55f};
    case PAL_PAINT_DARK_GREY:
        return {0.f, 0.f, 0.2f};
    case PAL_PAINT_BROWN:
        return {0.07f, 0.9f, 0.45f};
    default:
        return {0.f, 0.f, 0.f};
    }
}

struct GameInput {
    uint8_t key;
    uint8_t player; // 1 or 2
    uint32_t value;
};

// Shown in the web app.
struct GameStatus {
    uint32_t score = 0;
    int16_t lives = -1;   // -1 = not used
    uint16_t level = 0;   // 0 = not used
    uint8_t score1 = 0;   // two player games
    uint8_t score2 = 0;
    uint8_t winner = 0;   // 0 = none / draw, 1, 2
    uint8_t turn = 0;     // two player games: whose turn
    char text[48] = "";   // question, word pattern, ...
    char text2[48] = "";  // hint, last answer, ...
};

//------------------------------------------------------------------------------
// Canvas with a 3x5 font for numbers
//------------------------------------------------------------------------------

struct Canvas {
    uint8_t cell[GAME_ROWS][GAME_COLS];

    void clear(uint8_t color = PAL_OFF) { memset(cell, color, sizeof cell); }

    void set(int16_t row, int16_t col, uint8_t color) {
        if (row >= 0 && row < GAME_ROWS && col >= 0 && col < GAME_COLS) {
            cell[row][col] = color;
        }
    }

    void fill(int16_t row, int16_t col, int16_t rows, int16_t cols,
              uint8_t color) {
        for (int16_t r = row; r < row + rows; r++) {
            for (int16_t c = col; c < col + cols; c++) {
                set(r, c, color);
            }
        }
    }

    // 3x5 glyphs, three bits per row, bit 2 = left column.
    static const uint8_t *glyph(char ch) {
        static const uint8_t DIGITS[10][5] = {
            {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7},
            {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1}, {7, 4, 7, 1, 7},
            {7, 4, 7, 5, 7}, {7, 1, 2, 2, 2}, {7, 5, 7, 5, 7},
            {7, 5, 7, 1, 7}};
        static const uint8_t PLUS[5] = {0, 2, 7, 2, 0};
        static const uint8_t MINUS[5] = {0, 0, 7, 0, 0};
        static const uint8_t TIMES[5] = {0, 5, 2, 5, 0};
        static const uint8_t DIVIDE[5] = {2, 0, 7, 0, 2};
        static const uint8_t EQUALS[5] = {0, 7, 0, 7, 0};
        static const uint8_t QUESTION[5] = {7, 1, 3, 0, 2};
        static const uint8_t COLON[5] = {0, 2, 0, 2, 0};
        if (ch >= '0' && ch <= '9') {
            return DIGITS[ch - '0'];
        }
        switch (ch) {
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case 'x':
            return TIMES;
        case '/':
            return DIVIDE;
        case '=':
            return EQUALS;
        case '?':
            return QUESTION;
        case ':':
            return COLON;
        default:
            return nullptr;
        }
    }

    static uint8_t textWidth(const char *text) {
        const size_t len = strlen(text);
        return len ? len * 4 - 1 : 0;
    }

    void text(int16_t row, int16_t col, const char *str, uint8_t color) {
        for (; *str; str++, col += 4) {
            const uint8_t *g = glyph(*str);
            if (!g) {
                continue;
            }
            for (uint8_t r = 0; r < 5; r++) {
                for (uint8_t c = 0; c < 3; c++) {
                    if (g[r] & (4 >> c)) {
                        set(row + r, col + c, color);
                    }
                }
            }
        }
    }

    void centeredText(int16_t row, const char *str, uint8_t color) {
        const uint8_t width = textWidth(str);
        text(row, width < GAME_COLS ? (GAME_COLS - width) / 2 : 0, str,
             color);
    }
};

// Up to 8 digits of a score on two lines.
inline void drawScore(Canvas &canvas, uint32_t score, uint8_t color) {
    char buf[12];
    uint8_t len = 0;
    do {
        buf[len++] = '0' + score % 10;
        score /= 10;
    } while (score && len < 8);
    char line1[5] = "", line2[5] = "";
    // reversed digits; the first line takes what does not fit in 4
    uint8_t first = len > 4 ? len - 4 : 0;
    for (uint8_t i = 0; i < first; i++) {
        line1[i] = buf[len - 1 - i];
    }
    line1[first] = '\0';
    for (uint8_t i = 0; i < len - first; i++) {
        line2[i] = buf[len - 1 - first - i];
    }
    line2[len - first] = '\0';
    if (first) {
        canvas.centeredText(2, line1, color);
        canvas.centeredText(9, line2, color);
    } else {
        canvas.centeredText(5, line2, color);
    }
}

//------------------------------------------------------------------------------
// Letters of the front, umlauts as '#' (Ü) and '%' (Ö)
//------------------------------------------------------------------------------

static const char FRONT_LETTERS[GAME_ROWS][GAME_COLS + 1] = {
    "ESAISTOVIERTELEINS", "DREINERSECHSIEBENE", "ELF#NFNEUNVIERACHT",
    "NULLZWEINZW%LFZEHN", "UNDOZWANZIGVIERZIG", "DREISSIGF#NFZIGUHR",
    "MINUTENIVORUNDNACH", "EINDREIVIERTELHALB", "SIEBENEUNULLZWEINE",
    "F#NFSECHSNACHTVIER", "DREINSUNDAELFEZEHN", "ZWANZIGGRADREISSIG",
    "VIERZIGZW%LF#NFZIG", "MINUTENUHREFR#HVOR", "ABENDSMITTERNACHTS",
    "MORGENSWARMMITTAGS",
};

//------------------------------------------------------------------------------
// Base class
//------------------------------------------------------------------------------

class ArcadeGame {
public:
    virtual ~ArcadeGame() {}

    bool over = false;
    bool twoPlayer = false;
    // Single player games can be paused; turn based games do not need it.
    bool pausable = true;

    virtual void start(uint32_t seed, uint32_t now) = 0;
    virtual void input(const GameInput &in, uint32_t now) = 0;
    // Advances the game; returns true when the picture changed.
    virtual bool update(uint32_t now) = 0;
    virtual void render(Canvas &canvas, uint32_t now) const = 0;
    virtual void status(GameStatus &s) const = 0;

protected:
    GameRng rng;
};

//------------------------------------------------------------------------------
// Tetris and Snake (Games.h) behind the same interface
//------------------------------------------------------------------------------

class TetrisGame : public ArcadeGame {
public:
    Tetris t;
    uint32_t lastStep = 0;

    void start(uint32_t seed, uint32_t now) override {
        t.start(seed);
        over = false;
        lastStep = now;
    }
    void input(const GameInput &in, uint32_t) override {
        t.input(in.key);
        over = t.over;
    }
    bool update(uint32_t now) override {
        if (t.over || now - lastStep < t.intervalMs()) {
            return false;
        }
        lastStep = now;
        t.step();
        over = t.over;
        return true;
    }
    void render(Canvas &canvas, uint32_t) const override {
        // I, J, L, O, S, T, Z
        static const uint8_t COLORS[TETRIS_PIECES] = {
            PAL_CYAN, PAL_BLUE, PAL_ORANGE, PAL_YELLOW,
            PAL_GREEN, PAL_PURPLE, PAL_RED};
        canvas.clear();
        canvas.fill(0, TETRIS_OFFSET - 1, GAME_ROWS, 1, PAL_DIM);
        canvas.fill(0, TETRIS_OFFSET + TETRIS_COLS, GAME_ROWS, 1, PAL_DIM);
        for (uint8_t r = 0; r < TETRIS_ROWS; r++) {
            for (uint8_t c = 0; c < TETRIS_COLS; c++) {
                const uint8_t v = t.cell(r, c);
                if (v) {
                    canvas.set(r, TETRIS_OFFSET + c, COLORS[v - 1]);
                }
            }
        }
    }
    void status(GameStatus &s) const override {
        s.score = t.score;
        s.level = t.level();
        snprintf(s.text, sizeof s.text, "%u", t.lines);
    }
};

class SnakeGame : public ArcadeGame {
public:
    Snake s;
    uint32_t lastStep = 0;

    void start(uint32_t seed, uint32_t now) override {
        s.start(seed);
        over = false;
        lastStep = now;
    }
    void input(const GameInput &in, uint32_t) override { s.input(in.key); }
    bool update(uint32_t now) override {
        if (s.over || now - lastStep < s.intervalMs()) {
            return false;
        }
        lastStep = now;
        s.step();
        over = s.over;
        return true;
    }
    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        canvas.set(s.food / GAME_COLS, s.food % GAME_COLS, PAL_RED);
        for (uint16_t i = s.length; i-- > 0;) {
            const uint16_t c = s.segment(i);
            canvas.set(c / GAME_COLS, c % GAME_COLS,
                       i == 0 ? PAL_YELLOW : PAL_GREEN);
        }
    }
    void status(GameStatus &st) const override {
        st.score = s.score;
        snprintf(st.text, sizeof st.text, "%u", s.length);
    }
};

//------------------------------------------------------------------------------
// Simon: remember the color sequence
//------------------------------------------------------------------------------

class SimonGame : public ArcadeGame {
public:
    static constexpr uint8_t MAX_LEN = 64;
    enum Phase : uint8_t { PHASE_SHOW, PHASE_INPUT, PHASE_NEXT };

    uint8_t seq[MAX_LEN];
    uint8_t len = 0;
    uint8_t phase = PHASE_SHOW;
    uint8_t pos = 0;       // shown resp. entered so far
    uint32_t phaseStart = 0;
    int8_t lit = -1;       // quadrant lit right now
    uint32_t litUntil = 0;

    uint16_t onMs() const { return len > 12 ? 250 : 450 - len * 15; }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        len = 1;
        seq[0] = rng.below(4);
        phase = PHASE_SHOW;
        pos = 0;
        phaseStart = now + 600;
        lit = -1;
    }

    void input(const GameInput &in, uint32_t now) override {
        if (phase != PHASE_INPUT || in.key < GAME_KEY_COLOR1 ||
            in.key > GAME_KEY_COLOR4) {
            return;
        }
        const uint8_t color = in.key - GAME_KEY_COLOR1;
        lit = color;
        litUntil = now + 250;
        if (color != seq[pos]) {
            over = true;
            return;
        }
        if (++pos == len) {
            if (len < MAX_LEN) {
                seq[len++] = rng.below(4);
            }
            phase = PHASE_NEXT;
            phaseStart = now + 800;
        }
    }

    bool update(uint32_t now) override {
        const int8_t before = lit;
        if (phase == PHASE_NEXT && now >= phaseStart) {
            phase = PHASE_SHOW;
            pos = 0;
        }
        if (phase == PHASE_SHOW) {
            if (now < phaseStart) {
                lit = -1;
            } else {
                const uint32_t slot = onMs() + 200;
                const uint32_t t = now - phaseStart;
                const uint8_t index = t / slot;
                if (index >= len) {
                    phase = PHASE_INPUT;
                    pos = 0;
                    lit = -1;
                } else {
                    lit = (t % slot) < onMs() ? seq[index] : -1;
                }
            }
        } else if (lit >= 0 && now >= litUntil) {
            lit = -1;
        }
        return lit != before;
    }

    void render(Canvas &canvas, uint32_t) const override {
        static const uint8_t BRIGHT[4] = {PAL_RED, PAL_GREEN, PAL_BLUE,
                                          PAL_YELLOW};
        static const uint8_t DARK[4] = {PAL_DARK_RED, PAL_DARK_GREEN,
                                        PAL_DARK_BLUE, PAL_DARK_YELLOW};
        canvas.clear();
        for (uint8_t q = 0; q < 4; q++) {
            const int16_t row = q < 2 ? 0 : 9;
            const int16_t col = q % 2 ? 10 : 0;
            canvas.fill(row, col, 7, 8, lit == q ? BRIGHT[q] : DARK[q]);
        }
    }

    void status(GameStatus &s) const override {
        s.score = len > 1 ? len - 1 : 0;
        if (over) {
            s.score = len - 1;
        }
        s.level = len;
        strcpy(s.text, phase == PHASE_INPUT ? "input" : "watch");
    }
};

//------------------------------------------------------------------------------
// Flappy bird: one key, fly through the gaps
//------------------------------------------------------------------------------

class FlappyGame : public ArcadeGame {
public:
    static constexpr uint8_t BIRD_COL = 4;
    static constexpr uint8_t PIPES = 4;
    struct Pipe {
        int16_t col;
        uint8_t gapTop;
        bool passed;
    };

    int16_t y16 = 0;   // bird position in 1/16 rows
    int16_t vy16 = 0;  // per physics tick
    Pipe pipes[PIPES];
    uint8_t pipeCount = 0;
    bool started = false;
    uint32_t score = 0;
    uint32_t lastPhysics = 0;
    uint32_t lastScroll = 0;

    uint8_t gapSize() const { return score < 10 ? 6 : 5; }
    uint16_t scrollMs() const {
        return score >= 30 ? 110 : 200 - score * 3;
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        y16 = 7 * 16;
        vy16 = 0;
        pipeCount = 0;
        started = false;
        score = 0;
        lastPhysics = lastScroll = now;
    }

    // Gentle physics for young players: a flap lifts about 2.5 rows.
    static constexpr uint16_t PHYSICS_MS = 60;

    void flap() {
        started = true;
        vy16 = -12;
    }

    void input(const GameInput &in, uint32_t) override {
        if (in.key == GAME_KEY_ACTION || in.key == GAME_KEY_UP) {
            flap();
        }
    }

    void spawn() {
        if (pipeCount < PIPES) {
            pipes[pipeCount++] = {
                GAME_COLS,
                static_cast<uint8_t>(1 + rng.below(GAME_ROWS - gapSize() - 1)),
                false};
        }
    }

    bool collides() const {
        const int16_t row = y16 / 16;
        if (y16 < 0 || row >= GAME_ROWS) {
            return true;
        }
        for (uint8_t i = 0; i < pipeCount; i++) {
            if (pipes[i].col == BIRD_COL &&
                (row < pipes[i].gapTop || row >= pipes[i].gapTop + gapSize())) {
                return true;
            }
        }
        return false;
    }

    bool update(uint32_t now) override {
        if (over || !started) {
            return false;
        }
        bool changed = false;
        while (now - lastPhysics >= PHYSICS_MS) {
            lastPhysics += PHYSICS_MS;
            vy16 += 2;
            if (vy16 > 10) {
                vy16 = 10;
            }
            const int16_t before = y16 / 16;
            y16 += vy16;
            changed |= before != y16 / 16;
        }
        if (now - lastScroll >= scrollMs()) {
            // keep a steady rhythm even if the main loop was late once
            lastScroll += scrollMs();
            if (now - lastScroll >= scrollMs()) {
                lastScroll = now;
            }
            for (uint8_t i = 0; i < pipeCount; i++) {
                pipes[i].col--;
                if (!pipes[i].passed && pipes[i].col < BIRD_COL) {
                    pipes[i].passed = true;
                    score++;
                }
            }
            if (pipeCount && pipes[0].col < 0) {
                memmove(pipes, pipes + 1, (pipeCount - 1) * sizeof(Pipe));
                pipeCount--;
            }
            if (!pipeCount || pipes[pipeCount - 1].col <= GAME_COLS - 7) {
                spawn();
            }
            changed = true;
        }
        if (collides()) {
            over = true;
            changed = true;
        }
        return changed;
    }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        for (uint8_t i = 0; i < pipeCount; i++) {
            for (uint8_t r = 0; r < GAME_ROWS; r++) {
                if (r < pipes[i].gapTop || r >= pipes[i].gapTop + gapSize()) {
                    canvas.set(r, pipes[i].col, PAL_GREEN);
                }
            }
        }
        const int16_t row = y16 < 0 ? 0 : y16 / 16;
        canvas.set(row, BIRD_COL, PAL_YELLOW);
        canvas.set(row, BIRD_COL - 1, PAL_ORANGE);
    }

    void status(GameStatus &s) const override {
        s.score = score;
        strcpy(s.text, started ? "" : "tap");
    }
};

//------------------------------------------------------------------------------
// Breakout
//------------------------------------------------------------------------------

class BreakoutGame : public ArcadeGame {
public:
    static constexpr uint8_t BRICK_ROWS = 4;
    static constexpr uint8_t BRICK_COLS = 9; // 2 LEDs each
    static constexpr uint8_t PADDLE = 4;
    static constexpr uint8_t PADDLE_ROW = GAME_ROWS - 1;

    bool bricks[BRICK_ROWS][BRICK_COLS];
    int16_t paddle = 7;
    int16_t ballRow = 0, ballCol = 0;
    int8_t dr = -1, dc = 1;
    bool launched = false;
    uint32_t launchAt = 0;
    uint32_t lastStep = 0;
    uint32_t score = 0;
    int16_t lives = 3;
    uint16_t level = 1;

    uint16_t stepMs() const {
        return level >= 8 ? 60 : 130 - (level - 1) * 10;
    }

    void resetBricks() {
        for (auto &row : bricks) {
            for (bool &b : row) {
                b = true;
            }
        }
    }

    void resetBall(uint32_t now) {
        launched = false;
        launchAt = now + 1200;
        ballRow = PADDLE_ROW - 1;
        ballCol = paddle + PADDLE / 2;
        dr = -1;
        dc = rng.below(2) ? 1 : -1;
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        score = 0;
        lives = 3;
        level = 1;
        paddle = (GAME_COLS - PADDLE) / 2;
        resetBricks();
        resetBall(now);
        lastStep = now;
    }

    void input(const GameInput &in, uint32_t now) override {
        if (in.key == GAME_KEY_LEFT && paddle > 0) {
            paddle--;
        } else if (in.key == GAME_KEY_RIGHT &&
                   paddle < GAME_COLS - PADDLE) {
            paddle++;
        } else if (in.key == GAME_KEY_ACTION || in.key == GAME_KEY_UP) {
            launchAt = now;
        }
        if (!launched) {
            ballCol = paddle + PADDLE / 2;
        }
    }

    bool brickAt(int16_t row, int16_t col) const {
        const int16_t br = row - 1;
        return br >= 0 && br < BRICK_ROWS && col >= 0 && col < GAME_COLS &&
               bricks[br][col / 2];
    }

    bool update(uint32_t now) override {
        if (over) {
            return false;
        }
        if (!launched) {
            if (now >= launchAt) {
                launched = true;
                lastStep = now;
            }
            return true;
        }
        if (now - lastStep < stepMs()) {
            return false;
        }
        lastStep = now;

        int16_t nc = ballCol + dc;
        if (nc < 0 || nc >= GAME_COLS) {
            dc = -dc;
            nc = ballCol + dc;
        }
        int16_t nr = ballRow + dr;
        if (nr < 0) {
            dr = 1;
            nr = ballRow + dr;
        }
        if (brickAt(nr, nc)) {
            bricks[nr - 1][nc / 2] = false;
            score += 10 * level;
            dr = -dr;
            nr = ballRow + dr;
        } else if (brickAt(nr, ballCol)) {
            bricks[nr - 1][ballCol / 2] = false;
            score += 10 * level;
            dr = -dr;
            nr = ballRow + dr;
        }
        if (nr == PADDLE_ROW && nc >= paddle - 1 && nc <= paddle + PADDLE) {
            const int16_t hit = nc - paddle;
            dc = hit < PADDLE / 2 ? -1 : 1;
            dr = -1;
            nr = ballRow + dr;
            nc = ballCol + dc;
            if (nc < 0 || nc >= GAME_COLS) {
                dc = -dc;
                nc = ballCol + dc;
            }
        }
        ballRow = nr;
        ballCol = nc;

        if (ballRow > PADDLE_ROW) {
            if (--lives <= 0) {
                over = true;
            } else {
                resetBall(now);
            }
        }

        bool any = false;
        for (auto &row : bricks) {
            for (bool b : row) {
                any |= b;
            }
        }
        if (!any) {
            level++;
            resetBricks();
            resetBall(now);
        }
        return true;
    }

    void render(Canvas &canvas, uint32_t) const override {
        static const uint8_t COLORS[BRICK_ROWS] = {PAL_RED, PAL_ORANGE,
                                                   PAL_YELLOW, PAL_GREEN};
        canvas.clear();
        for (uint8_t r = 0; r < BRICK_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                if (bricks[r][c / 2]) {
                    canvas.set(r + 1, c, COLORS[r]);
                }
            }
        }
        canvas.fill(PADDLE_ROW, paddle, 1, PADDLE, PAL_BLUE);
        canvas.set(ballRow, ballCol, PAL_WHITE);
        for (int16_t i = 0; i < lives - 1; i++) {
            canvas.set(0, GAME_COLS - 1 - i * 2, PAL_DARK_BLUE);
        }
    }

    void status(GameStatus &s) const override {
        s.score = score;
        s.lives = lives;
        s.level = level;
    }
};

//------------------------------------------------------------------------------
// Space invaders
//------------------------------------------------------------------------------

class InvadersGame : public ArcadeGame {
public:
    static constexpr uint8_t ALIEN_ROWS = 3;
    static constexpr uint8_t ALIEN_COLS = 6;
    static constexpr uint8_t BOMBS = 3;
    static constexpr uint8_t PLAYER_ROW = GAME_ROWS - 1;

    bool alive[ALIEN_ROWS][ALIEN_COLS];
    int16_t ox = 0, oy = 1;
    int8_t dir = 1;
    int16_t player = 8;
    int16_t shotRow = -1, shotCol = 0;
    int16_t bombRow[BOMBS], bombCol[BOMBS];
    uint32_t lastAlien = 0, lastShot = 0, lastBomb = 0;
    uint32_t score = 0;
    int16_t lives = 3;
    uint16_t wave = 1;

    uint8_t remaining() const {
        uint8_t n = 0;
        for (auto &row : alive) {
            for (bool a : row) {
                n += a;
            }
        }
        return n;
    }

    uint16_t alienMs() const {
        const uint16_t base = wave >= 6 ? 250 : 600 - (wave - 1) * 70;
        return base * remaining() / (ALIEN_ROWS * ALIEN_COLS) + 80;
    }

    void newWave() {
        for (auto &row : alive) {
            for (bool &a : row) {
                a = true;
            }
        }
        ox = 0;
        oy = wave < 4 ? wave : 4;
        dir = 1;
        shotRow = -1;
        for (uint8_t i = 0; i < BOMBS; i++) {
            bombRow[i] = -1;
        }
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        score = 0;
        lives = 3;
        wave = 1;
        player = GAME_COLS / 2;
        newWave();
        lastAlien = lastShot = lastBomb = now;
    }

    void input(const GameInput &in, uint32_t) override {
        if (in.key == GAME_KEY_LEFT && player > 1) {
            player--;
        } else if (in.key == GAME_KEY_RIGHT && player < GAME_COLS - 2) {
            player++;
        } else if ((in.key == GAME_KEY_ACTION || in.key == GAME_KEY_UP) &&
                   shotRow < 0) {
            shotRow = PLAYER_ROW - 1;
            shotCol = player;
        }
    }

    bool alienAt(int16_t row, int16_t col, uint8_t &ar, uint8_t &ac) const {
        const int16_t dy = row - oy;
        const int16_t dx = col - ox;
        if (dy < 0 || dx < 0 || dy % 2 || dx % 2 ||
            dy / 2 >= ALIEN_ROWS || dx / 2 >= ALIEN_COLS) {
            return false;
        }
        ar = dy / 2;
        ac = dx / 2;
        return alive[ar][ac];
    }

    bool update(uint32_t now) override {
        if (over) {
            return false;
        }
        bool changed = false;
        uint8_t ar, ac;

        if (shotRow >= 0 && now - lastShot >= 50) {
            lastShot = now;
            shotRow--;
            if (shotRow >= 0 && alienAt(shotRow, shotCol, ar, ac)) {
                alive[ar][ac] = false;
                score += 20 * wave;
                shotRow = -1;
                if (!remaining()) {
                    wave++;
                    newWave();
                }
            }
            changed = true;
        }

        if (now - lastAlien >= alienMs()) {
            lastAlien = now;
            int16_t minC = GAME_COLS, maxC = -1, maxR = -1;
            for (uint8_t r = 0; r < ALIEN_ROWS; r++) {
                for (uint8_t c = 0; c < ALIEN_COLS; c++) {
                    if (alive[r][c]) {
                        minC = minC < c ? minC : c;
                        maxC = maxC > c ? maxC : c;
                        maxR = maxR > r ? maxR : r;
                    }
                }
            }
            const int16_t left = ox + minC * 2 + dir;
            const int16_t right = ox + maxC * 2 + dir;
            if (left < 0 || right >= GAME_COLS) {
                dir = -dir;
                oy++;
            } else {
                ox += dir;
            }
            if (oy + maxR * 2 >= PLAYER_ROW - 1) {
                over = true;
            }
            // A random bottom alien drops a bomb.
            if (rng.below(3) == 0) {
                const uint8_t c = rng.below(ALIEN_COLS);
                for (int8_t r = ALIEN_ROWS - 1; r >= 0; r--) {
                    if (alive[r][c]) {
                        for (uint8_t i = 0; i < BOMBS; i++) {
                            if (bombRow[i] < 0) {
                                bombRow[i] = oy + r * 2 + 1;
                                bombCol[i] = ox + c * 2;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            changed = true;
        }

        if (now - lastBomb >= 160) {
            lastBomb = now;
            for (uint8_t i = 0; i < BOMBS; i++) {
                if (bombRow[i] < 0) {
                    continue;
                }
                bombRow[i]++;
                if (bombRow[i] == PLAYER_ROW &&
                    bombCol[i] >= player - 1 && bombCol[i] <= player + 1) {
                    bombRow[i] = -1;
                    if (--lives <= 0) {
                        over = true;
                    }
                } else if (bombRow[i] >= GAME_ROWS) {
                    bombRow[i] = -1;
                }
                changed = true;
            }
        }
        return changed;
    }

    void render(Canvas &canvas, uint32_t) const override {
        static const uint8_t COLORS[ALIEN_ROWS] = {PAL_PURPLE, PAL_PINK,
                                                   PAL_CYAN};
        canvas.clear();
        for (uint8_t r = 0; r < ALIEN_ROWS; r++) {
            for (uint8_t c = 0; c < ALIEN_COLS; c++) {
                if (alive[r][c]) {
                    canvas.set(oy + r * 2, ox + c * 2, COLORS[r]);
                }
            }
        }
        canvas.fill(PLAYER_ROW, player - 1, 1, 3, PAL_GREEN);
        canvas.set(PLAYER_ROW - 1, player, PAL_GREEN);
        if (shotRow >= 0) {
            canvas.set(shotRow, shotCol, PAL_YELLOW);
        }
        for (uint8_t i = 0; i < BOMBS; i++) {
            if (bombRow[i] >= 0) {
                canvas.set(bombRow[i], bombCol[i], PAL_RED);
            }
        }
        for (int16_t i = 0; i < lives - 1; i++) {
            canvas.set(0, GAME_COLS - 1 - i * 2, PAL_DARK_GREEN);
        }
    }

    void status(GameStatus &s) const override {
        s.score = score;
        s.lives = lives;
        s.level = wave;
    }
};

//------------------------------------------------------------------------------
// Frogger: cross the busy roads
//------------------------------------------------------------------------------

class FroggerGame : public ArcadeGame {
public:
    static constexpr uint8_t GOAL_ROW = 1;
    static constexpr uint8_t SAFE_ROW = 8;
    static constexpr uint8_t START_ROW = GAME_ROWS - 1;

    struct Lane {
        uint32_t cars; // bit per column
        int8_t dir;
        uint16_t ms;
        uint32_t last;
        uint8_t color;
    };

    Lane lanes[GAME_ROWS];
    int16_t frogRow = START_ROW, frogCol = GAME_COLS / 2;
    uint32_t score = 0;
    int16_t lives = 3;
    uint16_t level = 1;

    static bool isLane(uint8_t row) {
        return row > GOAL_ROW && row < START_ROW && row != SAFE_ROW;
    }

    void setupLanes(uint32_t now) {
        static const uint8_t COLORS[] = {PAL_RED, PAL_ORANGE, PAL_PURPLE,
                                         PAL_CYAN, PAL_PINK, PAL_BLUE};
        for (uint8_t row = 0; row < GAME_ROWS; row++) {
            Lane &lane = lanes[row];
            lane.cars = 0;
            if (!isLane(row)) {
                continue;
            }
            lane.dir = row % 2 ? 1 : -1;
            const uint16_t base = 650 - (level - 1) * 60;
            lane.ms = (base < 250 ? 250 : base) - rng.below(150);
            lane.last = now;
            lane.color = COLORS[row % 6];
            // 2-3 cars of 2-3 cells with gaps of at least 3
            uint8_t col = rng.below(4);
            while (col < GAME_COLS) {
                const uint8_t len = 2 + rng.below(2);
                for (uint8_t i = 0; i < len && col + i < GAME_COLS; i++) {
                    lane.cars |= 1u << (col + i);
                }
                col += len + 3 + rng.below(4);
            }
        }
    }

    void resetFrog() {
        frogRow = START_ROW;
        frogCol = GAME_COLS / 2;
    }

    bool carAt(int16_t row, int16_t col) const {
        return row >= 0 && row < GAME_ROWS && isLane(row) &&
               (lanes[row].cars >> col) & 1u;
    }

    void hitCheck() {
        if (carAt(frogRow, frogCol)) {
            if (--lives <= 0) {
                over = true;
            } else {
                resetFrog();
            }
        }
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        score = 0;
        lives = 3;
        level = 1;
        setupLanes(now);
        resetFrog();
    }

    void input(const GameInput &in, uint32_t now) override {
        switch (in.key) {
        case GAME_KEY_UP:
            frogRow--;
            break;
        case GAME_KEY_DOWN:
            if (frogRow < START_ROW) {
                frogRow++;
            }
            break;
        case GAME_KEY_LEFT:
            if (frogCol > 0) {
                frogCol--;
            }
            break;
        case GAME_KEY_RIGHT:
            if (frogCol < GAME_COLS - 1) {
                frogCol++;
            }
            break;
        default:
            return;
        }
        if (frogRow <= GOAL_ROW) {
            score += 10 * level;
            level++;
            setupLanes(now);
            resetFrog();
            return;
        }
        hitCheck();
    }

    bool update(uint32_t now) override {
        if (over) {
            return false;
        }
        bool changed = false;
        const uint32_t mask = (1u << GAME_COLS) - 1;
        for (uint8_t row = 0; row < GAME_ROWS; row++) {
            Lane &lane = lanes[row];
            if (!isLane(row) || now - lane.last < lane.ms) {
                continue;
            }
            lane.last = now;
            if (lane.dir > 0) {
                lane.cars = ((lane.cars << 1) | (lane.cars >> (GAME_COLS - 1))) &
                            mask;
            } else {
                lane.cars = ((lane.cars >> 1) | (lane.cars << (GAME_COLS - 1))) &
                            mask;
            }
            changed = true;
        }
        if (changed) {
            hitCheck();
        }
        return changed;
    }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        canvas.fill(0, 0, GOAL_ROW + 1, GAME_COLS, PAL_DARK_GREEN);
        canvas.fill(SAFE_ROW, 0, 1, GAME_COLS, PAL_DARK_GREEN);
        canvas.fill(START_ROW, 0, 1, GAME_COLS, PAL_DARK_GREEN);
        for (uint8_t row = 0; row < GAME_ROWS; row++) {
            if (!isLane(row)) {
                continue;
            }
            for (uint8_t col = 0; col < GAME_COLS; col++) {
                if ((lanes[row].cars >> col) & 1u) {
                    canvas.set(row, col, lanes[row].color);
                }
            }
        }
        canvas.set(frogRow, frogCol, PAL_YELLOW);
        for (int16_t i = 0; i < lives - 1; i++) {
            canvas.set(0, GAME_COLS - 1 - i * 2, PAL_GREEN);
        }
    }

    void status(GameStatus &s) const override {
        s.score = score;
        s.lives = lives;
        s.level = level;
    }
};

//------------------------------------------------------------------------------
// Pong for two players (or against the clock)
//------------------------------------------------------------------------------

class PongGame : public ArcadeGame {
public:
    static constexpr uint8_t PADDLE = 4;
    static constexpr uint8_t WIN = 5;

    bool cpu = false;
    int16_t p1 = 6, p2 = 6; // top row of the paddles
    int16_t ballRow = 7, ballCol = 8;
    int8_t dr = 1, dc = 1;
    uint8_t s1 = 0, s2 = 0;
    uint8_t winner = 0;
    uint16_t rally = 0;
    uint32_t lastStep = 0, serveAt = 0, lastCpu = 0;

    PongGame() { pausable = false; }

    uint16_t stepMs() const { return rally > 15 ? 60 : 120 - rally * 4; }

    void serve(int8_t towards, uint32_t now) {
        ballRow = GAME_ROWS / 2 - 1 + rng.below(2);
        ballCol = GAME_COLS / 2;
        dc = towards;
        dr = rng.below(2) ? 1 : -1;
        rally = 0;
        serveAt = now + 1000;
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        twoPlayer = true;
        p1 = p2 = (GAME_ROWS - PADDLE) / 2;
        s1 = s2 = 0;
        winner = 0;
        serve(rng.below(2) ? 1 : -1, now);
        lastStep = lastCpu = now;
    }

    void input(const GameInput &in, uint32_t) override {
        if (in.player == 2) {
            cpu = false; // a second phone joined
        }
        int16_t &p = in.player == 2 ? p2 : p1;
        if (in.key == GAME_KEY_UP && p > 0) {
            p--;
        } else if (in.key == GAME_KEY_DOWN && p < GAME_ROWS - PADDLE) {
            p++;
        }
    }

    bool update(uint32_t now) override {
        if (over) {
            return false;
        }
        bool changed = false;
        if (cpu && now - lastCpu >= 140) {
            lastCpu = now;
            // Follows the ball, a little lazy so it can be beaten.
            const int16_t center = p2 + PADDLE / 2;
            if (dc > 0 && rng.below(10) < 8) {
                if (ballRow < center - 1 && p2 > 0) {
                    p2--;
                } else if (ballRow > center && p2 < GAME_ROWS - PADDLE) {
                    p2++;
                }
                changed = true;
            }
        }
        if (now < serveAt || now - lastStep < stepMs()) {
            return changed;
        }
        lastStep = now;

        int16_t nr = ballRow + dr;
        if (nr < 0 || nr >= GAME_ROWS) {
            dr = -dr;
            nr = ballRow + dr;
        }
        int16_t nc = ballCol + dc;
        if (nc == 0 && nr >= p1 - 1 && nr <= p1 + PADDLE) {
            dc = 1;
            dr = nr < p1 + PADDLE / 2 ? -1 : 1;
            nc = ballCol + dc;
            rally++;
        } else if (nc == GAME_COLS - 1 && nr >= p2 - 1 && nr <= p2 + PADDLE) {
            dc = -1;
            dr = nr < p2 + PADDLE / 2 ? -1 : 1;
            nc = ballCol + dc;
            rally++;
        }
        if (nr < 0 || nr >= GAME_ROWS) {
            dr = -dr;
            nr = ballRow + dr;
        }
        ballRow = nr;
        ballCol = nc;

        if (ballCol < 0 || ballCol >= GAME_COLS) {
            const bool p2Scores = ballCol < 0;
            uint8_t &points = p2Scores ? s2 : s1;
            points++;
            if (points >= WIN) {
                winner = p2Scores ? 2 : 1;
                over = true;
            } else {
                serve(p2Scores ? -1 : 1, now);
            }
        }
        return true;
    }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        for (uint8_t r = 0; r < GAME_ROWS; r += 2) {
            canvas.set(r, GAME_COLS / 2, PAL_DIM);
        }
        canvas.fill(p1, 0, PADDLE, 1, PAL_BLUE);
        canvas.fill(p2, GAME_COLS - 1, PADDLE, 1, PAL_ORANGE);
        canvas.set(ballRow, ballCol, PAL_WHITE);
        for (uint8_t i = 0; i < s1; i++) {
            canvas.set(0, GAME_COLS / 2 - 2 - i, PAL_DARK_BLUE);
        }
        for (uint8_t i = 0; i < s2; i++) {
            canvas.set(0, GAME_COLS / 2 + 2 + i, PAL_DARK_YELLOW);
        }
    }

    void status(GameStatus &s) const override {
        s.score1 = s1;
        s.score2 = s2;
        s.winner = winner;
        s.score = s1 > s2 ? s1 : s2;
        strcpy(s.text, cpu ? "cpu" : "");
    }
};

//------------------------------------------------------------------------------
// Connect four (two players or against the clock)
//------------------------------------------------------------------------------

class Connect4Game : public ArcadeGame {
public:
    static constexpr uint8_t ROWS = 6;
    static constexpr uint8_t COLS = 7;

    uint8_t board[ROWS][COLS];
    uint8_t cursor = 3;
    uint8_t turn = 1;
    uint8_t winner = 0;
    bool cpu = false;
    bool draw = false;
    uint8_t winCells[4][2];
    uint32_t cpuAt = 0;

    Connect4Game() {
        twoPlayer = true;
        pausable = false;
    }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        memset(board, 0, sizeof board);
        cursor = 3;
        turn = 1;
        winner = 0;
        draw = false;
    }

    int8_t dropRow(uint8_t col) const {
        for (int8_t r = ROWS - 1; r >= 0; r--) {
            if (!board[r][col]) {
                return r;
            }
        }
        return -1;
    }

    bool fourAt(uint8_t player, bool remember) {
        static const int8_t DIRS[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
        for (uint8_t r = 0; r < ROWS; r++) {
            for (uint8_t c = 0; c < COLS; c++) {
                for (auto &d : DIRS) {
                    uint8_t n = 0;
                    for (; n < 4; n++) {
                        const int8_t rr = r + d[0] * n;
                        const int8_t cc = c + d[1] * n;
                        if (rr < 0 || rr >= ROWS || cc < 0 || cc >= COLS ||
                            board[rr][cc] != player) {
                            break;
                        }
                    }
                    if (n == 4) {
                        if (remember) {
                            for (uint8_t i = 0; i < 4; i++) {
                                winCells[i][0] = r + d[0] * i;
                                winCells[i][1] = c + d[1] * i;
                            }
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool drop(uint8_t col, uint32_t now) {
        const int8_t row = dropRow(col);
        if (row < 0) {
            return false;
        }
        board[row][col] = turn;
        if (fourAt(turn, true)) {
            winner = turn;
            over = true;
            return true;
        }
        bool full = true;
        for (uint8_t c = 0; c < COLS; c++) {
            full &= board[0][c] != 0;
        }
        if (full) {
            draw = true;
            over = true;
            return true;
        }
        turn = 3 - turn;
        cpuAt = now + 700;
        return true;
    }

    // Wins if it can, blocks a win, avoids handing one over, likes the center.
    uint8_t cpuChoice() {
        for (uint8_t who : {static_cast<uint8_t>(2), static_cast<uint8_t>(1)}) {
            for (uint8_t c = 0; c < COLS; c++) {
                const int8_t r = dropRow(c);
                if (r < 0) {
                    continue;
                }
                board[r][c] = who;
                const bool wins = fourAt(who, false);
                board[r][c] = 0;
                if (wins) {
                    return c;
                }
            }
        }
        static const uint8_t ORDER[COLS] = {3, 2, 4, 1, 5, 0, 6};
        uint8_t fallback = COLS;
        const uint8_t shift = rng.below(3); // a bit of variety
        for (uint8_t i = 0; i < COLS; i++) {
            const uint8_t c = ORDER[(i + (i < 3 ? shift : 0)) % COLS];
            const int8_t r = dropRow(c);
            if (r < 0) {
                continue;
            }
            if (fallback == COLS) {
                fallback = c;
            }
            bool givesWin = false;
            if (r > 0) {
                board[r][c] = 2;
                board[r - 1][c] = 1;
                givesWin = fourAt(1, false);
                board[r - 1][c] = 0;
                board[r][c] = 0;
            }
            if (!givesWin) {
                return c;
            }
        }
        return fallback;
    }

    void input(const GameInput &in, uint32_t now) override {
        if (over || (cpu && turn == 2)) {
            return;
        }
        if (in.key == GAME_KEY_LEFT && cursor > 0) {
            cursor--;
        } else if (in.key == GAME_KEY_RIGHT && cursor < COLS - 1) {
            cursor++;
        } else if (in.key == GAME_KEY_DOWN || in.key == GAME_KEY_ACTION) {
            drop(cursor, now);
        }
    }

    bool update(uint32_t now) override {
        if (over || !cpu || turn != 2 || now < cpuAt) {
            return false;
        }
        const uint8_t c = cpuChoice();
        cursor = c;
        drop(c, now);
        return true;
    }

    void render(Canvas &canvas, uint32_t now) const override {
        static const uint8_t PLAYER[3] = {PAL_DARK_BLUE, PAL_RED, PAL_YELLOW};
        canvas.clear();
        const uint8_t top = GAME_ROWS - ROWS * 2;  // 4
        const uint8_t left = (GAME_COLS - COLS * 2) / 2; // 2
        for (uint8_t r = 0; r < ROWS; r++) {
            for (uint8_t c = 0; c < COLS; c++) {
                uint8_t color = PLAYER[board[r][c]];
                if (winner && (now / 300) % 2) {
                    for (auto &w : winCells) {
                        if (w[0] == r && w[1] == c) {
                            color = PAL_WHITE;
                        }
                    }
                }
                canvas.fill(top + r * 2, left + c * 2, 2, 2, color);
            }
        }
        if (!over) {
            canvas.fill(1, left + cursor * 2, 2, 2, PLAYER[turn]);
        }
    }

    void status(GameStatus &s) const override {
        s.winner = winner;
        s.turn = turn;
        strcpy(s.text, draw ? "draw" : (cpu ? "cpu" : ""));
    }
};

//------------------------------------------------------------------------------
// Reaction duel: who presses first when it turns green?
//------------------------------------------------------------------------------

class ReactionGame : public ArcadeGame {
public:
    static constexpr uint8_t WIN = 5;
    enum Phase : uint8_t { PHASE_WAIT, PHASE_GO, PHASE_RESULT };

    uint8_t phase = PHASE_WAIT;
    uint32_t goAt = 0, resultUntil = 0;
    uint8_t s1 = 0, s2 = 0;
    uint8_t lastPoint = 0; // player who got the last point
    bool foul = false;
    uint8_t winner = 0;

    ReactionGame() {
        twoPlayer = true;
        pausable = false;
    }

    void nextRound(uint32_t now) {
        phase = PHASE_WAIT;
        goAt = now + 2000 + rng.below(3000);
    }

    void start(uint32_t seed, uint32_t now) override {
        rng.seed(seed);
        over = false;
        s1 = s2 = 0;
        winner = 0;
        lastPoint = 0;
        nextRound(now);
    }

    void point(uint8_t player, uint32_t now) {
        lastPoint = player;
        uint8_t &score = player == 1 ? s1 : s2;
        score++;
        phase = PHASE_RESULT;
        resultUntil = now + 1500;
        if (score >= WIN) {
            winner = player;
            over = true;
        }
    }

    void input(const GameInput &in, uint32_t now) override {
        if (in.key != GAME_KEY_ACTION || over) {
            return;
        }
        const uint8_t player = in.player == 2 ? 2 : 1;
        if (phase == PHASE_WAIT) {
            foul = true;
            point(3 - player, now); // too early: the other one scores
        } else if (phase == PHASE_GO) {
            foul = false;
            point(player, now);
        }
    }

    bool update(uint32_t now) override {
        if (over) {
            return false;
        }
        if (phase == PHASE_WAIT && now >= goAt) {
            phase = PHASE_GO;
            return true;
        }
        if (phase == PHASE_RESULT && now >= resultUntil) {
            nextRound(now);
            return true;
        }
        return false;
    }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        switch (phase) {
        case PHASE_WAIT:
            canvas.fill(0, 0, 1, GAME_COLS, PAL_DARK_RED);
            canvas.fill(GAME_ROWS - 2, 0, 1, GAME_COLS, PAL_DARK_RED);
            canvas.fill(0, 0, GAME_ROWS - 1, 1, PAL_DARK_RED);
            canvas.fill(0, GAME_COLS - 1, GAME_ROWS - 1, 1, PAL_DARK_RED);
            break;
        case PHASE_GO:
            canvas.fill(0, 0, GAME_ROWS - 1, GAME_COLS, PAL_GREEN);
            break;
        case PHASE_RESULT:
            if (lastPoint == 1) {
                canvas.fill(0, 0, GAME_ROWS - 1, GAME_COLS / 2, PAL_BLUE);
            } else {
                canvas.fill(0, GAME_COLS / 2, GAME_ROWS - 1, GAME_COLS / 2,
                            PAL_ORANGE);
            }
            break;
        }
        for (uint8_t i = 0; i < s1; i++) {
            canvas.set(GAME_ROWS - 1, i * 2, PAL_BLUE);
        }
        for (uint8_t i = 0; i < s2; i++) {
            canvas.set(GAME_ROWS - 1, GAME_COLS - 1 - i * 2, PAL_ORANGE);
        }
    }

    void status(GameStatus &s) const override {
        s.score1 = s1;
        s.score2 = s2;
        s.winner = winner;
        s.score = s1 > s2 ? s1 : s2;
        strcpy(s.text, phase == PHASE_GO ? "go" : (phase == PHASE_WAIT ? "wait" : ""));
        strcpy(s.text2, phase == PHASE_RESULT && foul ? "foul" : "");
    }
};

//------------------------------------------------------------------------------
// Hangman: guessed letters light up on the front
//------------------------------------------------------------------------------

static const char *const HANGMAN_WORDS[] = {
    "HAUS",       "BAUM",       "SCHULE",     "KATZE",      "HUND",
    "PFERD",      "ELEFANT",    "GIRAFFE",    "BANANE",     "APFEL",
    "ERDBEERE",   "FUSSBALL",   "FAHRRAD",    "SCHOKOLADE", "PIZZA",
    "DRACHE",     "PIRAT",      "RAKETE",     "ROBOTER",    "DINOSAURIER",
    "PINGUIN",    "DELFIN",     "SONNE",      "REGENBOGEN", "SCHNEEMANN",
    "GEBURTSTAG", "FERIEN",     "ZAUBERER",   "RITTER",     "BURG",
    "KROKODIL",   "SCHMETTERLING", "COMPUTER", "TELEFON",   "GITARRE",
    "TROMPETE",   "KUCHEN",     "NUDELN",     "LIMONADE",   "WOLKE",
    "GEWITTER",   "VULKAN",     "PLANET",     "ASTRONAUT",  "FEUERWEHR",
    "POLIZEI",    "BAGGER",     "TRAKTOR",    "EISENBAHN",  "FLUGZEUG",
    "SCHIFF",     "INSEL",      "SCHATZ",     "KOMPASS",    "LABYRINTH",
    "ZEBRA",      "TIGER",      "AFFE",       "EULE",       "IGEL",
    "KAROTTE",    "TOMATE",     "BRILLE",     "RUCKSACK",   "TASCHENLAMPE",
    "WALD",       "BERG",       "STRAND",     "MUSCHEL",    "QUALLE",
};
constexpr uint8_t HANGMAN_WORD_COUNT =
    sizeof(HANGMAN_WORDS) / sizeof(HANGMAN_WORDS[0]);

class HangmanGame : public ArcadeGame {
public:
    static constexpr uint8_t MAX_WRONG = 8;

    const char *word = HANGMAN_WORDS[0];
    bool guessed[26];
    uint8_t wrong = 0;
    bool won = false;

    HangmanGame() { pausable = false; }

    bool inWord(char letter) const { return strchr(word, letter) != nullptr; }

    bool solved() const {
        for (const char *p = word; *p; p++) {
            if (!guessed[*p - 'A']) {
                return false;
            }
        }
        return true;
    }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        won = false;
        word = HANGMAN_WORDS[rng.below(HANGMAN_WORD_COUNT)];
        memset(guessed, 0, sizeof guessed);
        wrong = 0;
    }

    void input(const GameInput &in, uint32_t) override {
        if (over || in.key < GAME_KEY_LETTER_A || in.key > GAME_KEY_LETTER_Z) {
            return;
        }
        const uint8_t i = in.key - GAME_KEY_LETTER_A;
        if (guessed[i]) {
            return;
        }
        guessed[i] = true;
        if (!inWord('A' + i) && ++wrong >= MAX_WRONG) {
            over = true;
        } else if (solved()) {
            won = true;
            over = true;
        }
    }

    bool update(uint32_t) override { return false; }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                const char ch = FRONT_LETTERS[r][c];
                if (ch < 'A' || ch > 'Z' || !guessed[ch - 'A']) {
                    continue;
                }
                canvas.set(r, c, inWord(ch) ? PAL_GREEN : PAL_DARK_RED);
            }
        }
    }

    void status(GameStatus &s) const override {
        s.lives = MAX_WRONG - wrong;
        s.winner = won ? 1 : 0;
        s.score = won ? (MAX_WRONG - wrong) * 10 + strlen(word) : 0;
        // "K A _ Z E", the full word once the game is over
        size_t n = 0;
        for (const char *p = word; *p && n + 2 < sizeof s.text; p++) {
            s.text[n++] = (over || guessed[*p - 'A']) ? *p : '_';
            s.text[n++] = ' ';
        }
        s.text[n ? n - 1 : 0] = '\0';
        n = 0;
        for (uint8_t i = 0; i < 26 && n + 1 < sizeof s.text2; i++) {
            if (guessed[i] && !inWord('A' + i)) {
                s.text2[n++] = 'A' + i;
            }
        }
        s.text2[n] = '\0';
    }
};

//------------------------------------------------------------------------------
// Clock quiz: read the time in words, set it on an analog clock
//------------------------------------------------------------------------------

class ClockQuizGame : public ArcadeGame {
public:
    static constexpr uint8_t QUESTIONS = 10;

    uint8_t displayMode = 13;
    uint8_t question = 0;
    uint8_t hh = 0, mm = 0;
    uint8_t correct = 0;
    int8_t lastResult = -1; // -1 = none, 0 = wrong, 1 = right
    uint32_t feedbackUntil = 0;
    uint8_t lastH = 0, lastM = 0;

    ClockQuizGame() { pausable = false; }

    void next() {
        question++;
        hh = rng.below(24);
        mm = rng.below(12) * 5;
    }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        question = 0;
        correct = 0;
        lastResult = -1;
        feedbackUntil = 0;
        if (displayMode >= CLOCK_MODES_COUNT) {
            displayMode = 13;
        }
        next();
    }

    void input(const GameInput &in, uint32_t now) override {
        if (over || in.key != GAME_KEY_ANSWER || now < feedbackUntil) {
            return;
        }
        const uint8_t ah = in.value / 100;
        const uint8_t am = in.value % 100;
        lastResult = (ah % 12 == hh % 12 && am == mm) ? 1 : 0;
        correct += lastResult;
        lastH = hh;
        lastM = mm;
        feedbackUntil = now + 1800;
    }

    bool update(uint32_t now) override {
        if (feedbackUntil && now >= feedbackUntil) {
            feedbackUntil = 0;
            if (question >= QUESTIONS) {
                over = true;
            } else {
                next();
            }
            return true;
        }
        return false;
    }

    void render(Canvas &canvas, uint32_t now) const override {
        canvas.clear();
        WordSet words;
        fillWords(displayMode, feedbackUntil ? lastH : hh,
                  feedbackUntil ? lastM : mm, words);
        uint8_t color = PAL_WHITE;
        if (feedbackUntil && now < feedbackUntil) {
            color = lastResult ? PAL_GREEN : PAL_RED;
        }
        for (uint8_t idx = 1; idx < WP_COUNT; idx++) {
            if (!words[idx]) {
                continue;
            }
            const WORD_ILLUMINATION &w = illumination[idx];
            canvas.fill(w.row, w.col, 1, w.len & ILLUMINATION_LEN_MASK,
                        color);
        }
    }

    void status(GameStatus &s) const override {
        s.score = correct * 10;
        s.level = question;
        snprintf(s.text, sizeof s.text, "%u/%u", question, QUESTIONS);
        if (lastResult >= 0) {
            snprintf(s.text2, sizeof s.text2, "%c%02u:%02u",
                     lastResult ? '+' : '-', lastH, lastM);
        }
    }
};

//------------------------------------------------------------------------------
// Word find: tap the letters of the word on the front
//------------------------------------------------------------------------------

class WordFindGame : public ArcadeGame {
public:
    static constexpr uint8_t ROUNDS = 10;

    uint8_t target = 1;  // index into illumination[]
    uint8_t found = 0;
    uint8_t round = 0;
    int16_t tapRow = -1, tapStart = -1;
    uint8_t tapLen = 0;
    bool foundCells[GAME_ROWS][GAME_COLS];
    uint32_t flashUntil = 0;
    bool flashGood = false;

    WordFindGame() { pausable = false; }

    void wordText(uint8_t idx, char *out) const {
        const WORD_ILLUMINATION &w = illumination[idx];
        const uint8_t len = w.len & ILLUMINATION_LEN_MASK;
        for (uint8_t i = 0; i < len; i++) {
            const char ch = FRONT_LETTERS[w.row][w.col + i];
            out[i] = ch == '#' ? 'U' : (ch == '%' ? 'O' : ch);
        }
        out[len] = '\0';
    }

    void pick() {
        round++;
        do {
            target = 1 + rng.below(WP_COUNT - 1);
        } while ((illumination[target].len & ILLUMINATION_LEN_MASK) < 3);
        tapRow = tapStart = -1;
        tapLen = 0;
    }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        found = 0;
        round = 0;
        memset(foundCells, 0, sizeof foundCells);
        flashUntil = 0;
        pick();
    }

    void input(const GameInput &in, uint32_t now) override {
        if (over || in.key != GAME_KEY_TAP || in.value >= SNAKE_CELLS ||
            now < flashUntil) {
            return;
        }
        const int16_t row = in.value / GAME_COLS;
        const int16_t col = in.value % GAME_COLS;
        char word[20];
        wordText(target, word);
        const uint8_t len = strlen(word);
        const char want = word[tapLen];
        const char have = FRONT_LETTERS[row][col];
        const char plain = have == '#' ? 'U' : (have == '%' ? 'O' : have);

        const bool continues = tapLen == 0 || (row == tapRow &&
                                               col == tapStart + tapLen);
        if (!continues || plain != want) {
            // Start over with this tap if it fits as the first letter.
            tapLen = 0;
            tapRow = tapStart = -1;
            if (plain != word[0]) {
                flashGood = false;
                flashUntil = now + 500;
                return;
            }
        }
        if (tapLen == 0) {
            tapRow = row;
            tapStart = col;
        }
        tapLen++;
        if (tapLen == len) {
            for (uint8_t i = 0; i < len; i++) {
                foundCells[tapRow][tapStart + i] = true;
            }
            found++;
            flashGood = true;
            flashUntil = now + 900;
        }
    }

    bool update(uint32_t now) override {
        if (!flashUntil || now < flashUntil) {
            return false;
        }
        flashUntil = 0;
        if (flashGood) {
            if (round >= ROUNDS) {
                over = true;
            } else {
                pick();
            }
        }
        return true;
    }

    void render(Canvas &canvas, uint32_t now) const override {
        canvas.clear();
        for (uint8_t r = 0; r < GAME_ROWS; r++) {
            for (uint8_t c = 0; c < GAME_COLS; c++) {
                if (foundCells[r][c]) {
                    canvas.set(r, c, PAL_DARK_GREEN);
                }
            }
        }
        const bool flashing = flashUntil && now < flashUntil;
        for (uint8_t i = 0; i < tapLen; i++) {
            canvas.set(tapRow, tapStart + i,
                       flashing && flashGood ? PAL_GREEN : PAL_YELLOW);
        }
        if (flashing && !flashGood) {
            canvas.fill(0, 0, 1, GAME_COLS, PAL_RED);
            canvas.fill(GAME_ROWS - 1, 0, 1, GAME_COLS, PAL_RED);
        }
    }

    void status(GameStatus &s) const override {
        s.score = found * 10;
        s.level = round;
        wordText(target, s.text);
        snprintf(s.text2, sizeof s.text2, "%u/%u", round, ROUNDS);
    }
};

//------------------------------------------------------------------------------
// Mental arithmetic
//------------------------------------------------------------------------------

class MathGame : public ArcadeGame {
public:
    static constexpr uint8_t TASKS = 10;

    uint8_t task = 0;
    uint16_t a = 0, b = 0, answer = 0;
    char op = '+';
    uint8_t correct = 0;
    int8_t lastResult = -1;
    uint16_t lastAnswer = 0;
    uint32_t feedbackUntil = 0;

    MathGame() { pausable = false; }

    void next() {
        task++;
        switch (rng.below(4)) {
        case 0: // sum up to 100
            a = 1 + rng.below(89);
            b = 1 + rng.below(99 - a);
            op = '+';
            answer = a + b;
            break;
        case 1: // difference, never negative
            a = 10 + rng.below(90);
            b = 1 + rng.below(a);
            op = '-';
            answer = a - b;
            break;
        case 2: // times table
            a = 2 + rng.below(9);
            b = 2 + rng.below(9);
            op = 'x';
            answer = a * b;
            break;
        default: // exact division
            b = 2 + rng.below(9);
            answer = 2 + rng.below(9);
            a = b * answer;
            op = '/';
            break;
        }
    }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        task = 0;
        correct = 0;
        lastResult = -1;
        feedbackUntil = 0;
        next();
    }

    void input(const GameInput &in, uint32_t now) override {
        if (over || in.key != GAME_KEY_ANSWER || now < feedbackUntil) {
            return;
        }
        lastResult = in.value == answer ? 1 : 0;
        correct += lastResult;
        lastAnswer = answer;
        feedbackUntil = now + 1500;
    }

    bool update(uint32_t now) override {
        if (feedbackUntil && now >= feedbackUntil) {
            feedbackUntil = 0;
            if (task >= TASKS) {
                over = true;
            } else {
                next();
            }
            return true;
        }
        return false;
    }

    void render(Canvas &canvas, uint32_t now) const override {
        canvas.clear();
        char line[8];
        if (feedbackUntil && now < feedbackUntil) {
            const uint8_t color = lastResult ? PAL_GREEN : PAL_RED;
            snprintf(line, sizeof line, "%u", lastAnswer);
            canvas.centeredText(5, line, color);
            return;
        }
        snprintf(line, sizeof line, "%u%c", a, op);
        canvas.centeredText(2, line, PAL_WHITE);
        snprintf(line, sizeof line, "%u=", b);
        canvas.centeredText(9, line, PAL_WHITE);
    }

    void status(GameStatus &s) const override {
        s.score = correct * 10;
        s.level = task;
        static const char *const OPS = "+-x/";
        const char *pos = strchr(OPS, op);
        static const char *const SHOWN[4] = {"+", "-", "×", ":"};
        snprintf(s.text, sizeof s.text, "%u %s %u = ?", a,
                 pos ? SHOWN[pos - OPS] : "+", b);
        snprintf(s.text2, sizeof s.text2, "%u/%u", task, TASKS);
        if (lastResult >= 0) {
            char tmp[16];
            snprintf(tmp, sizeof tmp, " %c%u", lastResult ? '+' : '-',
                     lastAnswer);
            strncat(s.text2, tmp, sizeof s.text2 - strlen(s.text2) - 1);
        }
    }
};

//------------------------------------------------------------------------------
// Paint: draw on the front from the phone
//------------------------------------------------------------------------------

class PaintGame : public ArcadeGame {
public:
    Canvas picture;
    uint8_t brightness = 100; // percent of the clock brightness
    bool dirty = true;

    PaintGame() {
        pausable = false;
        picture.clear();
    }

    // Keeps the picture when the mode is opened again.
    void start(uint32_t, uint32_t) override {
        over = false;
        dirty = true;
    }

    void input(const GameInput &in, uint32_t) override {
        switch (in.key) {
        case GAME_KEY_PAINT_SET: {
            const uint32_t cellNumber = in.value / 64;
            const uint8_t color = in.value % 64;
            if (cellNumber < SNAKE_CELLS && color < PAL_EXT_COUNT) {
                picture.cell[cellNumber / GAME_COLS][cellNumber % GAME_COLS] =
                    color;
                dirty = true;
            }
            break;
        }
        case GAME_KEY_PAINT_FILL:
            if (in.value < PAL_EXT_COUNT) {
                picture.clear(in.value);
                dirty = true;
            }
            break;
        case GAME_KEY_PAINT_CLEAR:
            picture.clear();
            dirty = true;
            break;
        case GAME_KEY_PAINT_BRIGHTNESS:
            if (in.value >= 5 && in.value <= 100) {
                brightness = in.value;
                dirty = true;
            }
            break;
        default:
            break;
        }
    }

    bool update(uint32_t) override {
        const bool was = dirty;
        dirty = false;
        return was;
    }

    void render(Canvas &canvas, uint32_t) const override {
        memcpy(canvas.cell, picture.cell, sizeof canvas.cell);
    }

    void status(GameStatus &s) const override { s.level = brightness; }
};

//------------------------------------------------------------------------------
// Dice for board games: big enough to read from across the table
//------------------------------------------------------------------------------

class DiceGame : public ArcadeGame {
public:
    static constexpr uint16_t ROLL_MS = 1200;
    static constexpr uint16_t SHUFFLE_MS = 90;

    uint8_t count = 1;
    uint8_t faces[2] = {6, 6};
    bool rolling = false;
    uint32_t rollUntil = 0;
    uint32_t lastShuffle = 0;
    uint32_t rolls = 0;

    DiceGame() { pausable = false; }

    void start(uint32_t seed, uint32_t) override {
        rng.seed(seed);
        over = false;
        rolling = false;
        rolls = 0;
        faces[0] = faces[1] = 6;
    }

    void roll(uint32_t now) {
        rolling = true;
        rollUntil = now + ROLL_MS;
        lastShuffle = 0;
        rolls++;
    }

    void input(const GameInput &in, uint32_t now) override {
        if (in.key == GAME_KEY_ACTION) {
            roll(now);
        } else if (in.key == GAME_KEY_DICE_COUNT &&
                   (in.value == 1 || in.value == 2)) {
            count = in.value;
            roll(now);
        }
    }

    bool update(uint32_t now) override {
        if (!rolling || now - lastShuffle < SHUFFLE_MS) {
            return false;
        }
        lastShuffle = now;
        for (uint8_t &face : faces) {
            face = 1 + rng.below(6);
        }
        if (static_cast<int32_t>(now - rollUntil) >= 0) {
            rolling = false; // the last shuffle is the result
        }
        return true;
    }

    // Pips on a 3x3 grid, bit 8 = top left ... bit 0 = bottom right.
    static uint16_t pips(uint8_t face) {
        static const uint16_t PIPS[7] = {0,     0x010, 0x101, 0x111,
                                         0x145, 0x155, 0x16D};
        return face <= 6 ? PIPS[face] : 0;
    }

    static void drawDie(Canvas &canvas, int16_t row, int16_t col,
                        int16_t size, uint8_t face, uint8_t frame,
                        uint8_t pip) {
        for (int16_t i = 0; i < size; i++) {
            canvas.set(row, col + i, frame);
            canvas.set(row + size - 1, col + i, frame);
            canvas.set(row + i, col, frame);
            canvas.set(row + i, col + size - 1, frame);
        }
        // 12 wide: 2x2 pips at 2, 5, 8; 9 wide: single pips at 2, 4, 6
        const int16_t pipSize = size >= 12 ? 2 : 1;
        const int16_t step = size >= 12 ? 3 : 2;
        const uint16_t bits = pips(face);
        for (int16_t r = 0; r < 3; r++) {
            for (int16_t c = 0; c < 3; c++) {
                if (bits & (1 << (8 - (r * 3 + c)))) {
                    canvas.fill(row + 2 + r * step, col + 2 + c * step,
                                pipSize, pipSize, pip);
                }
            }
        }
    }

    void render(Canvas &canvas, uint32_t) const override {
        canvas.clear();
        const uint8_t pip = rolling ? PAL_DIM : PAL_WHITE;
        if (count == 1) {
            drawDie(canvas, 2, 3, 12, faces[0], PAL_BLUE, pip);
        } else {
            drawDie(canvas, 3, 0, 9, faces[0], PAL_BLUE, pip);
            drawDie(canvas, 3, 9, 9, faces[1], PAL_ORANGE, pip);
        }
    }

    void status(GameStatus &s) const override {
        s.score = rolling ? 0 : faces[0] + (count == 2 ? faces[1] : 0);
        s.level = count;
        if (rolling) {
            strcpy(s.text, "...");
        } else if (!rolls) {
            strcpy(s.text, "");
        } else if (count == 2) {
            snprintf(s.text, sizeof(s.text), "%u + %u = %u", faces[0],
                     faces[1], faces[0] + faces[1]);
        } else {
            snprintf(s.text, sizeof(s.text), "%u", faces[0]);
        }
    }
};

//------------------------------------------------------------------------------
// Hourglass for Activity, Tabu & co.: the sand runs from the upper into the
// lower bulb, at the end the glass blinks red
//------------------------------------------------------------------------------

class HourglassGame : public ArcadeGame {
public:
    static constexpr uint8_t SAND = 42; // inner cells of rows 1..6

    // Half width of the glass per row, centered between columns 8 and 9.
    static uint8_t half(uint8_t row) {
        static const uint8_t HALF[GAME_ROWS] = {7, 7, 6, 5, 4, 3, 2, 1,
                                                1, 2, 3, 4, 5, 6, 7, 7};
        return row < GAME_ROWS ? HALF[row] : 0;
    }

    uint16_t seconds = 60;
    bool running = false;
    bool done = false;
    uint32_t startedAt = 0;
    uint32_t now_ = 0;

    HourglassGame() { pausable = false; }

    void start(uint32_t, uint32_t) override {
        over = false;
        running = false;
        done = false;
    }

    void input(const GameInput &in, uint32_t now) override {
        if (in.key == GAME_KEY_HOURGLASS && in.value >= 5 &&
            in.value <= 3600) {
            seconds = in.value;
        } else if (in.key != GAME_KEY_ACTION) {
            return;
        }
        running = true;
        done = false;
        startedAt = now;
    }

    uint32_t elapsedMs(uint32_t now) const {
        if (!running && !done) {
            return 0;
        }
        const uint32_t elapsed = now - startedAt;
        return elapsed > seconds * 1000u ? seconds * 1000u : elapsed;
    }

    bool update(uint32_t now) override {
        now_ = now;
        if (running && now - startedAt >= seconds * 1000u) {
            running = false;
            done = true;
        }
        return running || done; // grains fall, the end blinks
    }

    // Fills `count` cells of a row from the middle outwards.
    static void fillRow(Canvas &canvas, uint8_t row, uint8_t count,
                        uint8_t color) {
        for (uint8_t i = 0; i < count; i++) {
            const int16_t offset = i / 2;
            canvas.set(row, i % 2 ? 9 + offset : 8 - offset, color);
        }
    }

    void render(Canvas &canvas, uint32_t now) const override {
        canvas.clear();
        const bool blinkOff = done && (now / 400) % 2;
        const uint8_t glass = done ? (blinkOff ? PAL_OFF : PAL_RED) : PAL_DIM;
        for (uint8_t row = 0; row < GAME_ROWS; row++) {
            const uint8_t width = half(row);
            if (row == 0 || row == GAME_ROWS - 1) {
                canvas.fill(row, 9 - width, 1, 2 * width, glass);
            } else {
                canvas.set(row, 9 - width, glass);
                canvas.set(row, 8 + width, glass);
            }
        }
        const uint32_t total = seconds * 1000u;
        const uint8_t fallen = static_cast<uint8_t>(
            (static_cast<uint64_t>(elapsedMs(now)) * SAND) / total);
        // Upper bulb: the sand sits on the neck, its surface sinks.
        uint8_t left = SAND - fallen;
        for (uint8_t row = 6; row >= 1 && left; row--) {
            const uint8_t inner = 2 * (half(row) - 1);
            const uint8_t n = left < inner ? left : inner;
            fillRow(canvas, row, n, PAL_YELLOW);
            left -= n;
        }
        // Lower bulb fills from the bottom.
        uint8_t heap = fallen;
        for (uint8_t row = 14; row >= 9 && heap; row--) {
            const uint8_t inner = 2 * (half(row) - 1);
            const uint8_t n = heap < inner ? heap : inner;
            fillRow(canvas, row, n, PAL_YELLOW);
            heap -= n;
        }
        if (running && (now / 150) % 2) {
            canvas.set(7, 8, PAL_YELLOW); // a falling grain in the neck
        } else if (running) {
            canvas.set(8, 9, PAL_YELLOW);
        }
    }

    void status(GameStatus &s) const override {
        const uint32_t left = seconds - elapsedMs(now_) / 1000;
        s.level = seconds;
        snprintf(s.text, sizeof(s.text), "%u:%02u", left / 60, left % 60);
        strcpy(s.text2, done ? "done" : running ? "run" : "ready");
    }
};

//------------------------------------------------------------------------------
// Which games keep a high score list
//------------------------------------------------------------------------------

inline bool gameHasHighscore(uint8_t game) {
    switch (game) {
    case GAME_TETRIS:
    case GAME_SNAKE:
    case GAME_SIMON:
    case GAME_FLAPPY:
    case GAME_BREAKOUT:
    case GAME_INVADERS:
    case GAME_FROGGER:
    case GAME_HANGMAN:
    case GAME_CLOCK_QUIZ:
    case GAME_WORD_FIND:
    case GAME_MATH:
        return true;
    default:
        return false;
    }
}

} // namespace wc24h
