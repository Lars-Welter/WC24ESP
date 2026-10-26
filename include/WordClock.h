#pragma once

#include <Arduino.h>

void applyTimezone();
void sendMQTTUpdate();
// Same as a Home Assistant notification, see ClockWork::showNotification().
void showNotification(int16_t icon, const char *text, uint8_t repeats);
