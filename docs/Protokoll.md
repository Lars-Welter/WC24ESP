# Protokoll und Aufbau (für Entwickler)

## Quellcode-Überblick

| Datei | Inhalt |
| --- | --- |
| `include/WC24h/Wc24hDisplay.h`, `Wc24hTablesDE.h` | Anzeigemodi der WordClock24h (Worttabellen) |
| `include/WC24h/Games.h` | Tetris, Snake, Countdown/Stoppuhr (Spiellogik) |
| `include/WC24h/Arcade.h` | übrige Spiele, gemeinsame Schnittstelle `ArcadeGame`, Spielfeld `Canvas` mit 3×5-Ziffernschrift |
| `include/WC24h/Ambient.h` | Stimmungsanimationen, Sonnenaufgang/Blinken für den Wecker |
| `include/WC24h/Calendar.h` | „Tage bis …", Mondphase |
| `include/WC24h/Sun.h` | Sonnenhöhe, Sonnenauf- und -untergang, Helligkeit nach Sonnenstand |
| `include/WC24h/Text.h` | Umlaute für die Laufschrift, `[Symbol] Text` zerlegen |
| `include/WC24h/Wc24hIcons.h` | Symbole, `findIcon()` mit deutschen Namen |
| `include/WC24h/Overlay.h`, `NightTimer.h` | Einblendungen, Nachtschaltung |
| `include/ClockWork.hpp` | Hauptschleife: Modi, Spiele, Timer, Nachricht, Stimmung, Wecker, Live-Bild, Bestenliste, Lern-Farben |
| `src/WebPageAdapter.cpp` | WebSocket-Befehle der Weboberfläche |
| `src/Mqtt.cpp` | MQTT und Home-Assistant-Discovery |
| `webpage/` | Weboberfläche (wird beim Build gebündelt, gzip, in die Firmware eingebettet) |
| `test/wc24h_test.cpp` | Host-Tests |

Alles unter `include/WC24h/` ist ohne Arduino lauffähig und wird von den
Host-Tests geprüft:

```sh
g++ -std=gnu++11 -Wall -Wextra -Werror -Iinclude test/wc24h_test.cpp -o wc24h_test
./wc24h_test
```

## Speicher

* **Einstellungen**: die Struktur `G` (`include/WordClockState.h`) wird
  byteweise im EEPROM-Bereich gespeichert. Neue Felder werden hinten
  angehängt und mit einer eigenen Kennung abgesichert (`extrasMagic`,
  `extras2Magic`); `ensureExtraSettings*()` in `src/WordClock.cpp` setzt
  sie bei älteren Speicherständen auf Standardwerte, ohne den Rest zu
  verlieren.
* **Bilder des Malbretts**: NVS-Namensraum `wc24h-pic`, Schlüssel
  `p<Platz>`, je 288 Farbnummern + Helligkeit. `cells` in der Nachricht
  `picture` sind zwei Hex-Ziffern je Feld, zeilenweise; leer bei freiem
  Platz. Nach Speichern/Löschen geht die Nachricht an alle Handys.
* **Sonnenstand**: dritter angehängter Block (`extras3Magic`) mit Tag-/
  Nacht-Helligkeit und Standort; `ensureExtraSettings3()`.
* **Bestenlisten**: ESP32-NVS (`Preferences`), Namensraum `wc24h-hs`,
  Schlüssel `g<Spiel-Nr>`, je fünf Einträge `{char name[9]; uint32_t score}`.

## WebSocket

Die Weboberfläche spricht über einen WebSocket auf Port 80 mit der Uhr.
Die Uhr hat acht Verbindungsplätze (`WEBSOCKETS_SERVER_CLIENT_MAX` in
`platformio.ini`) und schickt alle drei Sekunden einen Ping; wer zweimal
nicht antwortet, wird getrennt. Die Seite schließt ihre Verbindung, wenn sie
unsichtbar wird, und verbindet sich danach selbst neu.
Befehle sind Text: drei Ziffern Befehlsnummer, danach feste Felder (Zahlen
dreistellig mit führenden Nullen, Texte mit Leerzeichen aufgefüllt).

| Bereich | Bedeutung |
| --- | --- |
| 1–19 | Modus wechseln (`G.prog`) |
| 20–199 | Einstellung setzen (`G.conf`, wird im nächsten Durchlauf gespeichert) |
| 200–255 | Daten anfordern (Antwort als JSON an den anfragenden Client) |

### Modi

| Nr. | Modus | Nutzdaten |
| --- | --- | --- |
| 1 | Wordclock24 (Uhrzeit in Worten) | Farbe (siehe Weboberfläche) |
| 2–8, 10 | Sekunden, Laufschrift, Regenbogen, Farbwechsel, Farbe, Digitaluhr, Symbol, Übergang | wie Multilayout-Firmware |
| 11 | Spiel | Spiel-Nr. (3) + Spielername (bis 8 Zeichen) |
| 12 | Timer | Aktion (3): 1 Countdown, 2 Stoppuhr, 3 Pause/Weiter, 4 Beenden; Sekunden (5) |
| 13 | Nachricht | Durchläufe (3, 1–10) + Text (30) |
| 14 | Stimmung | Effekt (3), siehe unten |
| 15 | Wecker | nur intern; wird nicht von außen gestartet |

### Einstellungen (Auszug der neuen)

| Nr. | Einstellung | Nutzdaten |
| --- | --- | --- |
| 101 | Boot | Blitzen, Durchlauf, WLAN-Symbol, IP (je 3, 0/1) + Geschwindigkeit (3, 1–10) |
| 113 | Spieleingabe | Taste (3) + Spieler (3, 1/2) + Wert (5) |
| 114 | Digitaluhr | Bits (3): 0–1 Doppelpunkt (0 blinkend, 1 dauerhaft, 2 aus), 0x04 12 h, 0x08 ohne führende Null, 0x10 Sekundenbalken, 0x20 zentriert |
| 115 | Wecker | je Wecker (4×): Flags (3: 0x80 aktiv, Bit 0–6 = So–Sa), Stunde (3), Minute (3), Effekt (3) |
| 116 | Wortfarben | Bits (3): 0x01 Lern-Farben, 0x02 Silvester-Feuerwerk |
| 117 | Hinweis zeigen | Symbol (3, 999 = keins) + Durchläufe (3) + Text (30) |
| 118 | Helligkeit nach Sonnenstand | Tag % (3) + Nacht % (3) + Breite × 100 (6, mit Vorzeichen, z. B. `+05055`) + Länge × 100 (6) |
| 102 | Helligkeitsmodus | Modus (3: 0 Stundentabelle, 1 Lichtsensor, 2 Sonnenstand) + Min (3) + Max (3) + Spitze lx (4) |

### Anfragen

| Nr. | Antwort |
| --- | --- |
| 200 | Konfiguration, u. a. `digitalClockFlags`, `bootSpeed`, `wordColorFlags`, `alarms` (je `[flags, Stunde, Minute, Effekt]`), `sunDay`, `sunNight`, `sunLatitude`, `sunLongitude`, `sunrise`/`sunset` (heute, lokal „HH:MM“), `ledGain` |
| 208 | WordClock24h-Einstellungen, Symbolnamen |
| 209 | Live-Bild (siehe unten) |
| 210 + Spiel-Nr. (3) | Bestenliste |
| 211 | gespeicherte Bilder, je Platz eine Nachricht `picture` |

### Nachrichten der Uhr

```json
{"command": "frame", "prog": 1, "power": 1, "rssi": -58, "rgb": "000000ff8800…"}
```
288 Pixel × 6 Hex-Zeichen (RGB), zeilenweise von oben links, Spalten von
links — so, wie man die Front sieht. Die Werte sind die tatsächlich an die
LEDs geschickten (also gedimmt).

```json
{"command": "game", "game": 13, "score": 0, "over": false, "paused": false,
 "lives": 6, "level": 2, "score1": 0, "score2": 0, "turn": 1, "winner": 0,
 "text": "K A _ Z E", "text2": "R T", "rank": 0}
```
`text`/`text2` je nach Spiel: Tetris Reihen, Snake Länge, Simon Phase
(`watch`/`input`), Galgenmännchen Muster und falsche Buchstaben, Quiz
Fortschritt und letztes Ergebnis (`+13:25`/`-13:25`), Wörter finden
Zielwort und Fortschritt, Kopfrechnen Aufgabe und Ergebnis, Reaktionsduell
Phase (`wait`/`go`) und `foul`. `rank` > 0 bei neuem Rekord.

```json
{"command": "highscores", "game": 1, "list": [{"name": "LARS", "score": 1200}]}
{"command": "picture", "slot": 0, "bri": 80, "cells": "0f0f00…"}
{"command": "timer", "mode": 1, "running": true, "seconds": 287, "finished": false}
{"command": "alarm", "ringing": true}
```

## Spiele

| Nr. | Spiel | Zwei Spieler | Bestenliste |
| --- | --- | --- | --- |
| 1 | Tetris | | ja |
| 2 | Snake | | ja |
| 3 | Farben merken (Simon) | | ja |
| 4 | Flappy Vogel | | ja |
| 5 | Breakout | | ja |
| 6 | Weltraum-Invasion | | ja |
| 7 | Frosch über die Straße | | ja |
| 8 | Pong | ja | |
| 9 | Pong gegen die Uhr | (Spieler 2 kann übernehmen) | |
| 10 | Vier gewinnt | ja | |
| 11 | Vier gewinnt gegen die Uhr | | |
| 12 | Reaktionsduell | ja | |
| 13 | Galgenmännchen | | ja |
| 14 | Uhrzeit-Quiz (nutzt den eingestellten Anzeigemodus) | | ja |
| 15 | Wörter finden | | ja |
| 16 | Kopfrechnen | | ja |
| 20 | Malbrett | | |
| 21 | Würfel | | |
| 22 | Sanduhr | | |

### Tasten (Befehl 113)

| Taste | Bedeutung | Wert |
| --- | --- | --- |
| 1–4 | links, rechts, hoch, runter | |
| 5 | Aktion (fallen lassen, flattern, feuern, JETZT, würfeln, Sanduhr neu starten) | |
| 6 | Pause (nur Einzelspiele) | |
| 11–14 | Simon: rot, grün, blau, gelb | |
| 20 | Malbrett: Feld setzen | Feld × 64 + Farbe |
| 21 | Malbrett: alles füllen | Farbe |
| 22 | Malbrett: löschen | |
| 23 | Malbrett: Helligkeit des Bildes | 5–100 (%) |
| 24 | Malbrett: Bild speichern | Platz 0–7 |
| 25 | Malbrett: Bild laden | Platz 0–7 |
| 26 | Malbrett: Bild löschen | Platz 0–7 |
| 27 | Malbrett: Daumenkino | ms je Bild (100–5000), 0 = Stopp |
| 28 | Würfel: Anzahl | 1 oder 2 (würfelt gleich) |
| 29 | Sanduhr: Zeit | Sekunden (5–3600), startet |
| 30 | Antwort (Quiz `hh*100+mm`, Kopfrechnen Zahl) | Zahl |
| 31 | Feld antippen (Wörter finden) | Zeile × 18 + Spalte |
| 65–90 | Buchstabe A–Z (Galgenmännchen) | |

Farben (`wc24h::Palette`, `paletteHsv()`): 0 aus, 1 rot, 2 orange, 3 gelb,
4 grün, 5 cyan, 6 blau, 7 lila, 8 pink, 9 weiß, 10–14 gedimmte Töne. Für das
Malbrett zusätzlich 15–26 zwölf Farbtöne kräftig, 27–38 pastell, 39–50
dunkel, 51 warmweiß, 52 hellgrau, 53 dunkelgrau, 54 braun.

## Stimmungen und Wecker-Effekte

| Nr. | Effekt |
| --- | --- |
| 1 | Lavalampe |
| 2 | Kaminfeuer |
| 3 | Schneefall |
| 4 | Feuerwerk |
| 5 | Sternenhimmel |
| 6 | Regen |
| 7 | Sonnenaufgang (Wecker) |
| 8 | Blinken (Wecker) |

## Einblendungen

Zusätzlich zu den Typen der WordClock24h (1 Symbol, 2 Datum, 3 Temperatur,
4 Wettersymbol, 6 Laufschrift, 10 Temperatur in Ziffern):

| Typ | Anzeige | Felder |
| --- | --- | --- |
| 20 | Tage bis … | `month`/`day` = Zieldatum, `text` = Bezeichnung, Datumscode immer 0 |
| 21 | Mondphase | – |

## MQTT

Siehe [Home-Assistant.md](Home-Assistant.md#mqtt-themen).
