// Host unit test for the WordClock24h display mode engine.
//   g++ -std=gnu++11 -Wall -Wextra -Werror -Iinclude test/wc24h_test.cpp
//   ./a.out

#include "WC24h/NightTimer.h"
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

} // namespace

int main() {
    testNightTimers();

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
