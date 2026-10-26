// Text helpers for messages shown on the front.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace wc24h {

// Copies UTF-8 text into the plain ASCII the 7x5 font can show: German
// umlauts become AE, OE, UE, SS, other non-ASCII characters are dropped.
inline void toClockText(const char *in, char *out, size_t size) {
    if (!size) {
        return;
    }
    size_t n = 0;
    auto put = [&](char ch) {
        if (n + 1 < size) {
            out[n++] = ch;
        }
    };
    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(in);
         p && *p; p++) {
        if (*p < 0x80) {
            if (*p >= 0x20) {
                put(static_cast<char>(*p));
            }
            continue;
        }
        if (*p == 0xC3 && p[1]) {
            const char *repl = nullptr;
            switch (p[1]) {
            case 0x84: repl = "AE"; break; // Ä
            case 0x96: repl = "OE"; break; // Ö
            case 0x9C: repl = "UE"; break; // Ü
            case 0xA4: repl = "ae"; break; // ä
            case 0xB6: repl = "oe"; break; // ö
            case 0xBC: repl = "ue"; break; // ü
            case 0x9F: repl = "ss"; break; // ß
            default: break;
            }
            for (const char *r = repl; r && *r; r++) {
                put(*r);
            }
            p++;
            continue;
        }
        // skip the continuation bytes of other characters
        while ((p[1] & 0xC0) == 0x80) {
            p++;
        }
    }
    out[n] = '\0';
}

// Splits "[Washer] WASCHMASCHINE FERTIG" into the icon name and the text.
// Without brackets the icon name is empty.
inline void splitNotification(const char *in, char *icon, size_t iconSize,
                              const char **text) {
    if (iconSize) {
        icon[0] = '\0';
    }
    while (*in == ' ') {
        in++;
    }
    if (*in == '[') {
        const char *end = in + 1;
        while (*end && *end != ']') {
            end++;
        }
        if (*end == ']') {
            size_t n = 0;
            for (const char *p = in + 1; p < end && n + 1 < iconSize; p++) {
                icon[n++] = *p;
            }
            if (iconSize) {
                icon[n] = '\0';
            }
            in = end + 1;
            while (*in == ' ') {
                in++;
            }
        }
    }
    *text = in;
}

} // namespace wc24h
