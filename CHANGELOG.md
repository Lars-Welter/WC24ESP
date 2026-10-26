# Änderungen

Alle Änderungen dieser SP803E-Variante gegenüber der
[Multilayout-ESP-Wordclock](https://github.com/ESPWortuhr/Multilayout-ESP-Wordclock) 4.4.1.

## Wordclock24, Malbrett, Korrekturen

* Name in Weboberfläche, App-Symbol und Home Assistant (Gerätemodell):
  **Wordclock24** statt „Wortuhr".
* **Malbrett**: 40 Farben (12 Farbtöne kräftig, pastell und dunkel,
  Warmweiß, Grautöne, Braun) und Radierer, Regler für die Helligkeit des
  Bildes. Die Palette ist zentral in `wc24h::paletteHsv()` definiert.
* Korrektur: Farbänderungen (z. B. Hintergrundfarbe) oder ein Tippen nach
  einem Spiel starteten wieder Tetris. Die Seite merkte sich den Spielmodus
  und schickte ihn mit der Farbe; die Uhr las die Farbposition als
  Spielnummer. Beides ist abgefangen.
* Korrektur: Die Navigationsleiste unten rutschte auf dem iPhone SE
  (Safari) mit dem Inhalt nach oben.
* Foto des SP803E und aktuelle Bilder der Startseite in `docs/images/`.

## Home Assistant und Dokumentation

* **Hinweise aus Home Assistant**: neue Entität *Notification* (`notify`).
  `notify.send_message` mit `"[Waschmaschine] WASCHMASCHINE FERTIG"` zeigt
  erst das Symbol, dann den Text dreimal als Laufschrift, danach wieder die
  Uhrzeit. Per MQTT-JSON zusätzlich `notify_repeat`. Umlaute werden
  umgewandelt; während Spielen, Timer, Stimmung und Wecker wird nicht
  unterbrochen.
* Neue Symbole **Waschmaschine**, **Klingel**, **Post**; Symbole lassen sich
  per Name oder deutschem Namen angeben.
* *Wecker & Extras → Hinweise*: Hinweis ohne Home Assistant ausprobieren.
* Die Weboberfläche wird mit `Cache-Control: no-cache` ausgeliefert, damit
  nach einem Update sofort die neue Seite erscheint.
* Dokumentation in `docs/`: Bedienung, Home Assistant mit fertigen
  Automationen, Umbau einer Original-WordClock24h, Protokoll-Referenz.

## Spiele, Wecker, Startseite

* 15 weitere Spiele: Farben merken, Flappy Vogel, Breakout,
  Weltraum-Invasion, Frosch über die Straße, Pong und Vier gewinnt (zu zweit
  oder gegen die Uhr), Reaktionsduell, Galgenmännchen, Uhrzeit-Quiz, Wörter
  finden, Kopfrechnen, Malbrett. Bestenliste je Spiel im Flash.
* Nachricht an die Uhr (Laufschrift vom Handy), Stimmungen (Lavalampe,
  Kaminfeuer, Schnee, Feuerwerk, Sternenhimmel, Regen), vier Lichtwecker,
  Lern-Farben, Silvester-Feuerwerk, Einblendungen „Tage bis …" und
  Mondphase.
* Weboberfläche: Startseite mit Live-Vorschau und Statuszeile,
  Schnellzugriff, eigene Bedienelemente je Spiel, Navigationsleiste auf dem
  Handy, Dunkelmodus, Experteneinstellungen ein-/ausblendbar, Vibration.
* Vorübergehende Modi (Spiele, Timer, Nachricht, Stimmung, Wecker) werden
  nach einem Neustart nicht fortgesetzt.

## Tetris, Snake, Timer, Digitaluhr, Boot

* Tetris und Snake, Countdown und Stoppuhr mit eigener Seite
  *Spiele & Timer*.
* Digitaluhr: Doppelpunkt blinkend/dauerhaft/aus, 12/24 Stunden, führende
  Null, Sekundenbalken, zentrierte Ziffern.
* Boot-Animation mit einstellbarer Geschwindigkeit.
* Korrektur: Links/rechts in Tetris und Snake waren vertauscht.

## LED-Ansteuerung

* Die LED-Kette der WordClock24h beginnt mit einer Status-LED unter der
  Matrix; `STATUS_LED_COUNT` verschiebt alle Buchstaben um eins.
* LEDs werden über I2S mit DMA statt RMT angesteuert: bei 289 LEDs störten
  WLAN-Interrupts die Übertragung, einzelne Buchstaben blitzten in falschen
  Farben auf.

## Portierung WordClock24h

* Firmware für den BTF-LIGHTING SP803E (ESP32), Layout 16 × 18.
* Alle 27 Anzeigemodi, Nachtschaltung, Einblendungen, Animationen und
  Tageslicht-Farbe der Original-Firmware wordclock24h.
