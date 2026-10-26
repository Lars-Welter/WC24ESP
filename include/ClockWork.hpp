#include "ClockWork.h"
#include "I2CBus.h"
#include "NeoMultiFeature.hpp"
#include "OpenWeatherMap.h"
#include "SensitiveData.h"
#include "TransitionTypes/Transition.h"
#include "WC24h/Ambient.h"
#include "WC24h/Arcade.h"
#include "WC24h/Calendar.h"
#include "WC24h/CurrentWeather.h"
#include "WC24h/Games.h"
#include "WC24h/NightTimer.h"
#include "WC24h/Overlay.h"
#include "WC24h/Text.h"
#include "WC24h/Wc24hDisplay.h"
#include "WC24h/Wc24hIcons.h"
#include "WordClockState.h"
#include "WordClockTypes/ClockType.hpp"
#include "math.h"
#include <Arduino.h>
#include <BH1750.h>
#include <Preferences.h>

namespace {
void stopAlarm();
} // namespace

#define MAX_LED_COUNT 300

BH1750 lightMeter;
OpenWMap weather;
CurrentWeather currentWeather;

uint8_t activeLedPin = UINT8_MAX;
uint8_t activeLedColorType = UINT8_MAX;
uint16_t activeLedCount = 0;

//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

/*
Automatic Brightness Control (enabled/disabled by G.autoBrightEnabled)

1. Measure ambient light with sensor BH1750, if available. If not, use LDR.
2. Derive the ledGain for the LEDs 0.0 - 100.0%
3. When ledGain changed, set parametersChanged = true

Inputs:
G.autoBrightEnabled, G.autoBrightMin, G.autoBrightMax, G.autoBrightPeak
AUTOBRIGHT_LDR_RESBRIGHT, AUTOBRIGHT_LDR_RESDARK, AUTOBRIGHT_LDR_RESDIVIDER

Outputs:
lux = Ambient light [LUX]
ledGain = gain for LEDs: 0.0-100.0% (n % of the configured bri:effectBri)
*/

void ClockWork::loopAutoBrightLogic() {
    if (G.autoBrightEnabled != 1)
        return;

    if (G.autoBrightMin == G.autoBrightMax) {
        // If min and max are identical, nothing needs to be measured...
        // Besides: map() would crash with division by zero in this case
        ledGain = G.autoBrightMax;
        return;
    }

    float ledGainOld = ledGain;
    float luxNow = -1.0;

    if (bh1750Initialized && lightMeter.measurementReady()) {
        /*
        If BH1750 is not available or did not return a value, try to use LDR
        */
        luxNow = lightMeter.readLightLevel(); // 0.0-54612.5 LUX
        autoBrightUsingBH1750 = true;
    }

    else if (luxNow < 0) {
        /*
        The lux value is considerably misrepresented upwards at ADC values above
        980. As 980 with an LDR5528 corresponds to approx. 1500 lux, but usually
        no more than 1000 lux are reached indoors even on sunny days, a fixed
        upper measurement limit of 980 is set here. Regardless of this, a
        maximum value of 1023 has to be applied in any case in order to avoid
        division by zero (analogRead is 0-1023)!
        */
        uint16_t adcValue = analogRead(A0);
        if (adcValue > 980) {
            adcValue = 980;
        }

        luxNow = (adcValue * AUTOBRIGHT_LDR_RESDARK * 10) /
                 (AUTOBRIGHT_LDR_RESBRIGHT * AUTOBRIGHT_LDR_RESDIVIDER *
                  (1024 - adcValue));
        autoBrightUsingBH1750 = false;
    }

    else {
        /*
        If luxNow is still negative, no data could be retrieved from BH1750 or
        LDR. We return to preserve the previous ledGain (potentially default).
        Otherwise we may end up in a blinking light.
        */
        return;
    }

    /*
    Even in HIGH_RES_MODE, the BH1750 only has a precision of 1 lux. The decimal
    point values supplied by the library should therefore be considered as
    'noise' and their use makes little to no sense. Also: in order to not
    trigger parametersChanged more often than necessary, the value gets rounded.
    */

    lux = round(luxNow);

    ledGain = map(lux, 0, G.autoBrightPeak, G.autoBrightMin, G.autoBrightMax);

    // Ensure ledGain meets the defined limits.
    uint16_t minimum = min(G.autoBrightMin, G.autoBrightMax);
    uint16_t maximum = max(G.autoBrightMin, G.autoBrightMax);

    if (ledGain >= maximum) {
        ledGain = maximum;
    }
    if (ledGain <= minimum) {
        ledGain = minimum;
    }

    if (ledGainOld != ledGain) {
        parametersChanged = true;
    }
}

//------------------------------------------------------------------------------

void ClockWork::initHardwareButtons() {
    hardwareButtonController.begin(G.hardwarePins);
}

//------------------------------------------------------------------------------

void ClockWork::toggleHardwareButtonPower() {
    led.setState(!led.getState());

    requestHardwareButtonDisplayRefresh();
}

//------------------------------------------------------------------------------

void ClockWork::nextHardwareButtonMode() {
    static const uint8_t modes[] = {
        COMMAND_MODE_WORD_CLOCK,    COMMAND_MODE_SECONDS,
        COMMAND_MODE_SCROLLINGTEXT, COMMAND_MODE_RAINBOWCYCLE,
        COMMAND_MODE_RAINBOW,       COMMAND_MODE_COLOR,
        COMMAND_MODE_DIGITAL_CLOCK, COMMAND_MODE_SYMBOL};

    uint8_t nextMode = modes[0];
    for (uint8_t i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        if (G.prog == modes[i] ||
            (G.prog == COMMAND_IDLE && modes[i] == COMMAND_MODE_WORD_CLOCK)) {
            nextMode = modes[(i + 1) % (sizeof(modes) / sizeof(modes[0]))];
            break;
        }
    }

    G.prog = nextMode;
    G.progInit = true;
    parametersChanged = true;
}

//------------------------------------------------------------------------------

void ClockWork::nextHardwareButtonTransition() {
    static const uint8_t transitions[] = {
        NO_TRANSITION, ROLL_UP, ROLL_DOWN,   SHIFT_LEFT, SHIFT_RIGHT,
        FADE,          LASER,   MATRIX_RAIN, BALLS,      FIRE,
        SNAKE,         EXPLODE, TELETYPE,    CUBE,       DROP,
        SQUEEZE,       FLICKER, COLORED,     RANDOM};

    uint8_t nextTransition = transitions[0];
    for (uint8_t i = 0; i < sizeof(transitions) / sizeof(transitions[0]); i++) {
        if (G.transitionType == transitions[i]) {
            nextTransition = transitions[(i + 1) % (sizeof(transitions) /
                                                    sizeof(transitions[0]))];
            break;
        }
    }

    G.transitionType = nextTransition;
    G.prog = COMMAND_MODE_TRANSITION;
    G.progInit = true;
    parametersChanged = true;
}

//------------------------------------------------------------------------------

void ClockWork::increaseHardwareButtonBrightness() {
    float brightness = G.color[Foreground].B;
    if (brightness <= 0.0f) {
        brightness = 0.1f;
    } else if (brightness >= 1.0f) {
        brightness = 0.1f;
    } else {
        brightness += 0.1f;
        if (brightness > 1.0f) {
            brightness = 1.0f;
        }
    }

    G.color[Foreground].B = brightness;
    requestHardwareButtonDisplayRefresh();
}

//------------------------------------------------------------------------------

void ClockWork::nextHardwareButtonHue() {
    float hue = G.color[Foreground].H + (30.0f / 360.0f);
    if (hue >= 1.0f) {
        hue -= 1.0f;
    }
    G.color[Foreground].H = hue;
    requestHardwareButtonDisplayRefresh();
}

//------------------------------------------------------------------------------

void ClockWork::requestHardwareButtonDisplayRefresh() {
    parametersChanged = true;
    if (G.prog == COMMAND_IDLE) {
        G.prog = COMMAND_MODE_WORD_CLOCK;
    }
}

//------------------------------------------------------------------------------

void ClockWork::handleHardwareButtonAction(HardwareButtonAction action) {
    if (G.prog == COMMAND_MODE_ALARM &&
        action != HardwareButtonAction::None) {
        stopAlarm(); // any key ends the alarm
        return;
    }
    switch (action) {
    case HardwareButtonAction::TogglePower:
        toggleHardwareButtonPower();
        break;
    case HardwareButtonAction::NextMode:
        nextHardwareButtonMode();
        break;
    case HardwareButtonAction::NextTransition:
        nextHardwareButtonTransition();
        break;
    case HardwareButtonAction::IncreaseBrightness:
        increaseHardwareButtonBrightness();
        break;
    case HardwareButtonAction::NextHue:
        nextHardwareButtonHue();
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------

void ClockWork::loopHardwareButtons() {
    const HardwareButtonAction action = hardwareButtonController.loop();
    if (action != HardwareButtonAction::None) {
        handleHardwareButtonAction(action);
    }
}

//------------------------------------------------------------------------------

ClockType *ClockWork::getPointer(uint8_t type) {
    switch (type) {
#define X(name, id, var, i18n)                                                 \
    case name:                                                                 \
        return &var;
        CLOCK_TYPES_LIST
#undef X
    default:
        return nullptr;
    }
}

//------------------------------------------------------------------------------

void ClockWork::initLedStrip(uint8_t num) {
    NeoMultiFeature::setColortype(num);
    const uint16_t ledCount = MAX_LED_COUNT * getLedsPerLetter(G.buildTypeDef);

    if (activeLedPin != G.hardwarePins.led || activeLedColorType != num ||
        activeLedCount != ledCount) {
        deleteActiveLedStrip();
        activeLedPin = G.hardwarePins.led;
        activeLedColorType = num;
        activeLedCount = ledCount;
    }

    if (activeLedStrip != nullptr) {
        return;
    }

#ifdef ESP8266
    if (num == Grbw) {
        if (G.hardwarePins.led == LED_PIN) {
            activeLedStrip =
                new RgbwLedStripAdapter<Neo800KbpsMethod>(ledCount);
        } else {
            activeLedStrip =
                new RgbwLedStripAdapter<NeoEsp8266BitBangWs2812xMethod>(
                    ledCount, G.hardwarePins.led);
        }
    } else if (G.hardwarePins.led == LED_PIN) {
        activeLedStrip = new RgbLedStripAdapter<Neo800KbpsMethod>(ledCount);
    } else {
        activeLedStrip = new RgbLedStripAdapter<NeoEsp8266BitBangWs2812xMethod>(
            ledCount, G.hardwarePins.led);
    }
#else
#if defined(ESP32)
    pinMode(G.hardwarePins.led, OUTPUT);
#endif
    // I2S with DMA instead of RMT: on long chains (289 LEDs on the
    // WordClock24h) WiFi interrupts delay the RMT refill, the strip latches
    // mid-frame and stray letters light up in wrong colors.
    if (num == Grbw) {
        activeLedStrip = new RgbwLedStripAdapter<NeoEsp32I2s1Ws2812xMethod>(
            ledCount, G.hardwarePins.led);
    } else {
        activeLedStrip = new RgbLedStripAdapter<NeoEsp32I2s1Ws2812xMethod>(
            ledCount, G.hardwarePins.led);
    }
#endif
}

//------------------------------------------------------------------------------

uint32_t ClockWork::num32BitWithOnesAccordingToColumns() {
    return (1U << usedClockType->colsWordMatrix()) - 1;
}

//------------------------------------------------------------------------------

void sendJsonToClient(uint8_t client_nr, const JsonDocument &doc) {
    if (doc.overflowed()) {
        Serial.println("[ERROR] sendJsonToClient(): document capacity too "
                       "small, keys are missing");
    }

    const size_t payloadLength = measureJson(doc);

    String payload;
    payload.reserve(payloadLength + 1);
    serializeJson(doc, payload);

    if (payload.length() != payloadLength) {
        Serial.printf("[ERROR] sendJsonToClient(): out of memory for %u "
                      "bytes, payload dropped\n",
                      payloadLength + 1);
        return;
    }

    Serial.print("Sending Payload:");
    Serial.println(payload);

    webSocket.sendTXT(client_nr, payload);
}

//------------------------------------------------------------------------------

void sendI2CScanResult(uint8_t client_nr, uint8_t sdaPin, uint8_t sclPin) {
    DynamicJsonDocument config(384);
    config["command"] = "i2c-scan";
    config["sdaPin"] = sdaPin;
    config["sclPin"] = sclPin;

    JsonArray addresses = config.createNestedArray("addresses");
    i2cBus::ScanResult result =
        i2cBus::scan(sdaPin, sclPin, G.i2cSdaPin, G.i2cSclPin);

    if (result.status == i2cBus::ScanStatus::Invalid) {
        config["status"] = "invalid";
        sendJsonToClient(client_nr, config);
        return;
    }

    if (result.status == i2cBus::ScanStatus::Disabled) {
        config["status"] = "disabled";
        sendJsonToClient(client_nr, config);
        return;
    }

    for (uint8_t i = 0; i < result.addressCount; i++) {
        char addressText[5] = {0};
        snprintf(addressText, sizeof(addressText), "0x%02X",
                 result.addresses[i]);
        addresses.add(addressText);
    }

    config["status"] = "ok";
    sendJsonToClient(client_nr, config);
}

//------------------------------------------------------------------------------
// Front Effect Functions
//------------------------------------------------------------------------------

void ClockWork::rainbow() {
    static uint16_t hue = 0;

    for (uint8_t row = 0; row < usedClockType->rowsWordMatrix(); row++) {
        for (uint8_t col = 0; col < usedClockType->colsWordMatrix(); col++) {
            led.setPixel(row, col,
                         HsbColor(hue / 360.f, 1.f, G.effectBri / 100.f));
        }
    }
    led.show();
    hue++;
    led.checkIfHueIsOutOfBound(hue);
}

//-----------------------------------------------------------------------------

void ClockWork::rainbowCycle() {
    static uint16_t hue = 0;
    uint16_t numPixelsWordMatrix =
        usedClockType->rowsWordMatrix() * usedClockType->colsWordMatrix();
    uint16_t displayedHue;

    displayedHue = hue;
    for (uint8_t row = 0; row < usedClockType->rowsWordMatrix(); row++) {
        for (uint8_t col = 0; col < usedClockType->colsWordMatrix(); col++) {
            led.setPixel(
                row, col,
                HsbColor(displayedHue / 360.f, 1.f, G.effectBri / 100.f));
            displayedHue = displayedHue + 360.f / numPixelsWordMatrix;
            led.checkIfHueIsOutOfBound(displayedHue);
        }
    }
    led.show();
    hue++;
    led.checkIfHueIsOutOfBound(hue);
}

//------------------------------------------------------------------------------

void ClockWork::rainbowSpiralCycle() {
    static uint16_t hue = 0;

    uint8_t rows = usedClockType->rowsWordMatrix();
    uint8_t cols = usedClockType->colsWordMatrix();

    // 1. Calculate the center of the matrix.
    float centerRow = (rows - 1) / 2.0f;
    float centerCol = (cols - 1) / 2.0f;

    // 2. Calculate the maximum distance from the center to any corner.
    // This scales the color gradient cleanly to the matrix size.
    float maxDist = sqrt(centerRow * centerRow + centerCol * centerCol);

    for (uint8_t row = 0; row < rows; row++) {
        for (uint8_t col = 0; col < cols; col++) {

            // 3. Calculate the current pixel's distance from the center.
            float dRow = row - centerRow;
            float dCol = col - centerCol;
            float distance = sqrt(dRow * dRow + dCol * dCol);

            // 4. Calculate the color based on distance and base hue.
            // distance / maxDist returns a value between 0.0 and 1.0.
            // Multiplying it by 360 stretches the rainbow across the radius.
            float hueOffset = (distance / maxDist) * 360.0f;

            // Add the base hue, which drives the movement, and the offset.
            uint16_t pixelHue = hue + (uint16_t)hueOffset;

            // Keep generated hue values inside the 0-359 range.
            while (pixelHue >= 360) {
                pixelHue -= 360;
            }

            led.setPixel(row, col,
                         HsbColor(pixelHue / 360.f, 1.f, G.effectBri / 100.f));
        }
    }

    led.show();

    // 5. Advance the animation.
    // hue++ moves the colors inward.
    // hue-- (or hue + 359) would move the colors outward.
    hue++;
    if (hue >= 360) {
        hue = 0;
    }
}

//------------------------------------------------------------------------------

bool ClockWork::getStaticScrollingTextInfo(const char *buf,
                                           StaticScrollingText &info) {
    uint16_t textStart = 0;
    uint16_t textEnd = strlen(buf);

    auto isTrimmedWhitespace = [](char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    };

    while (textStart < textEnd && isTrimmedWhitespace(buf[textStart])) {
        textStart++;
    }

    while (textEnd > textStart && isTrimmedWhitespace(buf[textEnd - 1])) {
        textEnd--;
    }

    uint16_t trimmedLen = textEnd - textStart;
    if (trimmedLen == 0 || trimmedLen > 2) {
        return false;
    }

    info.start = static_cast<uint8_t>(textStart);
    info.len = static_cast<uint8_t>(trimmedLen);
    info.onlyDigits = true;

    for (uint16_t i = textStart; i < textEnd; i++) {
        if (buf[i] < '0' || buf[i] > '9') {
            info.onlyDigits = false;
            break;
        }
    }

    if (!info.onlyDigits) {
        info.width = pgm_read_byte(&(fontWidth[normalSizeASCII]));
        info.height = pgm_read_byte(&(fontHeight[normalSizeASCII]));
        info.totalWidth = info.len * info.width + info.len - 1;

        if (usedClockType->colsWordMatrix() < info.totalWidth ||
            usedClockType->rowsWordMatrix() < info.height) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void ClockWork::displayStaticScrollingText(const char *buf,
                                           const StaticScrollingText &info) {
    if (info.onlyDigits) {
        led.showNumbers(info.len == 1 ? ' ' : buf[info.start],
                        buf[info.start + info.len - 1]);
        return;
    }

    led.clearClock();

    uint8_t offsetRow = (usedClockType->rowsWordMatrix() - info.height) / 2;
    uint8_t offsetCol = (usedClockType->colsWordMatrix() - info.totalWidth) / 2;

    for (uint8_t charIndex = 0; charIndex < info.len; charIndex++) {
        uint8_t charOffsetCol = offsetCol + charIndex * (info.width + 1);
        unsigned char fontIndex =
            static_cast<unsigned char>(buf[info.start + charIndex]);

        for (uint8_t col = 0; col < info.width; col++) {
            for (uint8_t row = 0; row < info.height; row++) {
                led.setPixelForChar(col, row, charOffsetCol, offsetRow,
                                    fontIndex, normalSizeASCII);
            }
        }
    }

    led.mirrorFrontMatrixVertical();
    led.setbyFrontMatrix(Foreground);
    led.show();
}

//------------------------------------------------------------------------------

bool ClockWork::scrollingText(const char *buf, bool restart) {
    static uint8_t i = 0, ii = 0;
    StaticScrollingText staticText;

    if (restart) {
        i = 0;
        ii = 0;
    }

    if (getStaticScrollingTextInfo(buf, staticText)) {
        i = 0;
        ii = 0;
        displayStaticScrollingText(buf, staticText);
        return false;
    }

    uint8_t offsetRow = (usedClockType->rowsWordMatrix() -
                         pgm_read_byte(&(fontHeight[normalSizeASCII]))) /
                        2;

    uint16_t len = strlen(buf);
    uint8_t width = pgm_read_byte(&(fontWidth[normalSizeASCII]));
    uint8_t charsToFlush = (usedClockType->colsWordMatrix() / (width + 1)) + 1;

    if (ii >= len + charsToFlush) {
        ii = 0;
        i = 0;
    }

    uint8_t fontIndex = (ii < len) ? buf[ii] : ' ';

    led.setbyFrontMatrix(Foreground); // Needed for Mirrored Display
    led.shiftColumnToRight();
    led.clearFrontExeptofFontspace(offsetRow);

    if (i < width) {
        for (uint8_t row = 0;
             row < pgm_read_byte(&(fontHeight[normalSizeASCII])); row++) {
            usedClockType->setFrontMatrixPixel(
                row + offsetRow, 0,
                pgm_read_byte(&(font_7x5[fontIndex][i])) & (1u << row));
        }
    } else {
        for (uint8_t row = 0;
             row < pgm_read_byte(&(fontHeight[normalSizeASCII])); row++) {
            usedClockType->setFrontMatrixPixel(row + offsetRow, 0, false);
        }
    }

    led.setbyFrontMatrix(Foreground);
    led.show();

    i++;
    if (i >= width + 1) // +1 for spacing
    {
        i = 0;
        ii++;
        if (ii >= len + charsToFlush) {
            ii = 0;
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

void ClockWork::displaySymbols(BitmapSymbol symbolNum) {
    if (symbolNum >= BitmapSymbol::MAX_BITMAP_SYMBOLS) {
        symbolNum = BitmapSymbol::HEART;
    }

    HsbColor color = G.color[Foreground];
    color.B = G.effectBri / 100.f;
    led.setBitmapSymbol(symbolNum, color);
}

//------------------------------------------------------------------------------

void ClockWork::countdownToMidnight() {
    Serial.printf("Count down: %d\n", 60 - _second);
    switch (_second) {
    case 50:
        if (!usedClockType->hasOnlyQuarterLayout()) {
            usedClockType->show(FrontWord::min_10);
        } else {
            usedClockType->show(FrontWord::hour_10);
        }
        break;
    case 51:
        usedClockType->show(FrontWord::hour_9);
        break;
    case 52:
        usedClockType->show(FrontWord::hour_8);
        break;
    case 53:
        usedClockType->show(FrontWord::hour_7);
        break;
    case 54:
        usedClockType->show(FrontWord::hour_6);
        break;
    case 55:
        if (!usedClockType->hasOnlyQuarterLayout()) {
            usedClockType->show(FrontWord::min_5);
        } else {
            usedClockType->show(FrontWord::hour_5);
        }
        break;
    case 56:
        usedClockType->show(FrontWord::hour_4);
        break;
    case 57:
        usedClockType->show(FrontWord::hour_3);
        break;
    case 58:
        usedClockType->show(FrontWord::hour_2);
        break;
    case 59:
        if (usedClockType->usedLang() == LanguageAbbreviation::DE) {
            usedClockType->show(FrontWord::eins);
        } else {
            usedClockType->show(FrontWord::hour_1);
        }
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------
// Boot Functions
//------------------------------------------------------------------------------

void ClockWork::initBootShowIp(const char *buf) {
    uint8_t StringLength = strlen(buf);
    StringLength = StringLength * 6; // Times 6, because thats the length of a
                                     // char in the 7x5 font plus spacing
    for (uint16_t i = 0; i <= StringLength; i++) {
        scrollingText(buf);
        delay(200);
    }
}

//------------------------------------------------------------------------------

void ClockWork::initBootLedBlink() {
    for (uint8_t row = 0; row < usedClockType->rowsWordMatrix(); row++) {
        frontMatrix[row] ^= num32BitWithOnesAccordingToColumns();
    }
    led.setbyFrontMatrix(Foreground);
    led.show();
}

//------------------------------------------------------------------------------

void ClockWork::initBootLedSweep(uint32_t delay) { led.setSingle(delay); }

//------------------------------------------------------------------------------

void ClockWork::initBootLed() {
    led.clear();
    led.show();
}

//------------------------------------------------------------------------------
// Minute Functions
//------------------------------------------------------------------------------

uint8_t ClockWork::determineWhichMinuteVariant() {
    switch (G.minuteVariant) {
    case MinuteVariant::Off:
        return 0;
        break;
    case MinuteVariant::LED4x:
        return 0;
        break;
    case MinuteVariant::LED7x:
        return 1;
        break;
    case MinuteVariant::Corners:
        return 2;
        break;
    case MinuteVariant::InWords:
        return 0;
        break;
    default:
        Serial.println("[ERROR] G.minuteVariant undefined");
        return 0;
        break;
    }
}

//------------------------------------------------------------------------------

void ClockWork::showSpecialWordBeen(const uint8_t min) {
    if (usedClockType->hasSpecialWordBeen()) {
        if (min == 0) {
            usedClockType->show(FrontWord::nur);
        } else {
            usedClockType->show(FrontWord::gewesen);
        }
    }
}

//------------------------------------------------------------------------------

bool ClockWork::checkIfClockHasMinuteInWordsAndItIsSet() {
    return usedClockType->hasMinuteInWords() &&
           G.minuteVariant == MinuteVariant::InWords;
}

//------------------------------------------------------------------------------

void ClockWork::showMinuteInWords(const uint8_t min) {
    if (checkIfClockHasMinuteInWordsAndItIsSet()) {
        if (min > 0) {
            usedClockType->show(FrontWord::plus);
            usedClockType->show(FrontWord::minute);
            if (min % 5 > 1) {
                usedClockType->show(FrontWord::minuten);
            }
        }
        switch (min) {
        case 0:
            break;
        case 1:
            usedClockType->show(FrontWord::m_num1);
            break;
        case 2:
            usedClockType->show(FrontWord::m_num2);
            break;
        case 3:
            usedClockType->show(FrontWord::m_num3);
            break;
        case 4:
            usedClockType->show(FrontWord::m_num4);
            break;

        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------

void ClockWork::showMinute(uint8_t min) {
    /* To prevent errors if min is not in Range of [0-4] */
    min %= 5;

    /* saving corosponding minutePixelArray */
    usedClockType->getMinuteArray(minutePixelArray,
                                  determineWhichMinuteVariant());
    /* Reseting minute byte */
    minuteArray = 0;
    if (usedClockType->hasMinuteInWords() && min > 0) {
        minuteArray |= 1UL << (min - 1);
    } else {
        for (uint8_t i = 0; i < min; i++) {
            /* Shifting a 1 Bit for every pixel starting at LSB */
            minuteArray |= 1UL << i;
        }
    }
    /* Show "Been" according to ClockTypedef */
    showSpecialWordBeen(min);

    /* Show Minute "In Words" according to ClockTypedef */
    showMinuteInWords(min);
}

//------------------------------------------------------------------------------

void ClockWork::checkForValidLanguageVariant() {
    if (G.clockTypeDef == Eng10x11) {
        G.languageVariant[ItIs15] = false;
        G.languageVariant[ItIs20] = true;
        G.languageVariant[ItIs40] = true;
    } else { // default values
        G.languageVariant[ItIs15] = false;
        G.languageVariant[ItIs20] = false;
        G.languageVariant[ItIs40] = false;
    }
}

//------------------------------------------------------------------------------

void ClockWork::resetMinVariantIfNotAvailable() {
    if (usedClockType->supportsMinuteVariant(G.minuteVariant)) {
        return;
    }

    if (G.minuteVariant == MinuteVariant::Corners &&
        usedClockType->supportsMinuteVariant(MinuteVariant::LED4x)) {
        G.minuteVariant = MinuteVariant::LED4x;
    } else {
        G.minuteVariant = MinuteVariant::Off;
    }
}

//------------------------------------------------------------------------------

FrontWord ClockWork::getFrontWordForNum(uint8_t min) {
    if (min == 0 || min > 59) {
        Serial.println("[Error] getFrontWordForNum() out of Bounds");
        return FrontWord::error;
    } else {
        return static_cast<FrontWord>(min);
    }
}

//------------------------------------------------------------------------------

bool ClockWork::checkTwentyUsage() {
    return G.languageVariant[ItIs20] || G.languageVariant[ItIs40] ||
           usedClockType->usedLang() == LanguageAbbreviation::ES ||
           usedClockType->usedLang() == LanguageAbbreviation::TR;
}

//------------------------------------------------------------------------------

bool ClockWork::hasTwentyAndCheckForUsage() {
    // ToDo: Is this true for every supoorted language variant?
    return usedClockType->hasTwenty() && checkTwentyUsage();
}

//------------------------------------------------------------------------------

bool ClockWork::hasDreiviertelAndCheckForUsage() {
    if (usedClockType->hasDreiviertel()) {
        if (usedClockType->usedLang() != LanguageAbbreviation::DE) {
            return true;
        } else if (G.languageVariant[ItIs45]) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

void ClockWork::setMinute(uint8_t min, uint8_t &offsetHour, bool &fullHour) {
    if (usedClockType->has60MinuteLayout()) {
        usedClockType->show(FrontWord::uhr);

        if (min == 0) {
            fullHour = true;
        } else if (min > 0) {
            usedClockType->show(getFrontWordForNum(min));
            usedClockType->show(FrontWord::und);
        }

        if (min == 1) {
            usedClockType->show(FrontWord::minute);
        } else if (min > 1) {
            usedClockType->show(FrontWord::minuten);
        }
    } else if (usedClockType->hasOnlyQuarterLayout()) {

        if (8 <= min && min <= 22) {
            if (G.languageVariant[ItIs15]) {
                usedClockType->show(FrontWord::viertel);
                offsetHour = 1;
            } else {
                // A Quarter past
                if (G.languageVariant[EN_ShowAQuarter]) {
                    usedClockType->show(FrontWord::a_quarter);
                }
                usedClockType->show(FrontWord::viertel);
                usedClockType->show(FrontWord::v_nach);
            }
        } else if (23 <= min && min <= 37) { // half
            if (G.clockTypeDef == Eng10x11 || G.clockTypeDef == It10x11 ||
                G.clockTypeDef == Es10x11 || G.clockTypeDef == Ro10x11) {
                usedClockType->show(FrontWord::halb);
                usedClockType->show(FrontWord::nach);
            } else {
                if (G.clockTypeDef == Fr10x11 || G.clockTypeDef == Ru10x11) {
                    usedClockType->show(FrontWord::halb);
                } else {
                    usedClockType->show(FrontWord::halb);
                    offsetHour = 1;
                }
            }
        } else if (38 <= min && min <= 52) { // quarter to
            if (hasDreiviertelAndCheckForUsage()) {
                usedClockType->show(FrontWord::dreiviertel);
            } else {
                // A Quarter to
                if (G.languageVariant[EN_ShowAQuarter]) {
                    usedClockType->show(FrontWord::a_quarter);
                }
                usedClockType->show(FrontWord::viertel);
                usedClockType->show(FrontWord::v_vor);
            }
            offsetHour = 1;
        } else if (53 <= min && min <= 59) { // almost full hour
            offsetHour = 1;
        }
    } else {
        showMinute(min);
        min /= 5;
        min *= 5;

        switch (min) {
        case 0: // full hour
            usedClockType->show(FrontWord::uhr);
            fullHour = true;
            break;
        case 1:
            usedClockType->show(FrontWord::min_1);
            usedClockType->show(FrontWord::minute);
            usedClockType->show(FrontWord::nach);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            usedClockType->show(getFrontWordForNum(min));
            if (G.clockTypeDef != Fr10x11) {
                usedClockType->show(FrontWord::nach);
            }
            break;
        case 11:
        case 12:
        case 13:
        case 14:
            usedClockType->show(getFrontWordForNum(min));
            usedClockType->show(FrontWord::nach);
            break;
        case 15: // quarter past
            if (G.languageVariant[ItIs15]) {
                usedClockType->show(FrontWord::viertel);
                offsetHour = 1;
            } else {
                // A Quarter past
                if (G.languageVariant[EN_ShowAQuarter]) {
                    usedClockType->show(FrontWord::a_quarter);
                }
                usedClockType->show(FrontWord::viertel);
                usedClockType->show(FrontWord::v_nach);
            }
            break;
        case 16:
        case 17:
        case 18:
        case 19:
            usedClockType->show(getFrontWordForNum(min));
            usedClockType->show(FrontWord::nach);
            break;
        case 20:
            if (hasTwentyAndCheckForUsage()) {
                usedClockType->show(FrontWord::min_20);
                if (G.clockTypeDef != Fr10x11) {
                    usedClockType->show(FrontWord::nach);
                }
            } else {
                usedClockType->show(FrontWord::min_10);
                usedClockType->show(FrontWord::vor);
                usedClockType->show(FrontWord::halb);
                offsetHour = 1;
            }
            break;
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            if (usedClockType->hasTwentyfive()) {
                usedClockType->show(FrontWord::min_25);
                if (G.clockTypeDef != Fr10x11) {
                    usedClockType->show(FrontWord::nach);
                }
            } else {
                usedClockType->show(FrontWord::min_5);
                usedClockType->show(FrontWord::vor);
                usedClockType->show(FrontWord::halb);
                offsetHour = 1;
            }
            break;
        case 26:
        case 27:
        case 28:
            usedClockType->show(getFrontWordForNum(30 - min));
            usedClockType->show(FrontWord::vor);
            usedClockType->show(FrontWord::halb);
            offsetHour = 1;
            break;
        case 29:
            usedClockType->show(FrontWord::min_1);
            usedClockType->show(FrontWord::minute);
            usedClockType->show(FrontWord::vor);
            usedClockType->show(FrontWord::halb);
            offsetHour = 1;
            break;
        case 30: // half
            if (G.clockTypeDef == Tr10x11) {
                usedClockType->show(FrontWord::halb);
                fullHour = true;
            } else if (G.clockTypeDef == Eng10x11 ||
                       G.clockTypeDef == Eng08x08 ||
                       G.clockTypeDef == It10x11 || G.clockTypeDef == Ro10x11 ||
                       G.clockTypeDef == Es10x11 ||
                       G.clockTypeDef == Es08x08Cuarto) {
                usedClockType->show(FrontWord::halb);
                usedClockType->show(FrontWord::nach);
            } else {
                if (G.clockTypeDef == Fr10x11 || G.clockTypeDef == Ru10x11) {
                    usedClockType->show(FrontWord::halb);
                } else {
                    usedClockType->show(FrontWord::halb);
                    offsetHour = 1;
                }
            }
            break;
        case 31:
            usedClockType->show(FrontWord::min_1);
            usedClockType->show(FrontWord::minute);
            usedClockType->show(FrontWord::nach);
            usedClockType->show(FrontWord::halb);
            offsetHour = 1;
            break;
        case 32:
        case 33:
        case 34:
        case 35:
            if (usedClockType->hasThirtyfive()) {
                usedClockType->show(FrontWord::min_35);
                usedClockType->show(FrontWord::nach);
            } else if (usedClockType->hasTwentyfive()) {
                usedClockType->show(FrontWord::min_25);
                usedClockType->show(FrontWord::vor);
                offsetHour = 1;
            } else {
                usedClockType->show(FrontWord::min_5);
                usedClockType->show(FrontWord::nach);
                usedClockType->show(FrontWord::halb);
                offsetHour = 1;
            }
            break;
        case 36:
        case 37:
        case 38:
        case 39:
            usedClockType->show(getFrontWordForNum(min - 30));
            usedClockType->show(FrontWord::nach);
            usedClockType->show(FrontWord::halb);
            offsetHour = 1;
            break;
        case 40:
            if (usedClockType->hasForty()) {
                usedClockType->show(FrontWord::min_40);
            } else if (hasTwentyAndCheckForUsage()) {
                usedClockType->show(FrontWord::min_20);
                usedClockType->show(FrontWord::vor);
            } else {
                usedClockType->show(FrontWord::min_10);
                usedClockType->show(FrontWord::nach);
                usedClockType->show(FrontWord::halb);
            }
            if (G.clockTypeDef != Tr10x11) {
                offsetHour = 1;
            }
            break;
        case 41:
        case 42:
        case 43:
        case 44:
            usedClockType->show(getFrontWordForNum(60 - min));
            usedClockType->show(FrontWord::vor);
            offsetHour = 1;
            break;
        case 45: // quarter to
            if (G.clockTypeDef == Tr10x11) {
                usedClockType->show(FrontWord::min_45);
                usedClockType->show(FrontWord::nach);
            } else if (hasDreiviertelAndCheckForUsage()) {
                usedClockType->show(FrontWord::dreiviertel);
            } else {
                // A Quarter to
                if (G.languageVariant[EN_ShowAQuarter]) {
                    usedClockType->show(FrontWord::a_quarter);
                }
                usedClockType->show(FrontWord::viertel);
                usedClockType->show(FrontWord::v_vor);
            }
            if (G.clockTypeDef != Tr10x11) {
                offsetHour = 1;
            }
            break;
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
            if (usedClockType->hasFifty()) {
                usedClockType->show(FrontWord::min_50);
            } else {
                usedClockType->show(getFrontWordForNum(60 - min));
                usedClockType->show(FrontWord::vor);
            }
            if (G.clockTypeDef != Tr10x11) {
                offsetHour = 1;
            }
            break;
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
            if (usedClockType->hasFiftyFive()) {
                usedClockType->show(FrontWord::min_55);
            } else {
                usedClockType->show(getFrontWordForNum(60 - min));
                usedClockType->show(FrontWord::vor);
            }
            if (G.clockTypeDef != Tr10x11) {
                offsetHour = 1;
            }
            break;
        case 56:
        case 57:
        case 58:
            usedClockType->show(getFrontWordForNum(60 - min));
            usedClockType->show(FrontWord::minuten);
            usedClockType->show(FrontWord::vor);
            offsetHour = 1;
            break;
        case 59:
            usedClockType->show(FrontWord::min_1);
            usedClockType->show(FrontWord::minute);
            usedClockType->show(FrontWord::vor);
            offsetHour = 1;
            break;
        default:
            break;
        }

        if (G.clockTypeDef == Ru10x11) {
            offsetHour = 0;
        }
    }
}

//------------------------------------------------------------------------------
// Hour Functions
//------------------------------------------------------------------------------

void ClockWork::setHour(uint8_t hour, const bool fullHour) {
    bool midnight = isMidnight(hour);

    if (!usedClockType->has24HourLayout()) {
        hour %= 12;
    }

    bool turkishBaseHour =
        usedClockType->usedLang() == LanguageAbbreviation::TR && fullHour;
    if (turkishBaseHour && hour != 0) {
        hour += 12;
    }

    switch (hour) {
    case 0:
        if (turkishBaseHour) {
            usedClockType->show(FrontWord::hour_0);
        } else if (midnight) {
            usedClockType->show(FrontWord::hour_0);
        } else {
            usedClockType->show(FrontWord::hour_12);
        }
        break;
    case 1:
        if (fullHour || usedClockType->usedLang() != LanguageAbbreviation::DE) {
            usedClockType->show(FrontWord::hour_1);
        } else {
            usedClockType->show(FrontWord::eins);
        }
        break;
    case 2:
        usedClockType->show(FrontWord::hour_2);
        break;
    case 3:
        usedClockType->show(FrontWord::hour_3);
        break;
    case 4:
        usedClockType->show(FrontWord::hour_4);
        break;
    case 5:
        usedClockType->show(FrontWord::hour_5);
        break;
    case 6:
        usedClockType->show(FrontWord::hour_6);
        break;
    case 7:
        usedClockType->show(FrontWord::hour_7);
        break;
    case 8:
        usedClockType->show(FrontWord::hour_8);
        break;
    case 9:
        usedClockType->show(FrontWord::hour_9);
        break;
    case 10:
        usedClockType->show(FrontWord::hour_10);
        break;
    case 11:
        usedClockType->show(FrontWord::hour_11);
        break;
    case 12:
        usedClockType->show(FrontWord::hour_12);
        break;
    case 13:
        usedClockType->show(FrontWord::hour_13);
        break;
    case 14:
        usedClockType->show(FrontWord::hour_14);
        break;
    case 15:
        usedClockType->show(FrontWord::hour_15);
        break;
    case 16:
        usedClockType->show(FrontWord::hour_16);
        break;
    case 17:
        usedClockType->show(FrontWord::hour_17);
        break;
    case 18:
        usedClockType->show(FrontWord::hour_18);
        break;
    case 19:
        usedClockType->show(FrontWord::hour_19);
        break;
    case 20:
        usedClockType->show(FrontWord::hour_20);
        break;
    case 21:
        usedClockType->show(FrontWord::hour_21);
        break;
    case 22:
        usedClockType->show(FrontWord::hour_22);
        break;
    case 23:
        usedClockType->show(FrontWord::hour_23);
        break;

    default:
        break;
    }
}

//------------------------------------------------------------------------------
// Loop Helper Functions
//------------------------------------------------------------------------------

bool ClockWork::isMidnight(const uint8_t hour) {
    return usedClockType->hasMitternacht() && hour == 0;
}

//------------------------------------------------------------------------------

WordclockChanges ClockWork::changesInClockface() {
    for (uint16_t i = 0; i < MAX_ROW_SIZE; i++) {
        if (frontMatrix[i] != lastFrontMatrix[i]) {
            return WordclockChanges::Words;
        }
    }
    if (parametersChanged) {
        parametersChanged = false;
        return WordclockChanges::Parameters;
    } else if (layoutChanged) {
        layoutChanged = false;
        return WordclockChanges::Layout;
    } else if (lastMinuteArray != minuteArray) {
        return WordclockChanges::Minute;
    }
    return WordclockChanges::Null;
}

//------------------------------------------------------------------------------

bool ClockWork::DetermineIfItIsIsShown(const uint8_t min) {
    switch (G.itIsVariant) {
    case ItIsVariant::Permanent:
        return true;
    case ItIsVariant::Hourly:
        return !min;
    case ItIsVariant::HalfHourly:
        return !(min % 30);
    case ItIsVariant::Quarterly:
        return !(min % 15);
    case ItIsVariant::Off:
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
void ClockWork::setItIs(uint8_t min, const uint8_t offsetHour) {
    min /= 5;
    min *= 5;

    if (DetermineIfItIsIsShown(min)) {
        DetermineWhichItIsToShow(_hour + offsetHour, min);
    }
}

//------------------------------------------------------------------------------

void ClockWork::setClock() {
    if (setClockWc24h()) {
        return;
    }

    uint8_t offsetHour = 0;
    bool fullHour = 0;

    setMinute(_minute, offsetHour, fullHour);
    setHour(_hour + offsetHour, fullHour);
    setDaytimeWords(_hour + offsetHour);
    setItIs(_minute, offsetHour);
}

//------------------------------------------------------------------------------

void ClockWork::setDaytimeWords(uint8_t hour) {
    if (!usedClockType->hasDaytimeWords()) {
        return;
    }

    hour %= 24;

    if (hour == 0) {
        usedClockType->show(FrontWord::mitternachts);
    } else if (hour < 5) {
        usedClockType->show(FrontWord::day_night);
    } else if (hour < 10) {
        usedClockType->show(FrontWord::day_morning);
        usedClockType->show(FrontWord::day_early);
    } else if (hour < 12) {
        usedClockType->show(FrontWord::day_morning);
    } else if (hour < 14) {
        usedClockType->show(FrontWord::day_noon);
    } else if (hour >= 18) {
        usedClockType->show(FrontWord::day_evening);
    }
}

//------------------------------------------------------------------------------

void ClockWork::DetermineWhichItIsToShow(uint8_t hour, uint8_t min) {
    hour %= 12;
    if (G.clockTypeDef == Ru10x11) {
        switch (hour) {
        case 1:
            usedClockType->show(FrontWord::es_ist__singular__);
            break;
        case 2:
        case 3:
        case 4:
            usedClockType->show(FrontWord::es_ist___plural___);
            break;
        default:
            usedClockType->show(FrontWord::es_ist);
            break;
        }
    } else if (G.clockTypeDef == Es10x11 && hour == 1 && min < 35) {
        usedClockType->show(FrontWord::es_ist___plural___);
    } else if (G.clockTypeDef == Es10x11 && hour == 0 && min > 34) {
        usedClockType->show(FrontWord::es_ist___plural___);
    } else {
        usedClockType->show(FrontWord::es_ist);
    }
}

//------------------------------------------------------------------------------
// WordClock24h Functions
//------------------------------------------------------------------------------

namespace {
// Only the DS3231 has a temperature sensor; the overload resolution picks the
// template for every other RTC type.
bool readRtcTemperature(RTC_DS3231 &rtc, float &celsius) {
    celsius = rtc.getTemperature();
    return true;
}

template <typename Rtc> bool readRtcTemperature(Rtc &, float &) {
    return false;
}
} // namespace

//------------------------------------------------------------------------------

// The DS3231 of an attached RTC module measures the room temperature, else
// the outdoor temperature of OpenWeatherMap is used.
bool ClockWork::readTemperature(float &celsius) {
    if (externalRTC && readRtcTemperature(RTC, celsius)) {
        return true;
    }
    return currentWeather.getTemperature(celsius);
}

//------------------------------------------------------------------------------

namespace {
// Temperature in half degrees, as used by the WordClock24h tables.
bool toTemperatureIndex(float celsius, uint8_t &temperatureIndex) {
    if (celsius < 0.f || celsius > 127.f) {
        return false;
    }
    temperatureIndex = static_cast<uint8_t>(lroundf(celsius * 2.f));
    return true;
}
} // namespace

//------------------------------------------------------------------------------

bool ClockWork::readTemperatureIndex(uint8_t &temperatureIndex) {
    float celsius;
    return readTemperature(celsius) &&
           toTemperatureIndex(celsius, temperatureIndex);
}

//------------------------------------------------------------------------------

bool ClockWork::isCurrentWeatherNeeded() {
    if (usedClockType->hasWc24hTables() &&
        G.wc24hDisplayMode == wc24h::TEMPERATURE_MODE) {
        return true;
    }
    for (const wc24h::Overlay &overlay : G.overlays) {
        if ((overlay.flags & wc24h::OVERLAY_FLAG_ACTIVE) &&
            (overlay.type == wc24h::OVERLAY_TEMPERATURE ||
             overlay.type == wc24h::OVERLAY_TEMPERATURE_DIGITS ||
             overlay.type == wc24h::OVERLAY_WEATHER_ICON)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

void ClockWork::showWc24hWords(const bool *words, bool showItIs) {
    const uint8_t cols = usedClockType->colsWordMatrix();

    for (uint8_t idx = 1; idx < wc24h::WP_COUNT; idx++) {
        if (!words[idx]) {
            continue;
        }
        const wc24h::WORD_ILLUMINATION &word = wc24h::illumination[idx];
        if ((word.len & wc24h::ILLUMINATION_FLAG_IT_IS) && !showItIs) {
            continue;
        }
        // The tables count columns from the left, the front matrix from the
        // right.
        const uint8_t len = word.len & wc24h::ILLUMINATION_LEN_MASK;
        usedClockType->setFrontMatrixWord(word.row, cols - word.col - len,
                                          cols - 1 - word.col);
    }
}

//------------------------------------------------------------------------------

bool ClockWork::setClockWc24h() {
    if (!usedClockType->hasWc24hTables() ||
        G.wc24hDisplayMode >= wc24h::DISPLAY_MODES_COUNT) {
        return false;
    }

    wc24h::WordSet words;

    if (G.wc24hDisplayMode == wc24h::TEMPERATURE_MODE) {
        uint8_t temperatureIndex;
        // Without a usable temperature the clock keeps showing the time.
        if (!readTemperatureIndex(temperatureIndex) ||
            !wc24h::fillTemperatureWords(temperatureIndex, words)) {
            return false;
        }
    } else {
        wc24h::fillWords(G.wc24hDisplayMode, _hour, _minute, words);
    }

    showWc24hWords(words, DetermineIfItIsIsShown(_minute));
    return true;
}

//------------------------------------------------------------------------------

void ClockWork::loopNightTimers(const struct tm &tm) {
    const bool powerIsOn = led.getState();

    for (const NightTimer &timer : G.nightTimers) {
        if (wc24h::nightTimerFires(timer.flags, timer.hour, timer.minute,
                                   powerIsOn, tm.tm_wday, tm.tm_hour,
                                   tm.tm_min)) {
            led.setState(!powerIsOn);
            requestHardwareButtonDisplayRefresh();
            return;
        }
    }
}

//------------------------------------------------------------------------------
// Color animation "daylight" of the WordClock24h firmware: the foreground
// color follows the time of day, blue at night, red in the morning and
// evening, yellow at noon.

void ClockWork::loopColorAnimation() {
    // 0..63 per hour, as in display.c of the original firmware
    static const uint8_t red[24] = {0,  0,  0,  15, 31, 47, 63, 63,
                                    63, 63, 63, 63, 63, 63, 63, 63,
                                    63, 63, 63, 47, 31, 15, 0,  0};
    static const uint8_t green[24] = {0,  0,  0,  0,  0, 0, 0, 0, 0, 15, 31, 47,
                                      63, 47, 31, 15, 0, 0, 0, 0, 0, 0,  0,  0};
    static const uint8_t blue[24] = {63, 47, 31, 15, 0,  0,  0,  0,
                                     0,  0,  0,  0,  0,  0,  0,  15,
                                     31, 47, 63, 63, 63, 63, 63, 63};

    if (G.colorAnimation != COLOR_ANIMATION_DAYLIGHT) {
        return;
    }

    // Blend into the next hour's color in the course of the hour.
    const uint8_t next = (_hour + 1) % 24;
    const float progress = (_minute * 60 + _second) / 3600.f;
    const RgbColor from(red[_hour] * 4, green[_hour] * 4, blue[_hour] * 4);
    const RgbColor to(red[next] * 4, green[next] * 4, blue[next] * 4);
    const HsbColor blended(RgbColor::LinearBlend(from, to, progress));

    HsbColor &foreground = G.color[Foreground];
    if (fabsf(foreground.H - blended.H) > 0.002f ||
        fabsf(foreground.S - blended.S) > 0.002f) {
        foreground.H = blended.H;
        foreground.S = blended.S;
        parametersChanged = true;
    }
}

//------------------------------------------------------------------------------
// Overlays of the WordClock24h firmware

namespace {
struct OverlayState {
    const wc24h::Overlay *overlay = nullptr;
    const wc24h::Icon *icon = nullptr;
    uint32_t startMillis = 0;
    uint32_t lastFrameMillis = 0;
    uint32_t durationMs = 0;
    bool ticker = false;
    float celsius = 0.f;
    char text[48] = {};
} overlayState;

constexpr uint32_t OVERLAY_ICON_FADE_MS = 2000;
constexpr uint32_t OVERLAY_FRAME_MS = 30;
constexpr uint32_t OVERLAY_MAX_MS = 120000;

const wc24h::Icon *findWeatherIcon(const char *code) {
    for (const wc24h::Icon &icon : wc24h::WEATHER_ICONS) {
        if (strcmp(icon.name, code) == 0) {
            return &icon;
        }
    }
    return nullptr;
}
} // namespace

//------------------------------------------------------------------------------

void ClockWork::startOverlay(const struct tm &tm) {
    if (overlayActive || !led.getState() ||
        (G.prog != COMMAND_IDLE && G.prog != COMMAND_MODE_WORD_CLOCK)) {
        return;
    }

    const int8_t index =
        wc24h::selectOverlay(G.overlays, wc24h::MAX_OVERLAYS, tm.tm_year + 1900,
                             tm.tm_mon + 1, tm.tm_mday, tm.tm_min);
    if (index < 0 || !beginOverlay(G.overlays[index], tm)) {
        return;
    }

    overlayActive = true;
    overlayState.startMillis = millis();
    overlayState.lastFrameMillis = 0;
    led.clear();
    led.show();
}

//------------------------------------------------------------------------------
// Prepares the overlay, returns false when there is nothing to show.

bool ClockWork::beginOverlay(const wc24h::Overlay &overlay,
                             const struct tm &tm) {
    overlayState.overlay = &overlay;
    overlayState.icon = nullptr;
    overlayState.ticker = false;
    overlayState.text[0] = '\0';
    // At least 5 seconds as in the original, to fade in and out.
    overlayState.durationMs =
        (overlay.duration < 5 ? 5UL : overlay.duration) * 1000UL;

    switch (overlay.type) {
    case wc24h::OVERLAY_ICON:
        overlayState.icon = &wc24h::ICONS[overlay.icon];
        return true;

    case wc24h::OVERLAY_WEATHER_ICON:
        overlayState.icon = findWeatherIcon(currentWeather.getIconCode());
        return overlayState.icon != nullptr;

    case wc24h::OVERLAY_DATE:
        snprintf(overlayState.text, sizeof(overlayState.text), "%02d.%02d.%04d",
                 tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        overlayState.ticker = true;
        return true;

    case wc24h::OVERLAY_TICKER:
        strlcpy(overlayState.text, overlay.text, sizeof(overlayState.text));
        overlayState.ticker = overlayState.text[0] != '\0';
        return overlayState.ticker;

    case wc24h::OVERLAY_DAYS_UNTIL: {
        if (overlay.month < 1 || overlay.month > 12 || overlay.day < 1 ||
            overlay.day > 31) {
            return false;
        }
        const int32_t days =
            wc24h::daysUntil(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                             overlay.month, overlay.day);
        if (days == 0) {
            snprintf(overlayState.text, sizeof(overlayState.text), "HEUTE: %s",
                     overlay.text);
        } else if (days == 1) {
            snprintf(overlayState.text, sizeof(overlayState.text),
                     "MORGEN: %s", overlay.text);
        } else {
            snprintf(overlayState.text, sizeof(overlayState.text),
                     "NOCH %ld TAGE: %s", static_cast<long>(days),
                     overlay.text);
        }
        overlayState.ticker = true;
        return true;
    }

    case wc24h::OVERLAY_MOON: {
        static char moonColors[wc24h::MOON_SIZE * wc24h::MOON_SIZE + 1];
        static const wc24h::Icon moonIcon = {"Moon", wc24h::MOON_SIZE,
                                             wc24h::MOON_SIZE, moonColors,
                                             nullptr, nullptr};
        wc24h::moonIcon(wc24h::moonPhase(tm.tm_year + 1900, tm.tm_mon + 1,
                                         tm.tm_mday),
                        moonColors);
        overlayState.icon = &moonIcon;
        return true;
    }

    case wc24h::OVERLAY_TEMPERATURE:
    case wc24h::OVERLAY_TEMPERATURE_DIGITS: {
        if (!readTemperature(overlayState.celsius)) {
            return false;
        }
        uint8_t index;
        wc24h::WordSet words;
        if (overlay.type == wc24h::OVERLAY_TEMPERATURE &&
            !(toTemperatureIndex(overlayState.celsius, index) &&
              wc24h::fillTemperatureWords(index, words))) {
            // Outside of what the words can say: scroll it instead.
            snprintf(overlayState.text, sizeof(overlayState.text), "%d GRAD",
                     static_cast<int>(lroundf(overlayState.celsius)));
            overlayState.ticker = true;
        }
        return true;
    }

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Notification from Home Assistant / MQTT: an icon (see wc24h::findIcon)
// and/or a text. With both, the icon comes first and the text scrolls
// afterwards. Shown only on top of the word clock, so games, the timer and
// moods are not interrupted.

namespace {
char pendingNotificationText[PAYLOAD_LENGTH] = "";
uint8_t pendingNotificationRepeats = 0;

void startNotificationText(const char *text, uint8_t repeats) {
    wc24h::toClockText(text, messageText, sizeof(messageText));
    if (!messageText[0]) {
        return;
    }
    messageRepeats = repeats ? repeats : 3;
    G.prog = COMMAND_MODE_MESSAGE;
    G.progInit = true;
}
} // namespace

void ClockWork::showNotification(int16_t icon, const char *text,
                                 uint8_t repeats) {
    if (!led.getState() || overlayActive ||
        (G.prog != COMMAND_IDLE && G.prog != COMMAND_MODE_WORD_CLOCK)) {
        Serial.println("Notification skipped: display busy or off");
        return;
    }
    if (repeats > 10) {
        repeats = 10;
    }
    const bool hasText = text && text[0];
    if (icon < 0 || icon >= wc24h::ICON_COUNT) {
        if (hasText) {
            startNotificationText(text, repeats);
        }
        return;
    }

    static wc24h::Overlay notification;
    memset(&notification, 0, sizeof(notification));
    notification.flags = wc24h::OVERLAY_FLAG_ACTIVE;
    notification.duration = 6;
    notification.type = wc24h::OVERLAY_ICON;
    notification.icon = icon;
    const time_t now = time(nullptr);
    struct tm tm;
    localtime_r(&now, &tm);
    if (!beginOverlay(notification, tm)) {
        return;
    }
    strlcpy(pendingNotificationText, hasText ? text : "",
            sizeof(pendingNotificationText));
    pendingNotificationRepeats = repeats;
    overlayActive = true;
    overlayState.startMillis = millis();
    overlayState.lastFrameMillis = 0;
    led.clear();
    led.show();
}

//------------------------------------------------------------------------------

void ClockWork::stopOverlay() {
    overlayActive = false;
    overlayState.overlay = nullptr;
    led.clear();
    G.prog = COMMAND_MODE_WORD_CLOCK;
    parametersChanged = true;
    // A notification icon is followed by its text.
    if (pendingNotificationText[0]) {
        startNotificationText(pendingNotificationText,
                              pendingNotificationRepeats);
        pendingNotificationText[0] = '\0';
    }
}

//------------------------------------------------------------------------------

void ClockWork::showOverlayWords(const bool *words) {
    led.resetFrontMatrixBuffer();
    showWc24hWords(words, true);
    led.setbyFrontMatrix(Foreground);
    led.setbyFrontMatrix(Background, false);
    led.show();
}

//------------------------------------------------------------------------------

void ClockWork::showOverlayIcon(uint32_t elapsed) {
    const wc24h::Icon &icon = *overlayState.icon;
    const uint8_t rows = usedClockType->rowsWordMatrix();
    const uint8_t cols = usedClockType->colsWordMatrix();
    const uint8_t rowOffset = icon.rows < rows ? (rows - icon.rows) / 2 : 0;
    const uint8_t colOffset = icon.cols < cols ? (cols - icon.cols) / 2 : 0;
    const uint32_t step = elapsed / 62; // animation steps of 1/16 s

    float fade = 1.f;
    if (elapsed < OVERLAY_ICON_FADE_MS) {
        fade = static_cast<float>(elapsed) / OVERLAY_ICON_FADE_MS;
    } else if (overlayState.durationMs - elapsed < OVERLAY_ICON_FADE_MS) {
        fade = static_cast<float>(overlayState.durationMs - elapsed) /
               OVERLAY_ICON_FADE_MS;
    }
    const float brightness =
        led.getColorbyPositionWithAppliedBrightness(Foreground).B * fade;

    for (uint8_t row = 0; row < icon.rows && row + rowOffset < rows; row++) {
        for (uint8_t col = 0; col < icon.cols && col + colOffset < cols;
             col++) {
            const uint16_t idx = row * icon.cols + col;
            uint32_t onStep = 0;
            uint32_t offStep = UINT32_MAX;
            if (icon.animationOn && icon.animationOn[idx] >= 'A') {
                onStep = icon.animationOn[idx] - 'A';
            }
            if (icon.animationOff && icon.animationOff[idx] >= 'A') {
                offStep = icon.animationOff[idx] - 'A';
            }

            uint8_t displayRow = row + rowOffset;
            uint8_t displayCol = col + colOffset;
            if (G.layoutVariant[MirrorVertical]) {
                displayCol = cols - 1 - displayCol;
            }
            if (G.layoutVariant[MirrorHorizontal]) {
                displayRow = rows - 1 - displayRow;
            }

            const uint8_t colorIdx = icon.colors[idx] - '0';
            if (colorIdx == 0 || colorIdx > 9 || step < onStep ||
                step >= offStep) {
                led.setPixel(displayRow, displayCol, HsbColor(0.f, 0.f, 0.f));
                continue;
            }
            const wc24h::IconColor &rgb = wc24h::ICON_PALETTE[colorIdx];
            HsbColor color(RgbColor(rgb.red, rgb.green, rgb.blue));
            color.B *= brightness;
            led.setPixel(displayRow, displayCol, color);
        }
    }
    led.show();
}

//------------------------------------------------------------------------------

void ClockWork::loopOverlay() {
    if (!led.getState() ||
        (G.prog != COMMAND_IDLE && G.prog != COMMAND_MODE_WORD_CLOCK)) {
        // switched off or another mode was chosen meanwhile
        overlayActive = false;
        overlayState.overlay = nullptr;
        return;
    }

    const uint32_t now = millis();
    const uint32_t elapsed = now - overlayState.startMillis;

    if (overlayState.ticker) {
        const uint32_t frameMs = (11u - G.effectSpeed) * 30u;
        if (overlayState.lastFrameMillis != 0 &&
            now - overlayState.lastFrameMillis < frameMs) {
            return;
        }
        const bool restart = overlayState.lastFrameMillis == 0;
        overlayState.lastFrameMillis = now;
        // A text that fits on the display does not scroll and stays for the
        // overlay's duration instead.
        StaticScrollingText staticText;
        const bool isStatic =
            getStaticScrollingTextInfo(overlayState.text, staticText);
        if (scrollingText(overlayState.text, restart) ||
            elapsed >= OVERLAY_MAX_MS ||
            (isStatic && elapsed >= overlayState.durationMs)) {
            stopOverlay();
        }
        return;
    }

    if (elapsed >= overlayState.durationMs) {
        stopOverlay();
        return;
    }
    if (now - overlayState.lastFrameMillis < OVERLAY_FRAME_MS) {
        return;
    }
    overlayState.lastFrameMillis = now;

    if (overlayState.icon) {
        showOverlayIcon(elapsed);
        return;
    }

    uint8_t index;
    wc24h::WordSet words;
    if (overlayState.overlay->type == wc24h::OVERLAY_TEMPERATURE &&
        toTemperatureIndex(overlayState.celsius, index) &&
        wc24h::fillTemperatureWords(index, words)) {
        showOverlayWords(words);
    } else if (overlayState.overlay->type ==
               wc24h::OVERLAY_TEMPERATURE_DIGITS) {
        const int value = static_cast<int>(lroundf(overlayState.celsius));
        char digits[4];
        snprintf(digits, sizeof(digits), "%2d",
                 value < -9 ? -9 : (value > 99 ? 99 : value));
        led.showNumbers(digits[0], digits[1]);
    } else {
        stopOverlay();
    }
}

//------------------------------------------------------------------------------

void ClockWork::calcClockface() {
    led.resetFrontMatrixBuffer();

    if (_hour == 23 && _minute == 59 && _second >= 50) {
        countdownToMidnight();
    } else {
        setClock();
    }

    if (usedClockType->hasWeatherLayout()) {
        weather.calcWeatherClockface();
    }
}

//------------------------------------------------------------------------------

void ClockWork::clearClockByProgInit() {
    if (G.progInit) {
        G.progInit = false;
        led.clear();
        led.show();
    }
}

//------------------------------------------------------------------------------
// Games, timer, messages, mood animations and alarm clock
//------------------------------------------------------------------------------

namespace {

wc24h::TetrisGame tetrisGame;
wc24h::SnakeGame snakeGame;
wc24h::SimonGame simonGame;
wc24h::FlappyGame flappyGame;
wc24h::BreakoutGame breakoutGame;
wc24h::InvadersGame invadersGame;
wc24h::FroggerGame froggerGame;
wc24h::PongGame pongGame;
wc24h::Connect4Game connect4Game;
wc24h::ReactionGame reactionGame;
wc24h::HangmanGame hangmanGame;
wc24h::ClockQuizGame clockQuizGame;
wc24h::WordFindGame wordFindGame;
wc24h::MathGame mathGame;
wc24h::PaintGame paintGame;

wc24h::ArcadeGame *game = nullptr;
uint8_t gameId = wc24h::GAME_NONE;
char gameName[9] = "";
bool gamePaused = false;
uint32_t gameLastDraw = 0;
uint32_t gameOverMillis = 0;
int8_t gameRank = 0; // place in the high score list, 0 = none
wc24h::GameStatus gameReported;
bool gameReportedOver = false;
bool gameReportedPaused = false;
uint32_t gameLastReport = 0;
wc24h::Canvas gameCanvas;

wc24h::CountTimer countTimer;
uint32_t timerShownSeconds = UINT32_MAX;
uint32_t timerFinishedMillis = 0;
bool timerReportedRunning = false;

wc24h::Ambient ambient;
wc24h::RgbFrame ambientFrame;
uint32_t ambientLastFrame = 0;
uint32_t ambientUntil = 0; // 0 = until another mode is chosen

int16_t messageOffset = 0;
uint8_t messagePass = 0;
uint32_t messageLastStep = 0;

uint32_t alarmStartMillis = 0;

constexpr uint32_t GAME_OVER_BLINK_MS = 2500;
constexpr uint32_t GAME_OVER_SCORE_MS = 3000;
constexpr uint32_t TIMER_ALARM_MS = 15000;
constexpr uint32_t ALARM_RING_MS = 5UL * 60UL * 1000UL;
constexpr uint32_t NEW_YEAR_FIREWORKS_MS = 90UL * 1000UL;
constexpr uint8_t HIGHSCORE_COUNT = 5;
const char *const HIGHSCORE_NAMESPACE = "wc24h-hs";

// Digital clock with the 12/24 hour and leading zero options applied.
void showDigitalTime() {
    uint8_t hour = _hour;
    if (G.digitalClockFlags & DIGITAL_12_HOURS) {
        hour %= 12;
        if (hour == 0) {
            hour = 12;
        }
    }
    const char tens = (hour < 10 && (G.digitalClockFlags &
                                     DIGITAL_NO_LEADING_ZERO))
                          ? ' '
                          : hour / 10;
    led.showDigitalClock(_minute % 10, _minute / 10, hour % 10, tens);
}

float gameBrightness() {
    if (!led.getState()) {
        return 0.f; // switched off by the power key or a night timer
    }
    const float b = led.getColorbyPositionWithAppliedBrightness(Foreground).B;
    return b < 0.08f ? 0.08f : b;
}

void returnToWordClock() {
    game = nullptr;
    gameId = wc24h::GAME_NONE;
    countTimer.stop();
    ambient.effect = wc24h::AMBIENT_NONE;
    G.prog = COMMAND_MODE_WORD_CLOCK;
    G.progInit = true;
    parametersChanged = true;
}

void broadcastJson(const JsonDocument &doc) {
    String payload;
    serializeJson(doc, payload);
    webSocket.broadcastTXT(payload);
}

//------------------------------------------------------------------------------
// Drawing

HsbColor paletteColor(uint8_t index, float brightness) {
    const wc24h::PaletteHsv c = wc24h::paletteHsv(index);
    return HsbColor(c.h, c.s, c.v * brightness);
}

void drawCanvas(const wc24h::Canvas &canvas) {
    float bri = gameBrightness();
    if (game == &paintGame) {
        bri *= paintGame.brightness / 100.f;
    }
    for (uint8_t row = 0; row < wc24h::GAME_ROWS; row++) {
        for (uint8_t col = 0; col < wc24h::GAME_COLS; col++) {
            led.setPixel(row, col, paletteColor(canvas.cell[row][col], bri));
        }
    }
    led.show();
}

void drawRgbFrame(const wc24h::RgbFrame &frame) {
    const float bri = gameBrightness();
    for (uint8_t row = 0; row < wc24h::GAME_ROWS; row++) {
        for (uint8_t col = 0; col < wc24h::GAME_COLS; col++) {
            const wc24h::Rgb &p = frame.px[row][col];
            HsbColor color(RgbColor(p.r, p.g, p.b));
            color.B *= bri;
            led.setPixel(row, col, color);
        }
    }
    led.show();
}

//------------------------------------------------------------------------------
// High scores, kept in the ESP32 NVS (Preferences), five per game

struct HighscoreEntry {
    char name[9];
    uint32_t score;
};

void loadHighscores(uint8_t id, HighscoreEntry *list) {
    memset(list, 0, sizeof(HighscoreEntry) * HIGHSCORE_COUNT);
    Preferences prefs;
    if (!prefs.begin(HIGHSCORE_NAMESPACE, true)) {
        return; // nothing stored yet
    }
    char key[8];
    snprintf(key, sizeof(key), "g%u", id);
    prefs.getBytes(key, list, sizeof(HighscoreEntry) * HIGHSCORE_COUNT);
    prefs.end();
    for (uint8_t i = 0; i < HIGHSCORE_COUNT; i++) {
        list[i].name[8] = '\0';
    }
}

// Returns the place 1..5, or 0 when the score is not good enough.
int8_t saveHighscore(uint8_t id, const char *name, uint32_t score) {
    if (!score) {
        return 0;
    }
    HighscoreEntry list[HIGHSCORE_COUNT];
    loadHighscores(id, list);
    int8_t place = -1;
    for (uint8_t i = 0; i < HIGHSCORE_COUNT; i++) {
        if (score > list[i].score) {
            place = i;
            break;
        }
    }
    if (place < 0) {
        return 0;
    }
    memmove(&list[place + 1], &list[place],
            sizeof(HighscoreEntry) * (HIGHSCORE_COUNT - 1 - place));
    memset(&list[place], 0, sizeof(HighscoreEntry));
    strlcpy(list[place].name, name[0] ? name : "?", sizeof(list[place].name));
    list[place].score = score;
    Preferences prefs;
    if (prefs.begin(HIGHSCORE_NAMESPACE, false)) {
        char key[8];
        snprintf(key, sizeof(key), "g%u", id);
        prefs.putBytes(key, list, sizeof(list));
        prefs.end();
    }
    return place + 1;
}

// client < 0: to everybody
void sendHighscores(uint8_t id, int16_t client) {
    HighscoreEntry list[HIGHSCORE_COUNT];
    loadHighscores(id, list);
    StaticJsonDocument<512> doc;
    doc["command"] = "highscores";
    doc["game"] = id;
    JsonArray array = doc.createNestedArray("list");
    for (const HighscoreEntry &entry : list) {
        if (!entry.score) {
            break;
        }
        JsonObject item = array.createNestedObject();
        item["name"] = entry.name;
        item["score"] = entry.score;
    }
    if (client < 0) {
        broadcastJson(doc);
    } else {
        sendJsonToClient(client, doc);
    }
}

//------------------------------------------------------------------------------
// Paint board pictures, eight slots in the NVS, shared by all phones

constexpr uint8_t PICTURE_SLOTS = 8;
const char *const PICTURE_NAMESPACE = "wc24h-pic";

struct StoredPicture {
    uint8_t cells[wc24h::GAME_ROWS * wc24h::GAME_COLS];
    uint8_t brightness;
};

bool loadPicture(uint8_t slot, StoredPicture &picture) {
    Preferences prefs;
    if (slot >= PICTURE_SLOTS || !prefs.begin(PICTURE_NAMESPACE, true)) {
        return false;
    }
    char key[4];
    snprintf(key, sizeof(key), "p%u", slot);
    const bool found = prefs.isKey(key) &&
                       prefs.getBytes(key, &picture, sizeof(picture)) ==
                           sizeof(picture);
    prefs.end();
    return found;
}

// picture == nullptr deletes the slot
void storePicture(uint8_t slot, const StoredPicture *picture) {
    Preferences prefs;
    if (slot >= PICTURE_SLOTS || !prefs.begin(PICTURE_NAMESPACE, false)) {
        return;
    }
    char key[4];
    snprintf(key, sizeof(key), "p%u", slot);
    if (picture) {
        prefs.putBytes(key, picture, sizeof(*picture));
    } else {
        prefs.remove(key);
    }
    prefs.end();
}

// {"command":"picture","slot":n,"bri":b,"cells":"<2 hex digits per cell>"},
// "cells" is empty for a free slot. client < 0: to everybody
void sendPicture(uint8_t slot, int16_t client) {
    static char buffer[80 + sizeof(StoredPicture::cells) * 2];
    static const char HEX_DIGITS[] = "0123456789abcdef";
    StoredPicture picture;
    const bool found = loadPicture(slot, picture);
    int n = snprintf(buffer, sizeof(buffer),
                     "{\"command\":\"picture\",\"slot\":%u,\"bri\":%u,"
                     "\"cells\":\"",
                     slot, found ? picture.brightness : 0);
    if (found) {
        for (uint8_t cell : picture.cells) {
            buffer[n++] = HEX_DIGITS[cell >> 4];
            buffer[n++] = HEX_DIGITS[cell & 0x0F];
        }
    }
    buffer[n++] = '"';
    buffer[n++] = '}';
    buffer[n] = '\0';
    if (client < 0) {
        webSocket.broadcastTXT(buffer, n);
    } else {
        webSocket.sendTXT(client, buffer, n);
    }
}

// Save, load and delete need the NVS, which the host-testable PaintGame
// does not know about.
bool handlePictureKey(const GameKeyEvent &event) {
    const uint8_t slot = event.value;
    switch (event.key) {
    case wc24h::GAME_KEY_PAINT_SAVE: {
        StoredPicture picture;
        memcpy(picture.cells, paintGame.picture.cell, sizeof(picture.cells));
        picture.brightness = paintGame.brightness;
        storePicture(slot, &picture);
        sendPicture(slot, -1);
        return true;
    }
    case wc24h::GAME_KEY_PAINT_LOAD: {
        StoredPicture picture;
        if (loadPicture(slot, picture)) {
            for (uint16_t i = 0; i < sizeof(picture.cells); i++) {
                const uint8_t color = picture.cells[i];
                paintGame.picture.cell[i / wc24h::GAME_COLS]
                                      [i % wc24h::GAME_COLS] =
                    color < wc24h::PAL_EXT_COUNT ? color : 0;
            }
            if (picture.brightness >= 5 && picture.brightness <= 100) {
                paintGame.brightness = picture.brightness;
            }
            paintGame.dirty = true;
        }
        return true;
    }
    case wc24h::GAME_KEY_PAINT_DELETE:
        storePicture(slot, nullptr);
        sendPicture(slot, -1);
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// Games

wc24h::ArcadeGame *gameFor(uint8_t id) {
    switch (id) {
    case wc24h::GAME_TETRIS:
        return &tetrisGame;
    case wc24h::GAME_SNAKE:
        return &snakeGame;
    case wc24h::GAME_SIMON:
        return &simonGame;
    case wc24h::GAME_FLAPPY:
        return &flappyGame;
    case wc24h::GAME_BREAKOUT:
        return &breakoutGame;
    case wc24h::GAME_INVADERS:
        return &invadersGame;
    case wc24h::GAME_FROGGER:
        return &froggerGame;
    case wc24h::GAME_PONG:
    case wc24h::GAME_PONG_CPU:
        pongGame.cpu = id == wc24h::GAME_PONG_CPU;
        return &pongGame;
    case wc24h::GAME_CONNECT4:
    case wc24h::GAME_CONNECT4_CPU:
        connect4Game.cpu = id == wc24h::GAME_CONNECT4_CPU;
        return &connect4Game;
    case wc24h::GAME_REACTION:
        return &reactionGame;
    case wc24h::GAME_HANGMAN:
        return &hangmanGame;
    case wc24h::GAME_CLOCK_QUIZ:
        clockQuizGame.displayMode =
            G.wc24hDisplayMode < wc24h::CLOCK_MODES_COUNT ? G.wc24hDisplayMode
                                                         : 13;
        return &clockQuizGame;
    case wc24h::GAME_WORD_FIND:
        return &wordFindGame;
    case wc24h::GAME_MATH:
        return &mathGame;
    case wc24h::GAME_PAINT:
        return &paintGame;
    default:
        return nullptr;
    }
}

void reportGame(bool force = false) {
    if (!game) {
        return;
    }
    wc24h::GameStatus status;
    game->status(status);
    const uint32_t now = millis();
    const bool changed =
        memcmp(&status, &gameReported, sizeof(status)) != 0 ||
        game->over != gameReportedOver || gamePaused != gameReportedPaused;
    if (!force && (!changed || now - gameLastReport < 100)) {
        return;
    }
    gameReported = status;
    gameReportedOver = game->over;
    gameReportedPaused = gamePaused;
    gameLastReport = now;

    StaticJsonDocument<384> doc;
    doc["command"] = "game";
    doc["game"] = gameId;
    doc["score"] = status.score;
    doc["over"] = game->over;
    doc["paused"] = gamePaused;
    if (status.lives >= 0) {
        doc["lives"] = status.lives;
    }
    if (status.level) {
        doc["level"] = status.level;
    }
    if (game->twoPlayer) {
        doc["score1"] = status.score1;
        doc["score2"] = status.score2;
        doc["turn"] = status.turn;
    }
    doc["winner"] = status.winner;
    doc["text"] = status.text;
    doc["text2"] = status.text2;
    doc["rank"] = gameRank;
    broadcastJson(doc);
}

void startGame(uint8_t id) {
    game = gameFor(id);
    if (!game) {
        id = wc24h::GAME_TETRIS;
        game = &tetrisGame;
    }
    gameId = id;
    strlcpy(gameName, gamePlayerName, sizeof(gameName));
    game->start(esp_random(), millis());
    gamePaused = false;
    gameOverMillis = 0;
    gameRank = 0;
    gameKeyCount = 0;
    gameLastDraw = 0;
    reportGame(true);
}

// Final screen: the score, or the winner's color for two players.
void drawGameResult() {
    wc24h::GameStatus status;
    game->status(status);
    gameCanvas.clear();
    if (game->twoPlayer) {
        const bool connect4 = gameId == wc24h::GAME_CONNECT4 ||
                              gameId == wc24h::GAME_CONNECT4_CPU;
        uint8_t color = wc24h::PAL_WHITE;
        if (status.winner == 1) {
            color = connect4 ? wc24h::PAL_RED : wc24h::PAL_BLUE;
        } else if (status.winner == 2) {
            color = connect4 ? wc24h::PAL_YELLOW : wc24h::PAL_ORANGE;
        }
        if (connect4) {
            gameCanvas.fill(2, 2, 12, 14, color);
        } else {
            char line[8];
            snprintf(line, sizeof(line), "%u:%u", status.score1,
                     status.score2);
            gameCanvas.centeredText(5, line, color);
        }
    } else {
        wc24h::drawScore(gameCanvas, status.score,
                         gameRank ? wc24h::PAL_GREEN : wc24h::PAL_YELLOW);
    }
    drawCanvas(gameCanvas);
}

void loopGame() {
    const uint32_t now = millis();

    if (gameRequest != wc24h::GAME_NONE) {
        startGame(gameRequest);
        gameRequest = wc24h::GAME_NONE;
    }
    if (!game) {
        startGame(wc24h::GAME_TETRIS);
    }

    bool changed = false;
    for (uint8_t i = 0; i < gameKeyCount; i++) {
        const GameKeyEvent &event = gameKeyQueue[i];
        if (game->over) {
            continue;
        }
        if (event.key == wc24h::GAME_KEY_PAUSE) {
            if (game->pausable) {
                gamePaused = !gamePaused;
                changed = true;
            }
            continue;
        }
        if (gamePaused) {
            continue;
        }
        if (game == &paintGame && handlePictureKey(event)) {
            changed = true;
            continue;
        }
        game->input({event.key, event.player, event.value}, now);
        changed = true;
    }
    gameKeyCount = 0;

    if (!game->over && !gamePaused) {
        changed |= game->update(now);
    }

    if (game->over) {
        if (gameOverMillis == 0) {
            gameOverMillis = now;
            if (wc24h::gameHasHighscore(gameId)) {
                wc24h::GameStatus status;
                game->status(status);
                gameRank = saveHighscore(gameId, gameName, status.score);
                reportGame(true);
                sendHighscores(gameId, -1);
            }
        }
        const uint32_t elapsed = now - gameOverMillis;
        if (elapsed < GAME_OVER_BLINK_MS) {
            const bool visible = (elapsed / 300) % 2 == 0;
            if (visible) {
                game->render(gameCanvas, now);
                drawCanvas(gameCanvas);
            } else {
                led.clear();
                led.show();
            }
        } else if (elapsed < GAME_OVER_BLINK_MS + GAME_OVER_SCORE_MS) {
            drawGameResult();
        } else {
            reportGame(true);
            returnToWordClock();
            return;
        }
        reportGame();
        return;
    }

    if (changed || now - gameLastDraw >= 100) {
        game->render(gameCanvas, now);
        drawCanvas(gameCanvas);
        gameLastDraw = now;
    }
    reportGame();
}

//------------------------------------------------------------------------------
// Timer

void reportTimer(uint32_t seconds) {
    StaticJsonDocument<128> doc;
    doc["command"] = "timer";
    doc["mode"] = countTimer.mode;
    doc["running"] = countTimer.running;
    doc["seconds"] = seconds;
    doc["finished"] = timerFinishedMillis != 0;
    broadcastJson(doc);
}

//------------------------------------------------------------------------------
// Message: scrolls a text from the phone across the front

void startMessage() {
    messageOffset = wc24h::GAME_COLS;
    messagePass = 0;
    messageLastStep = 0;
}

void loopMessage() {
    const uint32_t now = millis();
    if (now - messageLastStep < 70) {
        return;
    }
    messageLastStep = now;

    const size_t len = strlen(messageText);
    if (!len) {
        returnToWordClock();
        return;
    }
    const int16_t width = len * 6;
    const uint8_t top = (wc24h::GAME_ROWS - 7) / 2;
    const HsbColor on = led.getColorbyPositionWithAppliedBrightness(Foreground);
    const HsbColor off(0.f, 0.f, 0.f);
    for (uint8_t col = 0; col < wc24h::GAME_COLS; col++) {
        const int16_t x = col - messageOffset;
        const bool inside = x >= 0 && x < width && x % 6 < 5;
        const unsigned char ch =
            inside ? static_cast<unsigned char>(messageText[x / 6]) : ' ';
        for (uint8_t row = 0; row < wc24h::GAME_ROWS; row++) {
            const bool lit = inside && row >= top && row < top + 7 &&
                             led.getCharCol(normalSizeASCII, x % 6, row - top,
                                            ch);
            led.setPixel(row, col, lit ? on : off);
        }
    }
    led.show();

    if (--messageOffset < -width) {
        messageOffset = wc24h::GAME_COLS;
        if (++messagePass >= messageRepeats) {
            returnToWordClock();
        }
    }
}

//------------------------------------------------------------------------------
// Mood animations, also used for the alarm clock and New Year's fireworks

void startAmbient(uint8_t effect, uint32_t durationMs) {
    const uint32_t now = millis();
    ambient.start(effect, esp_random(), now);
    ambientFrame.clear();
    ambientLastFrame = 0;
    ambientUntil = durationMs ? now + durationMs : 0;
}

void loopAmbient() {
    const uint32_t now = millis();
    if (ambientRequest) {
        startAmbient(ambientRequest, 0);
        ambientRequest = 0;
    }
    if (ambient.effect == wc24h::AMBIENT_NONE) {
        startAmbient(wc24h::AMBIENT_LAVA, 0);
    }
    if (ambientUntil && static_cast<int32_t>(now - ambientUntil) >= 0) {
        returnToWordClock();
        return;
    }
    if (now - ambientLastFrame < ambient.frameMs()) {
        return;
    }
    ambientLastFrame = now;
    ambient.render(ambientFrame, now);
    drawRgbFrame(ambientFrame);
}

void reportAlarm(bool ringing) {
    StaticJsonDocument<64> doc;
    doc["command"] = "alarm";
    doc["ringing"] = ringing;
    broadcastJson(doc);
}

void stopAlarm() {
    reportAlarm(false);
    returnToWordClock();
}

// Called once per minute.
void checkAlarms(const struct tm &tm) {
    for (const AlarmSetting &alarm : G.alarms) {
        if (!(alarm.flags & ALARM_ACTIVE) ||
            !(alarm.flags & (1u << tm.tm_wday)) || alarm.hour != tm.tm_hour ||
            alarm.minute != tm.tm_min) {
            continue;
        }
        if (!led.getState()) {
            led.setState(true);
        }
        overlayActive = false;
        G.prog = COMMAND_MODE_ALARM;
        G.progInit = true;
        alarmStartMillis = millis();
        startAmbient(alarm.effect ? alarm.effect : wc24h::AMBIENT_SUNRISE,
                     ALARM_RING_MS);
        reportAlarm(true);
        return;
    }
    if ((G.wordColorFlags & WORD_COLORS_NEW_YEAR) && tm.tm_mon == 0 &&
        tm.tm_mday == 1 && tm.tm_hour == 0 && tm.tm_min == 0 &&
        (G.prog == COMMAND_IDLE || G.prog == COMMAND_MODE_WORD_CLOCK)) {
        overlayActive = false;
        G.prog = COMMAND_MODE_AMBIENT;
        G.progInit = true;
        startAmbient(wc24h::AMBIENT_FIREWORKS, NEW_YEAR_FIREWORKS_MS);
    }
}

void loopAlarm() {
    loopAmbient();
    if (G.prog != COMMAND_MODE_ALARM) {
        reportAlarm(false); // the ringing time is over
    }
}

//------------------------------------------------------------------------------
// Live picture for the web page: 288 pixels as hex, row by row from the
// top left.

void sendFrame(uint8_t client) {
    static char buffer[64 + wc24h::GAME_ROWS * wc24h::GAME_COLS * 6];
    const uint8_t perLetter = getLedsPerLetter(G.buildTypeDef);
    int n = snprintf(buffer, sizeof(buffer),
                     "{\"command\":\"frame\",\"prog\":%u,\"power\":%u,"
                     "\"rssi\":%d,\"rgb\":\"",
                     G.prog, led.getState() ? 1 : 0,
                     static_cast<int>(WiFi.RSSI()));
    static const char HEX_DIGITS[] = "0123456789abcdef";
    for (uint8_t row = 0; row < wc24h::GAME_ROWS; row++) {
        for (uint8_t col = 0; col < wc24h::GAME_COLS; col++) {
            const RgbColor px = led.getPixel(
                usedClockType->getFrontMatrixIndex(row, col) * perLetter);
            const uint8_t values[3] = {px.R, px.G, px.B};
            for (uint8_t v : values) {
                buffer[n++] = HEX_DIGITS[v >> 4];
                buffer[n++] = HEX_DIGITS[v & 0x0F];
            }
        }
    }
    buffer[n++] = '"';
    buffer[n++] = '}';
    buffer[n] = '\0';
    webSocket.sendTXT(client, buffer, n);
}

//------------------------------------------------------------------------------
// Learning colors: the hour words in a second color, so children see which
// part of the sentence is the hour. Re-applied every second on top of the
// word clock.

void applyLearningColors() {
    if (!(G.wordColorFlags & WORD_COLORS_LEARN) || !led.getState() ||
        !usedClockType->hasWc24hTables() ||
        G.wc24hDisplayMode >= wc24h::CLOCK_MODES_COUNT) {
        return;
    }
    wc24h::WordSet words;
    wc24h::WordSet hourWords;
    wc24h::fillWords(G.wc24hDisplayMode, _hour, _minute, words, hourWords);
    HsbColor color = led.getColorbyPositionWithAppliedBrightness(Foreground);
    color.H += 1.f / 3.f;
    if (color.H >= 1.f) {
        color.H -= 1.f;
    }
    if (color.S < 0.6f) {
        color.S = 1.f; // white text: use a clear orange for the hours
        color.H = 0.08f;
    }
    for (uint8_t idx = 1; idx < wc24h::WP_COUNT; idx++) {
        if (!hourWords[idx]) {
            continue;
        }
        const wc24h::WORD_ILLUMINATION &w = wc24h::illumination[idx];
        for (uint8_t i = 0; i < (w.len & wc24h::ILLUMINATION_LEN_MASK); i++) {
            led.setPixel(w.row, w.col + i, color);
        }
    }
    led.show();
}

void loopTimer() {
    const uint32_t now = millis();

    switch (timerRequest) {
    case wc24h::TIMER_ACTION_COUNTDOWN:
        countTimer.startCountdown(timerRequestSeconds, now);
        break;
    case wc24h::TIMER_ACTION_STOPWATCH:
        countTimer.startStopwatch(now);
        break;
    case wc24h::TIMER_ACTION_PAUSE:
        countTimer.togglePause(now);
        break;
    case wc24h::TIMER_ACTION_STOP:
        timerRequest = wc24h::TIMER_ACTION_NONE;
        timerFinishedMillis = 0;
        reportTimer(0);
        returnToWordClock();
        return;
    default:
        break;
    }
    if (timerRequest != wc24h::TIMER_ACTION_NONE) {
        timerRequest = wc24h::TIMER_ACTION_NONE;
        timerFinishedMillis = 0;
        timerShownSeconds = UINT32_MAX;
    }
    if (countTimer.mode == wc24h::TIMER_OFF) {
        returnToWordClock();
        return;
    }

    if (countTimer.finished(now)) {
        if (timerFinishedMillis == 0) {
            timerFinishedMillis = now;
            reportTimer(0);
        }
        // Flash 00:00 until the alarm time is over.
        const uint32_t phase = (now - timerFinishedMillis) / 400;
        if (phase != timerShownSeconds) {
            timerShownSeconds = phase;
            if (phase % 2 == 0) {
                led.showDigitalClock(0, 0, 0, 0, false);
            } else {
                led.clear();
                led.show();
            }
        }
        if (now - timerFinishedMillis >= TIMER_ALARM_MS) {
            timerFinishedMillis = 0;
            returnToWordClock();
        }
        return;
    }

    const uint32_t seconds = countTimer.displaySeconds(now);
    if (seconds != timerShownSeconds ||
        countTimer.running != timerReportedRunning) {
        timerShownSeconds = seconds;
        timerReportedRunning = countTimer.running;
        const uint8_t mm = seconds / 60;
        const uint8_t ss = seconds % 60;
        led.showDigitalClock(ss % 10, ss / 10, mm % 10, mm / 10, false);
        reportTimer(seconds);
    }
}

} // namespace

//------------------------------------------------------------------------------
// Loop Functions
//------------------------------------------------------------------------------

void ClockWork::loop(struct tm &tm) {
    unsigned long currentMillis = millis();
    countMillisSpeed += currentMillis - previousMillis;
    previousMillis = currentMillis;
    loopHardwareButtons();

    // Faster runtime for demo
    transition->demoMode(_hour, _minute, _second);

    //------------------------------------------------
    // Seconds and LDR Routine
    //------------------------------------------------
    if (lastSecond != _second) {

#if GENERAL_VERBOSE
        char currentTime[80];
        strftime(currentTime, sizeof(currentTime), "%F %T (%z)\n", &tm);
        Serial.printf(currentTime);
#endif

        //--------------------------------------------
        // Weather Loop
        //--------------------------------------------
        if (usedClockType->hasWeatherLayout()) {
            weather.loop();
        }

        //--------------------------------------------
        // Auto Brightness Logic
        //--------------------------------------------
        if (G.autoBrightEnabled == 1) {
            loopAutoBrightLogic();
        }

        currentWeather.loop(isCurrentWeatherNeeded());
        loopColorAnimation();

        if (G.prog == COMMAND_IDLE && G.conf == COMMAND_IDLE) {
            led.clear();
            G.prog = COMMAND_MODE_WORD_CLOCK;
        }

        if (G.prog == COMMAND_MODE_DIGITAL_CLOCK) {
            led.clear();
            showDigitalTime();
        }

        if (G.prog == COMMAND_IDLE && !overlayActive) {
            applyLearningColors();
        }

        lastSecond = _second;
    }

    //------------------------------------------------
    // Minute
    //------------------------------------------------
    if (lastMinute != _minute) {
        lastMinute = _minute;
        loopNightTimers(tm);
        checkAlarms(tm);
        startOverlay(tm);
        if (colorChangedByWebsite) {
            eeprom::write();
            colorChangedByWebsite = false;
            Serial.println("Saved Color");
        }

        //------------------------------------------------
        // MQTT
        //------------------------------------------------

        sendMQTTUpdate();
    }

    switch (G.conf) {

    case COMMAND_RESET: {
        delay(500);
#ifdef ESP8266
        ESP.reset();
        ESP.restart();
#elif defined(ESP32)
        ESP.restart();
        esp_restart();
#endif
        while (true) {
        }
        break;
    }

    case COMMAND_REQUEST_FRAME: {
        sendFrame(G.client_nr);
        break;
    }

    case COMMAND_REQUEST_PICTURES: {
        for (uint8_t slot = 0; slot < PICTURE_SLOTS; slot++) {
            sendPicture(slot, G.client_nr);
        }
        break;
    }

    case COMMAND_REQUEST_HIGHSCORES: {
        if (wc24h::gameHasHighscore(highscoreRequestGame)) {
            sendHighscores(highscoreRequestGame, G.client_nr);
        }
        break;
    }

    case COMMAND_REQUEST_MQTT_VALUES: {
        DynamicJsonDocument config(512);
        config["command"] = "mqtt";
        config["MQTT_State"] = G.mqtt.state;
        config["MQTT_Port"] = G.mqtt.port;
        config["MQTT_Server"] = G.mqtt.serverAdress;
        config["MQTT_User"] = G.mqtt.user;

        // don't expose password:
        char passMasked[sizeof(G.mqtt.password) + 1] = {0};
        sensitive::maskPreservingSuffix(passMasked, G.mqtt.password);
        config["MQTT_Pass"] = passMasked;

        config["MQTT_ClientId"] = G.mqtt.clientId;
        config["MQTT_Topic"] = G.mqtt.topic;

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_CONFIG_VALUES: {
        DynamicJsonDocument config(1792);
        config["command"] = "config";
        config["ssid"] = network.getSSID();
        config["timeserver"] = G.timeserver;
        config["timezone"] = G.timezone;
        config["hostname"] = G.hostname;
        config["scrollingText"] = G.scrollingText;
        config["h6"] = G.h6;
        config["h8"] = G.h8;
        config["h12"] = G.h12;
        config["h16"] = G.h16;
        config["h18"] = G.h18;
        config["h20"] = G.h20;
        config["h22"] = G.h22;
        config["h24"] = G.h24;
        for (uint8_t i = 0;
             i < sizeof(G.languageVariant) / sizeof(G.languageVariant[0]);
             i++) {
            char string2Send[11];
            sprintf(string2Send, "langVar%d", i);
            config[string2Send] = static_cast<uint8_t>(G.languageVariant[i]);
        }
        for (uint8_t i = 0;
             i < sizeof(G.layoutVariant) / sizeof(G.layoutVariant[0]); i++) {
            char string2Send[11];
            sprintf(string2Send, "layVar%d", i);
            config[string2Send] = static_cast<uint8_t>(G.layoutVariant[i]);
        }
        config["effectBri"] = G.effectBri;
        config["itIsVariant"] = static_cast<uint8_t>(G.itIsVariant);
        config["secondVariant"] = static_cast<uint8_t>(G.secondVariant);
        config["minuteVariant"] = static_cast<uint8_t>(G.minuteVariant);
        config["supportedMinuteVariants"] =
            usedClockType->supportedMinuteVariantMask();
        config["cityid"] = G.openWeatherMap.cityid;
        char apiKeyMasked[sizeof(G.openWeatherMap.apikey) + 1] = {0};
        sensitive::maskPreservingSuffix(apiKeyMasked, G.openWeatherMap.apikey);
        config["apiKey"] = apiKeyMasked;
        config["colortype"] = G.Colortype;
        config["buildtype"] = static_cast<uint8_t>(G.buildTypeDef);
        config["wType"] = static_cast<uint8_t>(G.wType);
        config["clockTypeDef"] = G.clockTypeDef;
        config["bitmapSymbol"] = static_cast<uint8_t>(G.bitmapSymbol);
        config["bootLedBlink"] = G.bootLedBlink;
        config["bootLedSweep"] = G.bootLedSweep;
        config["bootShowWifi"] = G.bootShowWifi;
        config["bootShowIP"] = G.bootShowIP;
        config["bootSpeed"] = G.bootSpeed;
        config["digitalClockFlags"] = G.digitalClockFlags;
        config["wordColorFlags"] = G.wordColorFlags;
        JsonArray alarms = config.createNestedArray("alarms");
        for (const AlarmSetting &alarm : G.alarms) {
            JsonArray entry = alarms.createNestedArray();
            entry.add(alarm.flags);
            entry.add(alarm.hour);
            entry.add(alarm.minute);
            entry.add(alarm.effect);
        }
        config["ledPin"] = G.hardwarePins.led;
        config["powerButtonPin"] = G.hardwarePins.powerButton;
        config["modeButtonPin"] = G.hardwarePins.modeButton;
        config["speedButtonPin"] = G.hardwarePins.speedButton;
        config["i2cSdaPin"] = G.i2cSdaPin;
        config["i2cSclPin"] = G.i2cSclPin;
        config["autoBrightEnabled"] = G.autoBrightEnabled;
        config["isRomanLanguage"] = usedClockType->isRomanLanguage();
        config["hasDreiviertel"] = usedClockType->hasDreiviertel();
        config["hasTwenty"] = usedClockType->hasTwenty();
        config["hasWeatherLayout"] = usedClockType->hasWeatherLayout();
        config["hasSecondsFrame"] = usedClockType->hasSecondsFrame();
        config["hasMinuteInWords"] = usedClockType->hasMinuteInWords();
        config["hasSpecialWordHappyBirthday"] =
            usedClockType->hasSpecialWordHappyBirthday();
        config["numOfRows"] = usedClockType->rowsWordMatrix();

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_BIRTHDAYS: {
        DynamicJsonDocument config(256);
        config["command"] = "birthdays";
        config["hasSpecialWordHappyBirthday"] =
            usedClockType->hasSpecialWordHappyBirthday();
        for (uint8_t i = 0; i < MAX_BIRTHDAY_COUNT; i++) {
            char dateString[10];
            char string2Send[16];
            snprintf(string2Send, sizeof(string2Send), "birthdayDate%d", i);
            snprintf(dateString, sizeof(dateString), "%02u-%02u",
                     G.birthday[i].month, G.birthday[i].day);
            config[string2Send] = dateString;
        }

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_I2C_SCAN: {
        sendI2CScanResult(G.client_nr, i2cScanSdaPin, i2cScanSclPin);
        break;
    }

    case COMMAND_REQUEST_WC24H: {
        DynamicJsonDocument config(4096);
        config["command"] = "wc24h";
        config["hasWc24hTables"] = usedClockType->hasWc24hTables();
        config["displayMode"] = G.wc24hDisplayMode;
        JsonArray modes = config.createNestedArray("displayModes");
        for (uint8_t i = 0; i < wc24h::DISPLAY_MODES_COUNT; i++) {
            modes.add(wc24h::modeName(i));
        }
        JsonArray timers = config.createNestedArray("nightTimers");
        for (const NightTimer &timer : G.nightTimers) {
            JsonArray entry = timers.createNestedArray();
            entry.add(timer.flags);
            entry.add(timer.hour);
            entry.add(timer.minute);
        }
        JsonArray overlays = config.createNestedArray("overlays");
        for (const wc24h::Overlay &overlay : G.overlays) {
            JsonArray entry = overlays.createNestedArray();
            entry.add(overlay.type);
            entry.add(overlay.flags);
            entry.add(overlay.interval);
            entry.add(overlay.duration);
            entry.add(overlay.dateCode);
            entry.add(overlay.month);
            entry.add(overlay.day);
            entry.add(overlay.days);
            entry.add(overlay.icon);
            entry.add(overlay.text);
        }
        JsonArray icons = config.createNestedArray("icons");
        for (const wc24h::Icon &icon : wc24h::ICONS) {
            icons.add(icon.name);
        }
        config["colorAnimation"] = G.colorAnimation;
        float celsius;
        if (readTemperature(celsius)) {
            config["temperature"] = celsius;
        }

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_COLOR_VALUES: {
        DynamicJsonDocument config(768);
        config["command"] = "set";
        for (uint8_t i = 0; i < 3; i++) {
            char string2Send[7];
            sprintf(string2Send, "hsb%d%d", i, 0);
            config[string2Send] = static_cast<uint16_t>(G.color[i].H * 360);
            sprintf(string2Send, "hsb%d%d", i, 1);
            config[string2Send] = static_cast<uint8_t>(G.color[i].S * 100);
            sprintf(string2Send, "hsb%d%d", i, 2);
            config[string2Send] = static_cast<uint8_t>(G.color[i].B * 100);
        }
        config["effectBri"] = G.effectBri;
        config["effectSpeed"] = G.effectSpeed;
        config["colortype"] = G.Colortype;
        config["hasSpecialWordHappyBirthday"] =
            usedClockType->hasSpecialWordHappyBirthday();
        config["hasSecondsFrame"] = usedClockType->hasSecondsFrame();
        config["prog"] = G.prog;
        config["bitmapSymbol"] = static_cast<uint8_t>(G.bitmapSymbol);

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_AUTO_BRIGHT: {
        DynamicJsonDocument config(256);
        config["command"] = "autoBright";
        config["autoBrightEnabled"] = G.autoBrightEnabled;
        if (G.param1 == 1) {
            config["autoBrightMin"] = G.autoBrightMin;
            config["autoBrightMax"] = G.autoBrightMax;
            config["autoBrightPeak"] = G.autoBrightPeak;
        }
        config["autoBrightSensor"] = (uint32_t)lux;
        config["autoBrightGain"] = (uint8_t)ledGain;
        config["autoBrightSource"] = autoBrightUsingBH1750 ? "BH1750" : "LDR";

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_REQUEST_TRANSITION: {
        DynamicJsonDocument config(256);
        config["command"] = "transition";
        config["transitionType"] = G.transitionType;
        config["transitionDuration"] = G.transitionDuration;
        config["transitionSpeed"] = G.transitionSpeed;
        config["transitionDemo"] = G.transitionDemo;
        config["transitionColorize"] = G.transitionColorize;

        sendJsonToClient(G.client_nr, config);
        break;
    }

    case COMMAND_SET_BIRTHDAYS:
    case COMMAND_SET_TIME:
    case COMMAND_SET_INITIAL_VALUES:
    case COMMAND_SET_WEATHER_DATA:
    case COMMAND_SET_SCROLLINGTEXT:
    case COMMAND_SET_TIMESERVER:
    case COMMAND_SET_BOOT: {
        eeprom::write();
        delay(100);
        break;
    }

    case COMMAND_SET_TIMEZONE: {
        applyTimezone();
        eeprom::write();
        parametersChanged = true;
        delay(100);
        break;
    }

    case COMMAND_SET_BUILDTYPE: {
        led.clear();

        // G.param1 sets new buildtype
        Serial.printf("Clock Buildtype: %u\n", G.param1);

        G.buildTypeDef = static_cast<BuildTypeDef>(G.param1);
        eeprom::write();
        led.clear();
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_WC24H_DISPLAY_MODE:
    case COMMAND_SET_NIGHT_TIMERS:
    case COMMAND_SET_OVERLAYS:
    case COMMAND_SET_COLOR_ANIMATION:
    case COMMAND_SET_DIGITAL_CLOCK:
    case COMMAND_SET_ALARMS:
    case COMMAND_SET_WORD_COLORS: {
        eeprom::write();
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_MINUTE:
        resetMinVariantIfNotAvailable();
        eeprom::write();
        led.clear();
        memset(frameArray, false, sizeof(frameArray));
        parametersChanged = true;
        break;

    case COMMAND_SET_BRIGHTNESS:
    case COMMAND_SET_AUTO_BRIGHT:
    case COMMAND_SET_WHITETYPE:
    case COMMAND_SET_TIME_MANUAL:
    case COMMAND_SET_LAYOUT_VARIANT: {
        eeprom::write();
        led.clear();
        memset(frameArray, false, sizeof(frameArray));
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_LANGUAGE_VARIANT:
    case COMMAND_SET_SETTING_SECOND: {
        eeprom::write();
        led.clear();
        G.progInit = true;
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_MQTT: {
        if (G.mqtt.state && G.progInit) {
            mqtt.init();
            G.progInit = false;
        }
        delay(100);

        eeprom::write();
        break;
    }

    case COMMAND_SET_MQTT_HA_DISCOVERY: {
        mqtt.sendDiscovery();
        break;
    }

    case COMMAND_SET_HARDWARE_PINS: {
        if (!hardwarePinsAreValid()) {
            Serial.println("Invalid hardware pin configuration, restoring "
                           "defaults");
            setDefaultHardwarePins();
            i2cBus::setDefaultPins(G.i2cSdaPin, G.i2cSclPin);
        }

        Serial.printf("Hardware LED pin: GPIO%u\n", G.hardwarePins.led);
        Serial.printf("Hardware power button pin: GPIO%u\n",
                      G.hardwarePins.powerButton);
        Serial.printf("Hardware mode button pin: GPIO%u\n",
                      G.hardwarePins.modeButton);
        Serial.printf("Hardware speed button pin: GPIO%u\n",
                      G.hardwarePins.speedButton);
        Serial.printf("I2C SDA pin: GPIO%u\n", G.i2cSdaPin);
        Serial.printf("I2C SCL pin: GPIO%u\n", G.i2cSclPin);

        eeprom::write();
        i2cBus::begin(G.i2cSdaPin, G.i2cSclPin);
        initHardwareButtons();
        initLedStrip(G.Colortype);
        led.clear();
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_COLORTYPE: {
        // G.param1 sets new Colortype
        Serial.printf("LED Colortype: %u\n", G.param1);

        // the G.Colortype must be called at the same time as initLedStrip,
        // otherwise it is referenced via a null-pointer.
        G.Colortype = G.param1;
        eeprom::write();
        initLedStrip(G.Colortype);

        clearClockByProgInit();
        parametersChanged = true;
        break;
    }

    case COMMAND_SET_CLOCK_TYPE: {
        if (!isValidClockTypeDef(G.clockTypeDef)) {
            Serial.printf("Ignoring invalid ClockType: %u\n", G.clockTypeDef);
            break;
        }

        eeprom::write();
        led.clear();
        led.show();
        delay(10);
        Serial.printf("ClockType: %u\n", G.clockTypeDef);

        usedClockType = getPointer(G.clockTypeDef);
        resetMinVariantIfNotAvailable();

        checkForValidLanguageVariant();

        delete secondsFrame;
        secondsFrame = nullptr;

        if (usedClockType->numPixelsFrameMatrix() != 0) {
            secondsFrame =
                new SecondsFrame(usedClockType->numPixelsFrameMatrix());
            G.progInit = true;
        }

        transition->resize(usedClockType->rowsWordMatrix(),
                           usedClockType->colsWordMatrix());
        transition->init();

        parametersChanged = true;
        break;
    }

    case COMMAND_SET_HOSTNAME: {
        Serial.print("Hostname: ");
        Serial.println(G.hostname);
        eeprom::write();
        network.reboot();
        break;
    }

    case COMMAND_SET_WIFI_DISABLED: {
        eeprom::write();
        delay(100);
        Serial.println("Conf: WLAN off");
        network.disable();
        break;
    }

    case COMMAND_SET_WIFI_AND_RESTART: {
        Serial.println("Conf: new Wifi Config");
        network.resetSettings();
        break;
    }

    default:
        break;
    }

    G.conf = COMMAND_IDLE;

    if (overlayActive) {
        loopOverlay();
        return;
    }

    switch (G.prog) {

    case COMMAND_MODE_SECONDS: {
        // 0xFF marks "nothing drawn yet" and cannot collide with a second.
        static uint8_t lastShownSecond = 0xFF;

        if (G.progInit) {
            clearClockByProgInit();
            lastShownSecond = 0xFF;
        }

        // This runs on every loop() iteration. Without this guard the LED
        // strip is rewritten thousands of times per second, which starves the
        // network stack -- the display only ever changes once per second.
        if (lastShownSecond == _second && !parametersChanged) {
            break;
        }
        lastShownSecond = _second;
        parametersChanged = false;

        char d1[5];
        char d2[5];
        sprintf(d1, "%d", static_cast<uint8_t>(_second / 10));
        sprintf(d2, "%d", static_cast<uint8_t>(_second % 10));
        led.showNumbers(d1[0], d2[0]);
        break;
    }

    case COMMAND_MODE_DIGITAL_CLOCK: {
        if (G.progInit) {
            clearClockByProgInit();
        }
        if (parametersChanged) {
            showDigitalTime();
            parametersChanged = false;
        }
        break;
    }

    case COMMAND_MODE_SCROLLINGTEXT:
    case COMMAND_MODE_RAINBOWCYCLE:
    case COMMAND_MODE_RAINBOW:
    case COMMAND_MODE_SYMBOL: {
        if (G.progInit) {
            countMillisSpeed = (11u - G.effectSpeed) * 30u;
            clearClockByProgInit();
        }

        if (countMillisSpeed >= (11u - G.effectSpeed) * 30u) {
            switch (G.prog) {
            case COMMAND_MODE_SCROLLINGTEXT: {
                scrollingText(G.scrollingText);
                break;
            }
            case COMMAND_MODE_RAINBOWCYCLE: {
                rainbowCycle();
                break;
            }
            case COMMAND_MODE_RAINBOW: {
                rainbow();
                break;
            }
            case COMMAND_MODE_SYMBOL: {
                displaySymbols(G.bitmapSymbol);
                break;
            }
            default:
                break;
            }
            countMillisSpeed = 0;
        }
        break;
    }

    case COMMAND_MODE_COLOR: {
        if (G.progInit) {
            clearClockByProgInit();
            for (uint8_t row = 0; row < usedClockType->rowsWordMatrix();
                 row++) {
                frontMatrix[row] = num32BitWithOnesAccordingToColumns();
            }
        }

        if (parametersChanged) {
            led.setbyFrontMatrix(Foreground, false);
            led.show();
            parametersChanged = false;
        }
        break;
    }

    case COMMAND_MODE_GAME: {
        if (G.progInit) {
            clearClockByProgInit();
            gameLastDraw = 0;
        }
        loopGame();
        break;
    }

    case COMMAND_MODE_MESSAGE: {
        if (G.progInit) {
            clearClockByProgInit();
            startMessage();
        }
        loopMessage();
        break;
    }

    case COMMAND_MODE_AMBIENT: {
        if (G.progInit) {
            G.progInit = false;
        }
        loopAmbient();
        break;
    }

    case COMMAND_MODE_ALARM: {
        if (G.progInit) {
            G.progInit = false;
        }
        loopAlarm();
        break;
    }

    case COMMAND_MODE_TIMER: {
        if (G.progInit) {
            clearClockByProgInit();
            timerShownSeconds = UINT32_MAX;
        }
        loopTimer();
        break;
    }

    case COMMAND_MODE_TRANSITION: {
        if (G.progInit) {
            G.progInit = false;
            eeprom::write();
            delay(100);
        }
        // There is no break here on purpose, directly after the call
        // COMMAND_MODE_TRANSITION, COMMAND_MODE_WORD_CLOCK must be called
    }

    case COMMAND_MODE_WORD_CLOCK: {
        clearClockByProgInit();
        calcClockface();

        switch (changesInClockface()) {
        case WordclockChanges::Minute:
            lastMinuteArray = minuteArray;
            memcpy(&lastFrontMatrix, &frontMatrix, sizeof lastFrontMatrix);
            led.set(WordclockChanges::Minute);
            break;
        case WordclockChanges::Words:
            lastMinuteArray = minuteArray;
            memcpy(&lastFrontMatrix, &frontMatrix, sizeof lastFrontMatrix);
            led.set(WordclockChanges::Words);
            break;
        case WordclockChanges::Layout:
            led.set(WordclockChanges::Layout);
            break;
        case WordclockChanges::Parameters:
            led.set(WordclockChanges::Parameters);
            break;
        default:
            break;
        }
        G.prog = COMMAND_IDLE;
    }
    default:
        break;
    }

    if (countMillisSpeed > 10000) {
        countMillisSpeed = 0;
    }
}
