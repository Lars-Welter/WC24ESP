#pragma once

#include "WordClockState.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

// Current temperature and weather icon from OpenWeatherMap, using the city id
// and API key of the weather settings. Stands in for the temperature sensors
// of the WordClock24h hardware, which the SP803E does not have.
class CurrentWeather {
public:
    // Fetch when needed: every 10 minutes, retry after a minute on errors.
    void loop(bool needed) {
        if (!needed || WiFi.status() != WL_CONNECTED ||
            G.openWeatherMap.cityid[0] == '\0' ||
            G.openWeatherMap.apikey[0] == '\0') {
            return;
        }
        const uint32_t now = millis();
        const uint32_t wait = valid ? FETCH_INTERVAL_MS : RETRY_INTERVAL_MS;
        if (lastAttempt != 0 && now - lastAttempt < wait) {
            return;
        }
        lastAttempt = now;
        if (fetch()) {
            lastSuccess = now;
            valid = true;
        }
    }

    bool getTemperature(float &celsius) const {
        if (!isFresh()) {
            return false;
        }
        celsius = temperature;
        return true;
    }

    // OpenWeatherMap icon code without day/night suffix, e.g. "10"
    const char *getIconCode() const { return isFresh() ? icon : ""; }

private:
    static constexpr uint32_t FETCH_INTERVAL_MS = 10UL * 60UL * 1000UL;
    static constexpr uint32_t RETRY_INTERVAL_MS = 60UL * 1000UL;
    static constexpr uint32_t MAX_AGE_MS = 30UL * 60UL * 1000UL;

    float temperature = 0.f;
    char icon[3] = "";
    bool valid = false;
    uint32_t lastAttempt = 0;
    uint32_t lastSuccess = 0;

    bool isFresh() const {
        return valid && millis() - lastSuccess < MAX_AGE_MS;
    }

    bool fetch() {
        static const char *host = "api.openweathermap.org";
        WiFiClient client;
        client.setTimeout(3000);

        if (!client.connect(host, 80)) {
            Serial.println("Current weather: connection failed");
            return false;
        }

        // HTTP/1.0 keeps the answer free of chunked transfer encoding.
        client.printf("GET /data/2.5/weather?id=%s&units=metric&appid=%s "
                      "HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
                      G.openWeatherMap.cityid, G.openWeatherMap.apikey, host);

        if (!client.find("\r\n\r\n")) {
            Serial.println("Current weather: no answer");
            client.stop();
            return false;
        }

        StaticJsonDocument<256> filter;
        filter["main"]["temp"] = true;
        filter["weather"][0]["icon"] = true;
        StaticJsonDocument<384> doc;
        const DeserializationError error =
            deserializeJson(doc, client, DeserializationOption::Filter(filter));
        client.stop();

        if (error || doc["main"]["temp"].isNull()) {
            Serial.printf("Current weather: invalid answer (%s)\n",
                          error.c_str());
            return false;
        }

        temperature = doc["main"]["temp"].as<float>();
        const char *code = doc["weather"][0]["icon"] | "";
        strlcpy(icon, code, sizeof(icon));
        Serial.printf("Current weather: %.1f C, icon %s\n", temperature, icon);
        return true;
    }
};
