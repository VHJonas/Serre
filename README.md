# SmartGreenhouse

SmartGreenhouse is een IoT-gebaseerd systeem voor monitoring en regeling van een kas. Dit project maakt gebruik van een ESP32 microcontroller om verschillende sensoren en actuatoren aan te sturen, waardoor automatische regeling van de omgevingscondities binnen een kas mogelijk is.

## Inhoudsopgave

- [Kenmerken](#kenmerken)
- [Benodigdheden](#benodigdheden)
- [Software Vereisten](#software-vereisten)
- [Installatie](#installatie)
- [Gebruik](#gebruik)
- [Schematisch Overzicht](#schematisch-overzicht)
- [Bijdragen](#bijdragen)
- [Licentie](#licentie)

## Kenmerken

- **WiFi Connectiviteit:** Verbindt met een WiFi-netwerk voor remote monitoring en controle.
- **MQTT Communicatie:** Publiceert sensorgegevens naar een MQTT-server.
- **RFID-gebaseerde Plant Selectie:** Gebruikt RFID om verschillende plantensoorten te selecteren en parameters in te stellen.
- **Omgevingsmonitoring:** Meet temperatuur, luchtvochtigheid, bodemvochtigheid, lichtintensiteit en afstand.
- **Automatische Regeling:** Regelt verwarming, verlichting en bewatering op basis van sensorgegevens.

## Benodigdheden

- ESP32 Development Board
- DHT11 Vochtigheid- en Temperatuursensor
- DS18B20 Waterdichte Temperatuursensor
- Bodemvochtigheidssensor
- Ultrasone Sensor (HC-SR04)
- LDR (Lichtgevoelige Weerstand)
- MFRC522 RFID Lezer en Tags
- 20x4 I2C LCD Display
- Relay Module voor het aansturen van actuatoren (verwarming, pomp, LED-strip)
- Jumperdraden en breadboard

## Software Vereisten

- Arduino IDE met ESP32 board support
- PubSubClient library voor MQTT
- MFRC522 library voor RFID
- DHT sensor library voor DHT11 sensor
- OneWire library voor DS18B20 sensor
- DallasTemperature library voor DS18B20 sensor
- LiquidCrystal I2C library voor LCD

## Installatie

1. **Installeer Arduino IDE:**
   - Download en installeer de Arduino IDE vanaf de [officiële website](https://www.arduino.cc/en/Main/Software).
   - Voeg ESP32 board support toe aan de Arduino IDE. Volg de instructies [hier](https://github.com/espressif/arduino-esp32#installation-instructions).

2. **Installeer Vereiste Bibliotheken:**
   - Open de Arduino IDE en ga naar `Sketch -> Include Library -> Manage Libraries...`
   - Zoek en installeer de volgende bibliotheken:
     - `PubSubClient` by Nick O'Leary
     - `MFRC522` by GithubCommunity
     - `DHT sensor library` by Adafruit
     - `OneWire` by Paul Stoffregen
     - `DallasTemperature` by Miles Burton
     - `LiquidCrystal I2C` by Frank de Brabander

3. **Open het Project:**
   - Download en open het `SmartGreenhouse.ino` bestand in de Arduino IDE.

4. **Configureer WiFi en MQTT:**
   - Update in het `SmartGreenhouse.ino` bestand de volgende regels met jouw WiFi en MQTT server details:
     ```cpp
     const char* ssid = "jouw-ssid";
     const char* password = "jouw-wachtwoord";
     const char* mqttServer = "jouw-mqtt-server-ip";
     const int mqttPort = jouw-mqtt-poort;
     const char* mqttUser = "jouw-mqtt-gebruikersnaam";
     const char* mqttPassword = "jouw-mqtt-wachtwoord";
     ```

5. **Upload de Code:**
   - Verbind de ESP32 met je computer via USB.
   - Selecteer het juiste board en poort in het `Tools` menu.
   - Klik op de uploadknop om de code naar de ESP32 te flashen.

## Gebruik

1. **Starten:**
   - Verbind alle sensoren en actuatoren volgens het schema.
   - Zet de ESP32 aan.

2. **Monitoren en Controleren:**
   - De ESP32 verbindt met het WiFi-netwerk en de MQTT-server.
   - Sensorgegevens worden gepubliceerd naar de MQTT topics die in de code zijn ingesteld.
   - Gebruik een MQTT-client om je te abonneren op deze topics en de gegevens te monitoren.
   - Het systeem regelt automatisch de verwarming, verlichting en bewatering op basis van de sensormetingen.

## Schematisch Overzicht

## Schematisch Overzicht

Hier is het elektrische schema van de SmartGreenhouse:

![Elektrisch Schema](url-naar-afbeelding)

## Bijdragen

Bijdragen zijn welkom! Fork deze repository en dien een pull request in voor verbeteringen of bugfixes.

## Licentie

Dit project is gelicentieerd onder de MIT Licentie - zie het [LICENSE](LICENSE) bestand voor details.
