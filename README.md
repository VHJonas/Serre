# SmartGreenhouse

SmartGreenhouse is een IoT-gebaseerd systeem voor monitoring en regeling van een kas. Dit project maakt gebruik van een ESP32 microcontroller om verschillende sensoren en actuatoren aan te sturen, waardoor automatische regeling van de omgevingscondities binnen een kas mogelijk is.

## Inhoudsopgave

- [Kenmerken](#kenmerken)
- [Benodigdheden](#benodigdheden)
- [Software Vereisten ESP32](#software-vereisten-ESP32)
- [Installatie ESP32](#installatie-ESP32)
- [Software Vereisten MQTT](#software-vereisten-MQTT)
- [Installatie MQTT](#Installatie-MQTT)
- [Configuratie MQTT](#Configuratie-MQTT)
- [Configuratie InfluxDB](#Configuratie-InfluxDB)
- [Gebruik](#gebruik)
- [Schematisch Overzicht](#schematisch-overzicht)
- [PCB design](#PCB-design)
- [Bijdragen](#bijdragen)
- [Volledige documentatie](#Volledige-documentatie)
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
- Raspberry Pi

## Software Vereisten ESP32

- Arduino IDE met ESP32 board support
- PubSubClient library voor MQTT
- MFRC522 library voor RFID
- DHT sensor library voor DHT11 sensor
- OneWire library voor DS18B20 sensor
- DallasTemperature library voor DS18B20 sensor
- LiquidCrystal I2C library voor LCD

## Installatie ESP32

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
   - Download en open het `Volle_Code_Met_MQTT` bestand in de Arduino IDE.

4. **Configureer WiFi en MQTT:**
   - Update in het `Volle_Code_Met_MQTT` bestand de volgende regels met jouw WiFi en MQTT server details:
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

## Software vereisten MQTT
   - Python 3.x
   - Paho MQTT library
   - InfluxDB Python library

## Installatie MQTT
1. Zorg ervoor dat Python is geïnstalleerd op je systeem
2. Installeer de paho MQTT Library met behulp van pip:
   - 'sudo pip install paho-mqtt'
3. Installeer de InfluxDB Python Library met behulp van pip:
   - 'sudo pip install influxdb'
   - 'sudo pip install influxdb-client'
  

## Configuratie MQTT

Pas de volgende variabelen aan in het script volgens jouw configuratie:
![MQTT Code](https://github.com/VHJonas/Serre/blob/main/MQTT_Code)

- `mqtt_broker`: IP-adres van de MQTT-broker (bijvoorbeeld de Raspberry Pi).
- `mqtt_port`: Poortnummer van de MQTT-broker (standaard is 1883).
- `mqtt_user`: Gebruikersnaam voor authenticatie bij de MQTT-broker.
- `mqtt_password`: Wachtwoord voor authenticatie bij de MQTT-broker.
- Topicnamen voor de sensoren: Pas deze aan indien nodig.

  
## Configuratie InfluxDB

Pas de volgende variabelen aan in het script volgens jouw configuratie:
![Bridge Code](https://github.com/VHJonas/Serre/blob/main/Bridge%20Code)

- `INFLUXDB_ADDRESS`: IP-adres van de InfluxDB-server.
- `INFLUXDB_USER`: Gebruikersnaam voor authenticatie bij de InfluxDB-server.
- `INFLUXDB_PASSWORD`: Wachtwoord voor authenticatie bij de InfluxDB-server.
- `INFLUXDB_DATABASE`: Naam van de InfluxDB-database.
- `MQTT_ADDRESS`: IP-adres van de MQTT-broker.
- `MQTT_USER`: Gebruikersnaam voor authenticatie bij de MQTT-broker.
- `MQTT_PASSWORD`: Wachtwoord voor authenticatie bij de MQTT-broker.
- `MQTT_TOPIC`: Onderwerp waarop de bridge zich abonneert om MQTT-berichten te ontvangen.
- `MQTT_REGEX`: Reguliere expressie om MQTT-onderwerpen te matchen.
- `MQTT_CLIENT_ID`: ID van de MQTT-client.
     
## Gebruik

1. **Starten:**
   - Verbind alle sensoren en actuatoren volgens het schema.
   - Zet de ESP32 aan.
   - Zet de Raspberry Pi aan.

2. **Monitoren en Controleren:**
   - De ESP32 verbindt met het WiFi-netwerk en de MQTT-server.
   - Sensorgegevens worden gepubliceerd naar de MQTT topics die in de code zijn ingesteld.
   - Gebruik een MQTT-client om je te abonneren op deze topics en de gegevens te monitoren.
   - Het systeem regelt automatisch de verwarming, verlichting en bewatering op basis van de sensormetingen.
  


## Schematisch Overzicht

Hier is het elektrische schema van de SmartGreenhouse:

![Elektrisch Schema](https://github.com/VHJonas/Serre/blob/main/Elektrisch_schema.png)

## PCB-design

Hier is het PCB-desgin van de SmartGreenhouse:

![PCB-design](https://github.com/VHJonas/Serre/blob/main/PCB_schema.png)

En dit is de Gerber-Zip File:

![CamOutputs](https://github.com/VHJonas/Serre/blob/main/CAMOutputs.zip)

## Volledige documentatie

In de volledige documentatie (PDF) kan je alle details terugvinden van A tot Z hoe ik mijn project heb verwezenlijkt.

![Volledige-documentatie](https://github.com/VHJonas/Serre/blob/main/Smart-Green-House%20(1).pdf)

## Bijdragen

Bijdragen zijn welkom! Fork deze repository en dien een pull request in voor verbeteringen of bugfixes.

## Licentie

Dit project is gelicentieerd onder de MIT Licentie - zie het [LICENSE](LICENSE) bestand voor details.
