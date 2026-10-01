# WC24ESP — WordClock 24h auf dem BTF-LIGHTING SP803E

Firmware für eine **WordClock 24h** (Layout 16 × 18 von
[mikrocontroller.net](https://www.mikrocontroller.net/articles/WordClock_mit_WS2812)),
lauffähig auf dem **BTF-LIGHTING SP803E** WLED-LED-Controller (ESP32).

Basis ist die
[Multilayout-ESP-Wordclock](https://github.com/ESPWortuhr/Multilayout-ESP-Wordclock)
von ESPWortuhr, vorkonfiguriert für den SP803E und das 24h-Layout. Dazu kommen
die Funktionen der Original-Firmware
[wordclock24h](https://github.com/ukw100/wordclock24h) von Frank Meyer, soweit
sie ohne zusätzliche Hardware auf dem SP803E laufen: alle 27 Anzeigemodi,
Nachtschaltung, Einblendungen (Overlays), die Animationen und die
Tageslicht-Farbe — siehe [Funktionen der WordClock24h](#funktionen-der-wordclock24h).

## Was die Uhr anzeigt

Das 24h-Layout zeigt die Zeit minutengenau und ohne AM/PM aus, z. B.:

```
13:37 -> ES IST DREIZEHN UHR UND SIEBENUNDDREISSIG MINUTEN MITTAGS
00:01 -> ES IST NULL UHR UND EINE MINUTE MITTERNACHTS
```

Das ist die Phrasierung der Multilayout-Firmware. Voreingestellt ist der
Anzeigemodus **„Rhein/Ruhr (12)"** der Original-WordClock24h; insgesamt stehen
deren 27 Modi zur Wahl, z. B. für 13:45:

```
Ossi/Berlin (12-)  -> ES IST DREIVIERTEL ZWEI
Wessi (12-)        -> ES IST VIERTEL VOR ZWEI
hh mm (24+)        -> ES IST FÜNFUNDVIERZIG MINUTEN NACH DREIZEHN UHR
Countdown          -> ES IST FÜNFZEHN MINUTEN VOR ZWEI UHR NACH MITTAGS
```

Die Frontmatrix hat 16 Zeilen à 18 Buchstaben = **288 LEDs**. Zusätzliche
Minuten-LEDs in den Ecken gibt es nicht — die Minuten stehen als Wörter in der
Matrix.

Ohne Hardware lässt sich jede Uhrzeit vorab prüfen:

```sh
python3 tools/clock_type_simulator.py DE16x18 --time 13:37 --active-letters
python3 tools/clock_type_gui.py     # Browser-GUI auf http://127.0.0.1:8765/
```

## Hardware: SP803E

| Anschluss / Pin | GPIO | Verwendung in dieser Firmware |
| --- | --- | --- |
| Klemme `DATA` (SPI-Ausgang) | 16 | Datenleitung der LED-Matrix |
| Taster `KEY` | 0 | Ein/Aus (kurzer Druck) |
| Mikrofon (analog) | 36 | nicht genutzt |
| PWM-Ausgänge | 25 / 26 / 27 | nicht genutzt |

Der SP803E nimmt 5–24 V DC auf und gibt die Eingangsspannung an den
LED-Klemmen wieder aus. Für WS2812B/SK6812 (5 V) muss also ein **5-V-Netzteil**
verwendet werden; bei WS2815 entsprechend 12 V.

**Stromversorgung:** 288 WS2812B ziehen bei Weiß und voller Helligkeit
theoretisch rund 17 A. Die Firmware startet deshalb mit 60 % Helligkeit, und die
Matrix zeigt nie alle Buchstaben gleichzeitig. Trotzdem gilt: Netzteil
großzügig dimensionieren (mindestens 5 V / 10 A für eine 288er-Matrix) und die
LED-Matrix bei langen Strips zusätzlich direkt am Netzteil einspeisen, nicht
allein über die Controller-Klemmen.

### Verdrahtung

```
Netzteil 5 V ──┬── SP803E V+ / GND ──── Matrix V+ / GND (Klemme)
               └── ggf. zweite Einspeisung am Ende der Matrix
SP803E DATA (GPIO16) ─────────────────── DIN der ersten LED
```

GND von Controller und Matrix müssen verbunden sein — bei getrennter
Einspeisung also zusätzlich Masse durchverbinden.

## Bauen und Flashen

Voraussetzungen: [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/)
und Node.js (für die eingebettete Weboberfläche).

```sh
pio run                      # baut die Umgebung SP803E
pio run -t upload            # flasht über USB-C
pio device monitor           # serielle Ausgabe, 115200 Baud
```

Der SP803E hat eine Auto-Download-Schaltung: ein normales USB-C-Datenkabel
genügt, es muss kein Taster gedrückt werden (ein reines Ladekabel funktioniert
nicht). Der Upload löscht den Flash vollständig — die ab Werk installierte
WLED-Firmware und ihre Einstellungen sind danach weg.

Zurück zu WLED geht es jederzeit über den WLED-Installer bzw. `esptool`.

## Erste Inbetriebnahme

1. Nach dem ersten Start öffnet die Uhr einen WLAN-Accesspoint
   **`Connect_to_Wordclock`**. Damit verbinden und das eigene WLAN eintragen.
2. Danach ist die Weboberfläche unter der vergebenen IP bzw. unter
   `http://ESPWordclock.local` (mDNS) erreichbar.
3. Die Zeit kommt per NTP (`europe.pool.ntp.org`, Zeitzone Europe/Berlin) — der
   SP803E hat keine Echtzeituhr, ein RTC-Modul ist ohne Löten nicht anschließbar.

Alle weiteren Einstellungen (Farbe, Helligkeit, Effekte, MQTT, Home Assistant)
laufen über die Weboberfläche. Anzeigemodus, Farbanimation, Nachtschaltung und
Einblendungen der WordClock24h stehen unter *Anzeigeoptionen → WordClock24h*.

**Update von einer früheren Version dieser Firmware:** Die Einstellungen
werden beim ersten Start einmalig auf die Standardwerte zurückgesetzt (der
Speicherbereich ist für die neuen Funktionen gewachsen). WLAN-Zugangsdaten
bleiben erhalten.

## Funktionen der WordClock24h

Abgleich mit der Original-Firmware
[wordclock24h](https://github.com/ukw100/wordclock24h) (v3.1.5, STM32 +
ESP8266):

| Funktion des Originals | Auf dem SP803E |
| --- | --- |
| 27 Anzeigemodi (hh mm 12/24, Ossi/Berlin, Oesi, Rhein/Ruhr, Schwaben, Wessi, Tirol, Countdown, Jester, Temperatur) | portiert — Tabellen byte-identisch zum Original |
| Nachtschaltung: 8 Timer, Wochentagsbereiche, Ein/Aus | portiert |
| Animationen Explode, Teletype, Cube, Drop, Squeeze, Flicker | portiert |
| Animationen Fade, Roll, Snake, Matrix, Zufall | vorhanden (Multilayout-Firmware) |
| Farbanimation Tageslicht | portiert |
| Farbanimation Regenbogen | vorhanden (Übergang „Bunt", Regenbogen-Modi) |
| Einblendungen: Symbol, Datum, Temperatur in Worten / Ziffern, Laufschrift, Wettersymbol; Datumsbereiche inkl. Rosenmontag, Ostern, Advent | portiert, inkl. der Original-Symbole |
| „ES IST" nur zur vollen/halben Stunde | vorhanden (*Sprache → „Es ist" Anzeige*) |
| Helligkeit nach Uhrzeit, Zeit per NTP, Weboberfläche | vorhanden |
| Temperatur von DS18xx oder RTC | DS3231-RTC oder ersatzweise OpenWeatherMap (Außentemperatur) |
| DCF77-Funkuhr | nicht möglich: kein freier GPIO am SP803E |
| IR-Fernbedienung | nicht möglich: kein Empfänger, kein freier GPIO — Bedienung über Weboberfläche, MQTT/Home Assistant und den Taster |
| DS18xx-Sensor, LDR-Helligkeitsregelung | nicht möglich: keine freien Pins (GPIO 36 belegt das Mikrofon) |
| DFPlayer (Sprachausgabe, Gong, Wecker) | nicht möglich: keine Audio-Hardware |
| Ambilight (zweiter LED-Streifen) | nicht portiert — der SP803E hat nur einen Datenausgang |
| TFT-Displays, Touch, Tetris/Snake-Spiel | nicht übertragbar |
| Wetter als Laufschrift, Wettervorhersage, Android-App (UDP) | nicht portiert |

Hinweise zur Portierung:

* Den Modus „Temperatur" und die Temperatur-Einblendungen speist auf dem
  SP803E entweder ein angeschlossenes DS3231-RTC-Modul (Raumtemperatur) oder
  OpenWeatherMap (Außentemperatur; Stadt-ID und API-Key wie beim
  Wetter-Layout eintragen). Ohne Quelle zeigt der Modus weiter die Uhrzeit.
  In Worten darstellbar sind 10,0–39,5 °C, außerhalb wird die Temperatur als
  Laufschrift gezeigt.
* Die Original-Firmware berechnet in Schaltjahren Termine, die über den
  Februar zurückrechnen, einen Tag zu früh (Rosenmontag 2024 am 11. statt
  12. Februar). Die Portierung rechnet korrekt.
* Die Tageslicht-Farbe überschreibt laufend die Vordergrundfarbe; zum
  Zurückkehren zur eigenen Farbe die Farbanimation ausschalten.

## Voreinstellungen dieser Variante

Gegenüber dem Upstream-Projekt sind in `include/Config.h` und `platformio.ini`
folgende Werte gesetzt:

| Einstellung | Wert | Grund |
| --- | --- | --- |
| `LED_PIN` | 16 | Datenausgang des SP803E |
| `POWER_BUTTON_PIN` | 0 | On-Board-Taster `KEY` |
| `DEFAULT_LAYOUT` | `Ger16x18` | WordClock24h, 16 × 18 |
| Minutenanzeige | `MINUTE_Off` | Minuten stehen im 24h-Layout als Wörter |
| `DEFAULT_BRIGHTNESS` | 60 | Strombedarf von 288 LEDs |
| `SERNR` | 2442 | erzwingt saubere EEPROM-Initialisierung |
| `DEFAULT_WC24H_DISPLAY_MODE` | 13 | „Rhein/Ruhr (12)" der WordClock24h |
| PlatformIO-Env | `SP803E` (esp32dev) | einzige Zielhardware |
| Partitionierung | `min_spiffs.csv` | Platz für Firmware inkl. Weboberfläche |

Falls die Matrix anders verdrahtet ist als angenommen (Serpentine, Startecke,
Laufrichtung), lässt sich das ohne Neubau in der Weboberfläche unter
*Einstellungen* umstellen — `MEANDER_ROWS`, `MIRROR_FRONT_*` und
`FLIP_HORIZONTAL_VERTICAL` in `Config.h` sind die entsprechenden Startwerte.
Ebenso die Farbreihenfolge des Strips (`DEFAULT_LEDTYPE`, Standard `Grb`).

## Bekannte Einschränkungen auf dieser Hardware

* **Automatische Helligkeit ist nicht nutzbar.** Der LDR-Eingang der Firmware
  liest `A0` — auf dem ESP32 ist das GPIO 36, und dort sitzt beim SP803E das
  On-Board-Mikrofon. Ein BH1750 bräuchte freie I²C-Pins, die nach außen nicht
  herausgeführt sind. Die Helligkeit lässt sich stattdessen per Uhrzeit-Profil
  in der Weboberfläche steuern.
* **Taster `KEY` liegt auf GPIO 0**, dem Boot-Strapping-Pin. Beim Einschalten
  und beim Flashen darf er nicht gedrückt sein.
* **Mikrofon und PWM-Ausgänge werden nicht verwendet** — musikreaktive Effekte
  und Einfarb-/CCT-Strips sind eine WLED-Funktion, keine der Wordclock-Firmware.

## Lizenz und Herkunft

Die Firmware als Ganzes steht unter der **GNU GPL Version 2 oder neuer**
([LICENSE.GPL-2.0](LICENSE.GPL-2.0)), weil sie Teile der
[wordclock24h](https://github.com/ukw100/wordclock24h)-Firmware von Frank Meyer
enthält: die Dateien unter `include/WC24h/` mit GPL-Kopf (Anzeigetabellen,
Symbole, Nachtschaltung, Einblendungen) und die portierten Animationen in
`include/TransitionTypes/Transition.hpp`.

Der übrige Code stammt aus
[ESPWortuhr/Multilayout-ESP-Wordclock](https://github.com/ESPWortuhr/Multilayout-ESP-Wordclock)
(Stand 4.4.1) und steht zusätzlich unter BSD-3-Clause ([LICENSE](LICENSE));
dessen Copyright-Hinweise gelten weiter. Das Frontlayout geht auf das Projekt
[WordClock 24h](https://www.mikrocontroller.net/articles/WordClock_mit_WS2812#Word_Clock_24h)
von mikrocontroller.net zurück. Nicht übernommen wurden die Bilder, 3D-Modelle,
SVG-Frontvorlagen und die PDF-Anleitung der Multilayout-Firmware — die liegen
weiterhin im Upstream-Repository.
