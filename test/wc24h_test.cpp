// Host unit test for the WordClock24h display mode engine.
//   g++ -std=gnu++11 -Wall -Wextra -Werror -Iinclude test/wc24h_test.cpp
//   ./a.out

#include "WC24h/Ambient.h"
#include "WC24h/Arcade.h"
#include "WC24h/Calendar.h"
#include "WC24h/Games.h"
#include "WC24h/NightTimer.h"
#include "WC24h/Overlay.h"
#include "WC24h/Wc24hDisplay.h"

#include <cstdio>
#include <string>

namespace {

// Front panel letters, umlauts written as their base letter.
const char *const LETTERS[wc24h::WC_ROWS] = {
    "ESAISTOVIERTELEINS", "DREINERSECHSIEBENE", "ELFUNFNEUNVIERACHT",
    "NULLZWEINZWOLFZEHN", "UNDOZWANZIGVIERZIG", "DREISSIGFUNFZIGUHR",
    "MINUTENIVORUNDNACH", "EINDREIVIERTELHALB", "SIEBENEUNULLZWEINE",
    "FUNFSECHSNACHTVIER", "DREINSUNDAELFEZEHN", "ZWANZIGGRADREISSIG",
    "VIERZIGZWOLFUNFZIG", "MINUTENUHREFRUHVOR", "ABENDSMITTERNACHTS",
    "MORGENSWARMMITTAGS",
};

std::string toText(const wc24h::WordSet &words) {
    std::string text;
    for (uint8_t idx = 1; idx < wc24h::WP_COUNT; idx++) {
        if (!words[idx]) {
            continue;
        }
        const wc24h::WORD_ILLUMINATION &w = wc24h::illumination[idx];
        if (!text.empty()) {
            text += ' ';
        }
        text.append(LETTERS[w.row] + w.col,
                    w.len & wc24h::ILLUMINATION_LEN_MASK);
    }
    return text;
}

int failures = 0;

void expectTime(uint8_t mode, uint8_t hh, uint8_t mm, const char *expected) {
    wc24h::WordSet words;
    wc24h::fillWords(mode, hh, mm, words);
    const std::string actual = toText(words);
    if (actual != expected) {
        printf("FAIL mode %u (%s) %02u:%02u\n  expected: %s\n  actual:   %s\n",
               mode, wc24h::modeName(mode), hh, mm, expected, actual.c_str());
        failures++;
    }
}

void expectTemperature(uint8_t index, bool valid, const char *expected) {
    wc24h::WordSet words;
    const bool ok = wc24h::fillTemperatureWords(index, words);
    const std::string actual = toText(words);
    if (ok != valid || actual != expected) {
        printf(
            "FAIL temperature index %u\n  expected: %d %s\n  actual:   %d %s\n",
            index, valid, expected, ok, actual.c_str());
        failures++;
    }
}

void expectNight(bool actual, bool expected, const char *what) {
    if (actual != expected) {
        printf("FAIL night timer: %s\n", what);
        failures++;
    }
}

uint8_t nightFlags(bool active, bool switchOn, uint8_t fromDay, uint8_t toDay) {
    return (active ? wc24h::NIGHT_FLAG_ACTIVE : 0) |
           (switchOn ? wc24h::NIGHT_FLAG_SWITCH_ON : 0) | (fromDay << 3) |
           toDay;
}

void testNightTimers() {
    using wc24h::nightTimerCoversWeekday;
    using wc24h::nightTimerFires;

    // Monday to Friday.
    const uint8_t workdays = nightFlags(true, false, 1, 5);
    expectNight(nightTimerCoversWeekday(workdays, 0), false, "Mo-Fr on Sun");
    expectNight(nightTimerCoversWeekday(workdays, 1), true, "Mo-Fr on Mon");
    expectNight(nightTimerCoversWeekday(workdays, 5), true, "Mo-Fr on Fri");
    expectNight(nightTimerCoversWeekday(workdays, 6), false, "Mo-Fr on Sat");

    // Saturday to Wednesday wraps around the weekend.
    const uint8_t wrapping = nightFlags(true, false, 6, 3);
    expectNight(nightTimerCoversWeekday(wrapping, 6), true, "Sa-We on Sat");
    expectNight(nightTimerCoversWeekday(wrapping, 0), true, "Sa-We on Sun");
    expectNight(nightTimerCoversWeekday(wrapping, 3), true, "Sa-We on Wed");
    expectNight(nightTimerCoversWeekday(wrapping, 4), false, "Sa-We on Thu");
    expectNight(nightTimerCoversWeekday(wrapping, 5), false, "Sa-We on Fri");

    // A single day.
    const uint8_t sunday = nightFlags(true, true, 0, 0);
    expectNight(nightTimerCoversWeekday(sunday, 0), true, "Su-Su on Sun");
    expectNight(nightTimerCoversWeekday(sunday, 1), false, "Su-Su on Mon");

    // Switch off at 22:30 on workdays.
    expectNight(nightTimerFires(workdays, 22, 30, true, 2, 22, 30), true,
                "switch off when on");
    expectNight(nightTimerFires(workdays, 22, 30, false, 2, 22, 30), false,
                "no switch off when already off");
    expectNight(nightTimerFires(workdays, 22, 30, true, 2, 22, 31), false,
                "wrong minute");
    expectNight(nightTimerFires(workdays, 22, 30, true, 6, 22, 30), false,
                "wrong weekday");
    expectNight(nightTimerFires(nightFlags(false, false, 1, 5), 22, 30, true, 2,
                                22, 30),
                false, "inactive timer");
    expectNight(nightTimerFires(sunday, 7, 0, false, 0, 7, 0), true,
                "switch on when off");
}

void expectDate(int32_t actual, int32_t year, uint8_t month, uint8_t day,
                const char *what) {
    const int32_t expected = wc24h::daysFromCivil(year, month, day);
    if (actual != expected) {
        printf("FAIL %s: expected %04d-%02u-%02u, %d days off\n", what, year,
               month, day, actual - expected);
        failures++;
    }
}

int32_t rangeStart(uint8_t dateCode, int32_t year) {
    wc24h::Overlay overlay = {};
    overlay.dateCode = dateCode;
    int32_t start = 0;
    wc24h::overlayRangeStart(overlay, year, start);
    return start;
}

void testCalendar() {
    using namespace wc24h;
    expectDate(daysFromCivil(1970, 1, 1), 1970, 1, 1, "epoch");
    if (daysFromCivil(1970, 1, 1) != 0 || weekdayFromDays(0) != 4) {
        printf("FAIL epoch is not a Thursday at day 0\n");
        failures++;
    }
    if (weekdayFromDays(daysFromCivil(2026, 10, 1)) != 4) {
        printf("FAIL 2026-10-01 is a Thursday\n");
        failures++;
    }

    expectDate(easterSunday(2000), 2000, 4, 23, "Easter 2000");
    expectDate(easterSunday(2019), 2019, 4, 21, "Easter 2019");
    expectDate(easterSunday(2024), 2024, 3, 31, "Easter 2024");
    expectDate(easterSunday(2025), 2025, 4, 20, "Easter 2025");
    expectDate(easterSunday(2026), 2026, 4, 5, "Easter 2026");

    // Leap year: the original add_days() is a day off here.
    expectDate(rangeStart(DATE_CARNIVAL_MONDAY, 2024), 2024, 2, 12,
               "Carnival Monday 2024");
    expectDate(rangeStart(DATE_CARNIVAL_MONDAY, 2025), 2025, 3, 3,
               "Carnival Monday 2025");
    expectDate(rangeStart(DATE_CARNIVAL_MONDAY, 2026), 2026, 2, 16,
               "Carnival Monday 2026");

    expectDate(rangeStart(DATE_ADVENT1, 2023), 2023, 12, 3, "Advent 1 2023");
    expectDate(rangeStart(DATE_ADVENT4, 2023), 2023, 12, 24, "Advent 4 2023");
    expectDate(rangeStart(DATE_ADVENT1, 2024), 2024, 12, 1, "Advent 1 2024");
    expectDate(rangeStart(DATE_ADVENT1, 2025), 2025, 11, 30, "Advent 1 2025");
    expectDate(rangeStart(DATE_ADVENT2, 2026), 2026, 12, 6, "Advent 2 2026");
    expectDate(rangeStart(DATE_ADVENT1, 2026), 2026, 11, 29, "Advent 1 2026");
}

void expectSelected(int8_t actual, int8_t expected, const char *what) {
    if (actual != expected) {
        printf("FAIL overlay selection %s: expected %d, got %d\n", what,
               expected, actual);
        failures++;
    }
}

void testOverlaySelection() {
    using namespace wc24h;
    Overlay overlays[4] = {};

    // 0: date ticker every 15 minutes, always
    overlays[0].type = OVERLAY_DATE;
    overlays[0].flags = OVERLAY_FLAG_ACTIVE;
    overlays[0].interval = 15;
    // 1: Christmas tree every 15 minutes from Advent 1 until 3 January
    overlays[1].type = OVERLAY_ICON;
    overlays[1].flags = OVERLAY_FLAG_ACTIVE;
    overlays[1].interval = 15;
    overlays[1].dateCode = DATE_ADVENT1;
    overlays[1].days = 36;
    // 2: temperature every 5 minutes, always
    overlays[2].type = OVERLAY_TEMPERATURE;
    overlays[2].flags = OVERLAY_FLAG_ACTIVE;
    overlays[2].interval = 5;
    // 3: inactive New Year's fireworks every minute on 31.12. for 2 days
    overlays[3].type = OVERLAY_ICON;
    overlays[3].interval = 1;
    overlays[3].dateCode = DATE_FIXED;
    overlays[3].month = 12;
    overlays[3].day = 31;
    overlays[3].days = 2;

    expectSelected(selectOverlay(overlays, 4, 2026, 7, 1, 7), -1,
                   "nothing due at :07");
    expectSelected(selectOverlay(overlays, 4, 2026, 7, 1, 5), 2,
                   "temperature at :05");
    expectSelected(selectOverlay(overlays, 4, 2026, 7, 1, 30), 0,
                   "longest interval wins at :30");
    expectSelected(selectOverlay(overlays, 4, 2026, 12, 6, 30), 1,
                   "dated overlay wins a tie in Advent");
    expectSelected(selectOverlay(overlays, 4, 2027, 1, 1, 30), 1,
                   "Advent range reaches into January");
    expectSelected(selectOverlay(overlays, 4, 2027, 1, 3, 30), 1,
                   "Advent range covers its last day");
    expectSelected(selectOverlay(overlays, 4, 2027, 1, 4, 30), 0,
                   "Advent range ends after 36 days");

    overlays[3].flags = OVERLAY_FLAG_ACTIVE;
    expectSelected(selectOverlay(overlays, 4, 2026, 12, 31, 7), 3,
                   "fixed date on 31.12.");
    expectSelected(selectOverlay(overlays, 4, 2027, 1, 1, 7), 3,
                   "fixed date range across New Year");
    expectSelected(selectOverlay(overlays, 4, 2027, 1, 2, 7), -1,
                   "fixed date range over");
}

} // namespace

void expectGame(bool ok, const char *what) {
    if (!ok) {
        printf("FAIL games: %s\n", what);
        failures++;
    }
}

void testTetris() {
    wc24h::Tetris t;
    t.start(42);
    expectGame(!t.over && t.score == 0 && t.level() == 1, "fresh game");
    expectGame(t.intervalMs() == 600, "level 1 gravity");

    // Walls stop sideways moves.
    for (uint8_t i = 0; i < 20; i++) {
        t.move(-1);
    }
    expectGame(!t.move(-1), "left wall blocks");

    // A filled bottom row except for one gap is cleared by an I piece.
    t.start(1);
    for (uint8_t col = 0; col < wc24h::TETRIS_COLS - 4; col++) {
        t.board[wc24h::TETRIS_ROWS - 1][col] = 1;
    }
    t.piece = 0; // I, horizontal in row 1 of its 4x4 box
    t.rotation = 0;
    t.x = wc24h::TETRIS_COLS - 4;
    t.y = 0;
    t.hardDrop();
    bool bottomEmpty = true;
    for (uint8_t col = 0; col < wc24h::TETRIS_COLS; col++) {
        bottomEmpty &= t.board[wc24h::TETRIS_ROWS - 1][col] == 0;
    }
    expectGame(bottomEmpty && t.lines == 1, "full row cleared");
    expectGame(t.score >= 40, "line scores points");

    // Stacking pieces to the top ends the game.
    t.start(7);
    for (uint16_t i = 0; i < 200 && !t.over; i++) {
        t.hardDrop();
    }
    expectGame(t.over, "stack to the top ends the game");
}

void testSnake() {
    wc24h::Snake s;
    s.start(3);
    expectGame(s.length == 3 && !s.over, "fresh snake");
    expectGame(!s.occupies(s.food), "food not on the snake");

    // Reversing into itself is ignored.
    s.input(wc24h::GAME_KEY_LEFT);
    expectGame(s.nextDirection == wc24h::GAME_KEY_RIGHT, "no reverse");

    // Eating grows the snake.
    const uint16_t ahead = s.segment(0) + 1;
    s.food = ahead;
    expectGame(s.step() && s.length == 4 && s.score == 10, "eat and grow");

    // Running into the right wall ends the game.
    for (uint8_t i = 0; i < wc24h::GAME_COLS && !s.over; i++) {
        s.food = 0;
        s.step();
    }
    expectGame(s.over && !s.won, "wall ends the game");
}

void testCountTimer() {
    wc24h::CountTimer t;
    t.startCountdown(90, 1000);
    expectGame(t.displaySeconds(1000) == 90, "countdown start");
    expectGame(t.displaySeconds(1500) == 90, "countdown rounds up");
    expectGame(t.displaySeconds(61000) == 30, "countdown after 60 s");
    t.togglePause(61000);
    expectGame(t.displaySeconds(200000) == 30, "pause holds the time");
    t.togglePause(200000);
    expectGame(!t.finished(229000) && t.finished(230000), "countdown ends");
    expectGame(t.displaySeconds(500000) == 0, "countdown stays at zero");

    t.startStopwatch(0);
    expectGame(t.displaySeconds(125400) == 125, "stopwatch counts up");
    expectGame(!t.finished(9999999), "stopwatch never finishes");
}


// Plays every arcade game for a while with random keys; nothing may crash
// or draw outside the palette.
void testArcadeSmoke() {
    wc24h::TetrisGame tetris;
    wc24h::SnakeGame snake;
    wc24h::SimonGame simon;
    wc24h::FlappyGame flappy;
    wc24h::BreakoutGame breakout;
    wc24h::InvadersGame invaders;
    wc24h::FroggerGame frogger;
    wc24h::PongGame pong;
    wc24h::Connect4Game connect4;
    wc24h::ReactionGame reaction;
    wc24h::HangmanGame hangman;
    wc24h::ClockQuizGame quiz;
    wc24h::WordFindGame words;
    wc24h::MathGame math;
    wc24h::PaintGame paint;
    wc24h::ArcadeGame *games[] = {&tetris, &snake, &simon, &flappy,
                                  &breakout, &invaders, &frogger, &pong,
                                  &connect4, &reaction, &hangman, &quiz,
                                  &words, &math, &paint};
    static const uint8_t KEYS[] = {1, 2, 3, 4, 5, 11, 12, 13, 14, 20,
                                   21, 22, 30, 31, 65, 69, 78, 90};
    wc24h::GameRng rng;
    rng.seed(99);
    for (wc24h::ArcadeGame *game : games) {
        uint32_t now = 1000;
        game->start(1234, now);
        for (uint32_t i = 0; i < 20000 && !game->over; i++) {
            now += 7;
            wc24h::GameInput in = {KEYS[rng.below(sizeof KEYS)],
                                   static_cast<uint8_t>(1 + rng.below(2)),
                                   rng.below(5000)};
            if (i % 5 == 0) {
                game->input(in, now);
            }
            game->update(now);
            if (i % 50 == 0) {
                wc24h::Canvas canvas;
                game->render(canvas, now);
                for (auto &row : canvas.cell) {
                    for (uint8_t v : row) {
                        expectGame(v < wc24h::PAL_COUNT, "palette index");
                    }
                }
                wc24h::GameStatus status;
                game->status(status);
            }
        }
    }
}

void testArcadeRules() {
    const wc24h::GameInput none = {0, 1, 0};
    (void)none;

    // Connect four: four in a column wins.
    wc24h::Connect4Game c4;
    c4.start(1, 0);
    for (uint8_t i = 0; i < 3; i++) {
        c4.drop(0, 0); // player 1
        c4.drop(1, 0); // player 2
    }
    c4.drop(0, 0);
    expectGame(c4.over && c4.winner == 1, "connect four column win");

    // The computer takes a winning move.
    c4.start(1, 0);
    c4.cpu = true;
    c4.board[5][4] = c4.board[5][5] = c4.board[5][6] = 2;
    expectGame(c4.cpuChoice() == 3, "connect four cpu wins");

    // Hangman: guessing all letters solves the word.
    wc24h::HangmanGame hm;
    hm.start(5, 0);
    for (const char *p = hm.word; *p; p++) {
        hm.input({static_cast<uint8_t>(*p), 1, 0}, 0);
    }
    expectGame(hm.over && hm.won, "hangman solved");

    // Math: the right answer counts.
    wc24h::MathGame mg;
    mg.start(8, 0);
    mg.input({wc24h::GAME_KEY_ANSWER, 1, mg.answer}, 0);
    expectGame(mg.correct == 1, "math right answer");

    // Clock quiz: 12 hour answers are accepted for the afternoon.
    wc24h::ClockQuizGame cq;
    cq.start(4, 0);
    const uint32_t answer = (cq.hh % 12) * 100 + cq.mm;
    cq.input({wc24h::GAME_KEY_ANSWER, 1, answer}, 0);
    expectGame(cq.correct == 1, "clock quiz answer");

    // Word find: tapping the target word letter by letter finds it.
    wc24h::WordFindGame wf;
    wf.start(3, 0);
    const wc24h::WORD_ILLUMINATION &w = wc24h::illumination[wf.target];
    for (uint8_t i = 0; i < (w.len & wc24h::ILLUMINATION_LEN_MASK); i++) {
        wf.input({wc24h::GAME_KEY_TAP, 1,
                  static_cast<uint32_t>(w.row * wc24h::GAME_COLS + w.col + i)},
                 0);
    }
    expectGame(wf.found == 1, "word find tap sequence");

    // Simon: repeating the sequence extends it, a wrong color ends it.
    wc24h::SimonGame sg;
    sg.start(2, 0);
    sg.phase = wc24h::SimonGame::PHASE_INPUT;
    sg.input({static_cast<uint8_t>(wc24h::GAME_KEY_COLOR1 + sg.seq[0]), 1, 0},
             0);
    expectGame(sg.len == 2 && !sg.over, "simon grows");
    sg.phase = wc24h::SimonGame::PHASE_INPUT;
    sg.pos = 0;
    sg.input({static_cast<uint8_t>(wc24h::GAME_KEY_COLOR1 +
                                   (sg.seq[0] + 1) % 4),
              1, 0},
             0);
    expectGame(sg.over, "simon wrong color");

    // Reaction: pressing too early gives the other player the point.
    wc24h::ReactionGame rg;
    rg.start(1, 0);
    rg.input({wc24h::GAME_KEY_ACTION, 1, 0}, 10);
    expectGame(rg.s2 == 1 && rg.s1 == 0, "reaction foul");

    // Paint keeps its picture.
    wc24h::PaintGame pg;
    pg.start(0, 0);
    pg.input({wc24h::GAME_KEY_PAINT_SET, 1, (2 * 18 + 3) * 16 + 4}, 0);
    wc24h::Canvas canvas;
    pg.render(canvas, 0);
    expectGame(canvas.cell[2][3] == 4, "paint sets a cell");

    // 3x5 text and scores stay on the canvas.
    canvas.clear();
    wc24h::drawScore(canvas, 12345678, wc24h::PAL_WHITE);
    canvas.clear();
    canvas.centeredText(5, "99x9=", wc24h::PAL_WHITE);
}

void testAmbientAndCalendar() {
    wc24h::Ambient ambient;
    wc24h::RgbFrame frame;
    frame.clear();
    for (uint8_t e = 1; e < wc24h::AMBIENT_COUNT; e++) {
        ambient.start(e, 7, 0);
        for (uint32_t t = 0; t < 200000; t += 97) {
            ambient.render(frame, t);
        }
    }
    expectGame(wc24h::daysUntil(2026, 10, 26, 12, 24) == 59, "days until xmas");
    expectGame(wc24h::daysUntil(2026, 12, 25, 12, 24) == 364, "next year");
    expectGame(wc24h::daysUntil(2026, 3, 1, 3, 1) == 0, "today");
    const float full = wc24h::moonPhase(2024, 1, 25);
    const float fresh = wc24h::moonPhase(2024, 1, 11);
    expectGame(full > 0.45f && full < 0.55f, "full moon 2024-01-25");
    expectGame(fresh < 0.05f || fresh > 0.95f, "new moon 2024-01-11");
    char icon[wc24h::MOON_SIZE * wc24h::MOON_SIZE + 1];
    wc24h::moonIcon(0.5f, icon);
    expectGame(icon[6 * wc24h::MOON_SIZE + 6] == '7', "full moon lit");
    wc24h::moonIcon(0.25f, icon);
    expectGame(icon[6 * wc24h::MOON_SIZE + 9] == '7' &&
                   icon[6 * wc24h::MOON_SIZE + 2] == '9',
               "first quarter lit on the right");
}

int main() {
    testAmbientAndCalendar();
    testArcadeSmoke();
    testArcadeRules();
    testTetris();
    testSnake();
    testCountTimer();
    testNightTimers();
    testCalendar();
    testOverlaySelection();

    // Words are listed in table order, not in reading order.
    expectTime(4, 13, 37,
               "ES IST DREI ZEHN UHR UND SIEBEN UND DREISSIG MINUTEN");
    expectTime(5, 13, 37,
               "ES IST SIEBEN UND DREISSIG MINUTEN NACH DREI ZEHN UHR");

    // Full hour 1 o'clock: "EIN UHR", not "EINS UHR".
    expectTime(0, 1, 0, "ES IST EIN UHR");
    expectTime(0, 13, 0, "ES IST EIN UHR");

    // Hour offset of the minute table: quarter to two refers to hour 14.
    expectTime(6, 13, 45, "ES IST DREIVIERTEL ZWEI");
    expectTime(18, 13, 45, "ES IST VIERTEL VOR ZWEI");
    expectTime(6, 13, 15, "ES IST VIERTEL ZWEI");
    expectTime(18, 13, 15, "ES IST VIERTEL NACH EINS");

    // Wrap around midnight.
    expectTime(24, 0, 0, "ES IST MITTERNACHT");
    expectTime(6, 23, 45, "ES IST DREIVIERTEL ZWOLF");

    // Out of range input lights nothing.
    expectTime(wc24h::CLOCK_MODES_COUNT, 12, 0, "");
    expectTime(0, 24, 0, "");
    expectTime(0, 12, 60, "");

    expectTemperature(19, false, "");
    expectTemperature(20, true, "ZEHN GRAD");
    expectTemperature(21, true, "ZEHN EIN HALB GRAD");
    expectTemperature(79, true, "NEUN UND DREISSIG EIN HALB GRAD WARM");
    expectTemperature(80, false, "");

    for (uint8_t mode = 0; mode < wc24h::CLOCK_MODES_COUNT; mode++) {
        for (uint8_t hh = 0; hh < wc24h::HOUR_COUNT; hh++) {
            for (uint8_t mm = 0; mm < wc24h::MINUTE_COUNT; mm++) {
                wc24h::WordSet words;
                wc24h::fillWords(mode, hh, mm, words);
                if (toText(words).empty()) {
                    printf("FAIL mode %u %02u:%02u lights no word\n", mode, hh,
                           mm);
                    failures++;
                }
            }
        }
    }

    if (failures) {
        printf("%d failure(s)\n", failures);
        return 1;
    }
    printf("all wc24h tests passed\n");
    return 0;
}
