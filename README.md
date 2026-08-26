# ESP32 Kniffel

Ein digitaler Kniffel-Würfel auf Basis eines ESP32 mit 480 × 320 TFT-Touchdisplay.

Das Projekt simuliert fünf Würfel und ermöglicht das Spielen nach den bekannten Kniffel-Regeln.

## Funktionen

- 5 digitale Würfel
- Würfelanimation
- Würfelgeräusch über Buzzer
- Touchbedienung
- einzelne Würfel können gehalten werden
- maximal 3 Würfe pro Runde
- automatische Erkennung verschiedener Würfelkombinationen
- 3D-Darstellung der Würfel
- RESET-Funktion

## Erkannte Kombinationen

Das Programm erkennt:

- Kniffel
- Kleine Straße
- Große Straße
- Full House
- Vier Gleiche
- Drei Gleiche
- Zwei Paare
- Ein Paar
- Keine Kombination

## Hardware

- ESP32
- ILI9488 TFT Display, 480 × 320 Pixel
- resistiver Touch
- Buzzer

## Anschluss

| Funktion | ESP32 GPIO |
|---|---:|
| TFT MISO | 19 |
| TFT MOSI | 23 |
| TFT SCLK | 18 |
| TFT CS | 15 |
| TFT DC | 2 |
| TFT RST | 4 |
| Touch CS | 21 |
| Buzzer | 13 |

## Software

Das Projekt wurde mit der Arduino IDE entwickelt.

Benötigt wird:

- ESP32 Board-Unterstützung
- TFT_eSPI

### ESP32-Version

Der aktuelle Stand wurde mit der ESP32-Bibliothek

**Espressif ESP32 2.10.14**

entwickelt.

## TFT_eSPI

Für das Display wird ein ILI9488 verwendet.

Die erforderliche TFT_eSPI-Konfiguration ist im Projekt dokumentiert.

## Bedienung

Nach dem Einschalten werden fünf Würfel angezeigt.

Mit **START** werden die Würfel geworfen.

Einzelne Würfel können durch Antippen gehalten werden. Diese Würfel werden bei den folgenden Würfen nicht verändert.

Nach spätestens drei Würfen kann die Runde mit **RESET** neu gestartet werden.

Mit **FERTIG** kann eine Runde vorzeitig beendet werden.

## Projektstatus

Aktuelle Version: V3

Stand: 30.12.2025

## Bilder

Fotos und weitere Informationen zum Aufbau werden später ergänzt.

## Lizenz

Die Lizenz für dieses Projekt wird noch festgelegt.
