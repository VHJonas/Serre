#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

// WiFi en MQTT server instellingen
const char* ssid = "embed";
const char* password = "weareincontrol";
const char* mqttServer = "192.168.1.34";
const int mqttPort = 1883;
const char* mqttUser = "SERRE";
const char* mqttPassword = "SERRE";
const char* clientID = "ESP32";

// WiFi en MQTT client initialisatie
WiFiClient espClient;
PubSubClient client(espClient);

// RFID instellingen
#define SS_PIN 5
#define RST_PIN 14
MFRC522 mfrc522(SS_PIN, RST_PIN);

// DHT11 sensor instellingen
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float hum;
float temp;

// DS18B20 sensor instellingen
const int oneWireBus = 15;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
float bodem_temp;

// Bodemvochtsensor instelling
#define soil_moisture_pin 39
float grondwater;

// Ultrasone sensor instellingen
const int trigPin = 32;
const int echoPin = 35;
#define SOUND_SPEED 0.034
long tijd;
float afstandCM;
String level;

// LDR sensor instelling
#define LDRPIN 13
int ldr;

// Pin instellingen voor verwarming en ventilatie
#define verwarming 25
#define ventilatie 16

// LCD initialisatie
LiquidCrystal_I2C lcd(0x27, 20, 4);
String kaart;

// Pin instellingen voor pomp en LED-strip
#define pomp 4
#define strip 27

// Variabelen voor grenswaarden en tijd
int max_grondwater = 200;
int min_grondwater = 0;
int max_temp = 0;
int min_temp = 0;
int max_lichttijd = 0;

// Tijd instellingen
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void setup() {
  Serial.begin(9600); // Initialiseer seriële communicatie
  dht.begin(); // Start de DHT sensor
  SPI.begin(); // Start SPI communicatie
  mfrc522.PCD_Init(); // Initialiseer RFID
  sensors.begin(); // Start de DS18B20 sensor

  // Pin configuraties
  pinMode(DHTPIN, INPUT);
  pinMode(soil_moisture_pin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LDRPIN, INPUT);
  pinMode(verwarming, OUTPUT);
  pinMode(strip, OUTPUT);
  pinMode(pomp, OUTPUT);

  // Initialiseer LCD
  lcd.init();
  lcd.backlight();

  // Verbinden met WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Verbinden met WiFi..");
  }
  Serial.println("Verbonden met het WiFi-netwerk");

  // Instellen van de MQTT server
  client.setServer(mqttServer, mqttPort);

  // Tijd synchroniseren via NTP
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
}

void loop() {
  // Uitvoeren van sensormetingen en regelingen
  RFID();
  DHT();
  DS18B20();
  Vochtsensor();
  UltraSone();
  LDR();
  temp_regeling();
  licht_regeling();
  water_regeling();
  LCD();
  publish();

  delay(1000); // Wacht een seconde

  // Verbind met de MQTT server indien niet verbonden
  client.loop();
  while (!client.connected()) {
    Serial.println("Verbinden met MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Verbonden");
    } else {
      Serial.print("Verbinding mislukt, status: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void RFID() {
  // Lees RFID-kaart
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID tag: ");
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    content.toUpperCase();
    lcd.clear();

    // Check welke kaart is gescand en stel de variabelen in
    if (content.substring(1) == "53 8A F6 27") {
      kaart = "Waterkers";
      max_grondwater = 30;
      min_grondwater = 5;
      max_temp = 22;
      min_temp = 20;
      max_lichttijd = 18000000; // 5 uur
    }
    if (content.substring(1) == "63 6C 58 14") {
      kaart = "Aardbei";
      max_grondwater = 200;
      min_grondwater = 50;
      max_temp = 24;
      min_temp = 20;
      max_lichttijd = 32400000; // 9 uur
    }
  }
}

void DHT() {
  // Lees de DHT11 sensor
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("Vochtigheid: " + String(hum));
  Serial.println("  Temperatuur: " + String(temp) + "°C");
}

void DS18B20() {
  // Lees de DS18B20 sensor
  sensors.requestTemperatures();
  bodem_temp = sensors.getTempCByIndex(0);
  Serial.println("Bodemtemperatuur: " + String(bodem_temp) + "ºC");
}

void Vochtsensor() {
  // Lees de bodemvochtsensor
  grondwater = analogRead(soil_moisture_pin);
  Serial.println(grondwater);
}

void UltraSone() {
  // Meet de afstand met de ultrasone sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  tijd = pulseIn(echoPin, HIGH);
  afstandCM = tijd * SOUND_SPEED / 2;
  Serial.print("Afstand (cm): ");
  Serial.println(afstandCM);
}

void LDR() {
  // Lees de lichtintensiteit
  ldr = analogRead(LDRPIN);
  Serial.println("Lichtintensiteit: " + String(ldr));
}

void temp_regeling() {
  // Regel de verwarming op basis van de temperatuur
  if (temp < min_temp) {
    digitalWrite(verwarming, HIGH);
  } else {
    digitalWrite(verwarming, LOW);
  }
}

void licht_regeling() {
  // Regel de LED-strip op basis van tijd en lichtintensiteit
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Tijd niet beschikbaar");
    return;
  }

  int hour = timeinfo.tm_hour;
  int minute = timeinfo.tm_min;
  int second = timeinfo.tm_sec;
  long currentTime = hour * 3600 + minute * 60 + second; // Huidige tijd in seconden sinds middernacht

  long startTime = 6 * 3600; // Starttijd voor de LED-strip (bijv. 06:00 AM)
  long endTime;

  // Stel de eindtijd in op basis van de gescande kaart
  if (kaart == "Waterkers") {
    endTime = startTime + (max_lichttijd / 1000); // 5 uur in seconden
  } else if (kaart == "Aardbei") {
    endTime = startTime + (max_lichttijd / 1000); // 9 uur in seconden
  } else {
    endTime = startTime; // Als geen kaart is gescand, stel het einde gelijk aan de start
  }

  // Controleer of het tijd is om de LED-strip aan te zetten en of de lichtintensiteit laag genoeg is
  if (currentTime >= startTime && currentTime < endTime && ldr < 500) { // Voeg lichtwaarde controle toe
    digitalWrite(strip, HIGH);
  } else {
    digitalWrite(strip, LOW);
  }

  // Print de tijdsinstellingen voor debugging
  Serial.print("Huidige tijd: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
  Serial.print("Starttijd: ");
  Serial.println(startTime);
  Serial.print("Eindtijd: ");
  Serial.println(endTime);
}

void water_regeling() {
  // Regel de waterpomp op basis van de grondvochtigheid
  if (grondwater < min_grondwater) {
    digitalWrite(pomp, HIGH);
  } else {
    digitalWrite(pomp, LOW);
  }
}

void LCD() {
  // Update het LCD-scherm met actuele waarden
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Plant: " + kaart);
  lcd.setCursor(0, 1);
  lcd.print("Temp: " + String(temp) + "C");
  lcd.setCursor(0, 2);
  lcd.print("Vocht: " + String(hum) + "%");
  lcd.setCursor(0, 3);
  lcd.print("Bodem: " + String(bodem_temp) + "C");
}

void publish() {
  // Alle waarden omzetten naar een String
  String Shum = String((float)hum);
  String Stemp = String((float)temp);
  String Sbodem_temp = String((float)bodem_temp);
  String Sgrondwater = String((float)grondwater);
  String SafstandCM = String((float)afstandCM);
  String Sldr = String((float)ldr);

  // Alle strings publiceren naar juiste topic
  client.publish("serre/serre/humdht11", Shum.c_str());
  client.publish("serre/serre/tempdht11", Stemp.c_str());
  client.publish("serre/serre/ds18b20", Sbodem_temp.c_str());
  client.publish("serre/serre/bodemvochtsensor", Sgrondwater.c_str());
  client.publish("serre/serre/ultrasone", SafstandCM.c_str());
  client.publish("serre/serre/ldr", Sldr.c_str());
}
