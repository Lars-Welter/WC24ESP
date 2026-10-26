# Home Assistant

Die Uhr meldet sich per **MQTT Discovery** als Gerät in Home Assistant an
(Home Assistant 2024.11 oder neuer). Danach lässt sie sich wie eine Lampe
steuern, und Automationen können **Hinweise** wie „Waschmaschine fertig"
auf der Uhr anzeigen.

## Einrichten

1. In Home Assistant die **MQTT-Integration** mit einem Broker einrichten
   (z. B. das Add-on *Mosquitto broker*).
2. In der Weboberfläche der Uhr unter *Einstellungen → Darstellung* die
   **Experteneinstellungen** einschalten. Dann erscheint der Menüpunkt
   **Smart Home**.
3. Unter *Smart Home* MQTT aktivieren und Broker-Adresse, Port (meist 1883),
   Benutzer, Passwort und das **Topic** eintragen (Standard `ESPWordclock`),
   speichern.
4. Die Uhr erscheint unter *Einstellungen → Geräte & Dienste → MQTT* als Gerät.
   Falls nicht: in der Weboberfläche *Smart Home → Sende HA Discovery*.

Im Folgenden steht `ESPWordclock` für das eingetragene Topic.

## Entitäten

| Entität | Typ | Zweck |
| --- | --- | --- |
| Word Clock | `light` | Ein/Aus, Helligkeit, Farbe, Effekt (Wortuhr, Digitaluhr, Laufschrift, Regenbogen …) |
| **Notification** | `notify` | Hinweis mit Symbol: erst das Symbol, dann der Text dreimal als Laufschrift, danach wieder die Uhrzeit |
| Message | `notify` | setzt die Laufschrift und schaltet dauerhaft in den Laufschrift-Modus |
| Scrolling Text | `text` | Text der Laufschrift |
| Effect Speed, Transition … | `number` / `select` | Geschwindigkeit, Übergangsanimation |
| Auto Brightness | `switch` | automatische Helligkeit (auf dem SP803E ohne Sensor wirkungslos) |
| WiFi Signal, IP, Uptime … | `sensor` | Diagnose |

Für Hinweise ist **Notification** die richtige Wahl: Die Uhr zeigt sie
einmal (mit Wiederholungen) und kehrt dann selbst zur Uhrzeit zurück. Läuft
gerade ein Spiel, ein Timer, eine Stimmung oder der Wecker, wird der Hinweis
übersprungen, damit nichts unterbrochen wird.

## Hinweis senden

### Über die notify-Entität

Ein Symbol steht in eckigen Klammern vor dem Text:

```yaml
action: notify.send_message
target:
  entity_id: notify.espwordclock_notification
data:
  message: "[Waschmaschine] WASCHMASCHINE FERTIG"
```

### Entitäts-ID finden

Die ID setzt sich aus dem Gerätenamen (in der Weboberfläche *Smart Home →
Client-ID*) und „notification" zusammen, z. B. `notify.espwordclock_notification`
bei der Client-ID `ESPWordclock`. Am sichersten: in Home Assistant das Gerät
öffnen und bei der Entität *Notification* nachsehen.

### Über MQTT direkt (mit Wiederholungen)

```yaml
action: mqtt.publish
data:
  topic: ESPWordclock/cmd
  payload: >
    {"notify_icon": "Waschmaschine", "notify_text": "WASCHMASCHINE FERTIG",
     "notify_repeat": 5}
```

| Feld | Bedeutung |
| --- | --- |
| `notify_icon` | Symbol als Name oder Nummer (siehe unten), weglassen für nur Text |
| `notify_text` | Text, bis 30 Zeichen; Umlaute werden zu AE, OE, UE |
| `notify_repeat` | wie oft der Text durchläuft, 1–10, Standard 3 |

### Symbole

| Nr. | Name | Deutsche Namen |
| --- | --- | --- |
| 0 | `Heart` | Herz |
| 1 | `Xmas Tree` | Tannenbaum |
| 2 | `Fireworks` | Feuerwerk |
| 3–6 | `Advent1` … `Advent4` | |
| 7 | `Blue Star` | Stern |
| 8 | `Washer` | Waschmaschine, Waesche |
| 9 | `Bell` | Klingel, Tuer, Tuerklingel |
| 10 | `Mail` | Post, Brief |

Groß- und Kleinschreibung spielt keine Rolle. Ausprobieren lässt sich das ohne
Home Assistant in der Weboberfläche unter *Wecker & Extras → Hinweise*.

## Beispiel: Waschmaschine fertig

Die meisten Waschmaschinen hängen an einer **Steckdose mit Strommessung**
(Shelly Plug, Tasmota, Zigbee-Steckdose …). Die Maschine gilt als fertig, wenn
sie vorher gewaschen hat und der Verbrauch danach einige Minuten lang unter
wenige Watt fällt. Ein Hilfsschalter merkt sich, dass gewaschen wird – so
löst nicht jedes kurze Abschalten im Standby einen Hinweis aus.

Ersetze `sensor.waschmaschine_leistung` durch deinen Leistungssensor und
`notify.espwordclock_notification` durch die ID deiner Uhr (siehe
[Entitäts-ID finden](#entitäts-id-finden)).

1. *Einstellungen → Geräte & Dienste → Helfer → Helfer erstellen → Schalter*
   mit dem Namen „Waschmaschine läuft" anlegen
   (`input_boolean.waschmaschine_laeuft`).
2. Erste Automation – Waschen hat begonnen:

```yaml
alias: Waschmaschine läuft
triggers:
  - trigger: numeric_state
    entity_id: sensor.waschmaschine_leistung
    above: 10
    for:
      minutes: 2
actions:
  - action: input_boolean.turn_on
    target:
      entity_id: input_boolean.waschmaschine_laeuft
```

3. Zweite Automation – fertig, Hinweis auf der Uhr:

```yaml
alias: Wortuhr – Waschmaschine fertig
triggers:
  - trigger: numeric_state
    entity_id: sensor.waschmaschine_leistung
    below: 3
    for:
      minutes: 3
conditions:
  - condition: state
    entity_id: input_boolean.waschmaschine_laeuft
    state: "on"
actions:
  - action: input_boolean.turn_off
    target:
      entity_id: input_boolean.waschmaschine_laeuft
  - action: notify.send_message
    target:
      entity_id: notify.espwordclock_notification
    data:
      message: "[Waschmaschine] WASCHMASCHINE FERTIG"
```

Die Automationen lassen sich in Home Assistant unter *Automationen → Neue
Automation → ⋮ → In YAML bearbeiten* einfügen.

Hat die Waschmaschine selbst eine Home-Assistant-Integration (Miele, Bosch
Home Connect, LG ThinQ …), reicht als Auslöser deren Programmstatus, z. B.:

```yaml
triggers:
  - trigger: state
    entity_id: sensor.waschmaschine_programmstatus
    to: "finished"
```

### Wiederholen, bis jemand reagiert

Soll der Hinweis alle 10 Minuten wiederkommen, bis die Wäsche ausgeräumt ist
(z. B. die Tür der Maschine geöffnet wurde):

```yaml
actions:
  - repeat:
      until:
        - condition: state
          entity_id: binary_sensor.waschmaschine_tuer
          state: "on"
      sequence:
        - action: notify.send_message
          target:
            entity_id: notify.espwordclock_notification
          data:
            message: "[Waschmaschine] WAESCHE AUSRAEUMEN"
        - delay:
            minutes: 10
```

## Weitere Beispiele

**Türklingel:**

```yaml
alias: Wortuhr – Es klingelt
triggers:
  - trigger: state
    entity_id: binary_sensor.tuerklingel
    to: "on"
actions:
  - action: notify.send_message
    target:
      entity_id: notify.espwordclock_notification
    data:
      message: "[Klingel] ES KLINGELT"
```

**Post im Briefkasten** (Kontaktsensor an der Klappe):

```yaml
alias: Wortuhr – Post ist da
triggers:
  - trigger: state
    entity_id: binary_sensor.briefkasten
    to: "on"
actions:
  - action: notify.send_message
    target:
      entity_id: notify.espwordclock_notification
    data:
      message: "[Post] POST IST DA"
```

**Nur Text**, z. B. vom Handy über ein Skript:

```yaml
action: notify.send_message
target:
  entity_id: notify.espwordclock_notification
data:
  message: "ESSEN IST FERTIG"
```

## MQTT-Themen

| Thema | Richtung | Inhalt |
| --- | --- | --- |
| `<topic>/cmd` | an die Uhr | JSON: `state`, `brightness`, `color`, `effect`, `scrolling_text`, `notify_icon`, `notify_text`, `notify_repeat` |
| `<topic>/notify/set` | an die Uhr | Klartext, optional `[Symbol]` vorneweg |
| `<topic>/scrolltext/set` | an die Uhr | JSON `{"scrolling_text": "..."}` |
| `<topic>/effect_speed/set` | an die Uhr | Zahl 1–10 |
| `<topic>/transition_*/set` | an die Uhr | Übergangsanimation (Auswahl / Zahl) |
| `<topic>/status` | von der Uhr | Zustand als JSON |
| `<topic>/diagnostics` | von der Uhr | Diagnose als JSON |
| `<topic>/availability` | von der Uhr | `online` / `offline` |
