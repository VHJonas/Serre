//Voorafbepaalde instellingen per Sensor/Actuator
// RFID-tag
#include <SPI.h>                   // Voegt de SPI Library toe
#include <MFRC522.h>               // Voegt de RFID Library toe
#define SS_PIN 5                   // Bepaalt dat de SS_pin aan GPIO 5 is verbonden
#define RST_PIN 14                 // Bepaalt dat de RST_pin aan GPIO 14 is verbonden
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Maak een instantie van MFRC522.

// DHT11
#include "DHT.h"           // Voegt de DHT.. Library toe
#define DHTPIN 26          // Geeft aan dat de Data pin van de DHT11 verbonden is aan pin 34
#define DHTTYPE DHT11      // Geeft aan dat het DHT-type == 11
DHT dht(DHTPIN, DHTTYPE);  // Initialiseren van DHT-Sensor
// Aanmaken van variabelen(floats) om de vochtigheid en temperatuur te meten verderin de code
float hum;
float temp;

// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 15;            // Bepaalt dat de DS18B20 aan GPIO 5 is verbonden
OneWire oneWire(oneWireBus);          // Stel een oneWire instantie in om te communiceren met OneWire apparaten
DallasTemperature sensors(&oneWire);  // Geef onze oneWire referentie door aan de Dallas Temperature sensor
// Aanmaken van variabelen om de temperatuur in de bodem te meten
float bodem_temp;

// Bodem_Vocht_Sensor
#define soil_moisture_pin 39  // Bepaalt dat de data_pin van de vochtigheidssensor op GPIO 39 is aangesloten
// Aanmaken van variabelen om de vochtigheid in de bodem te meten
float grondwater;

// UltraSone
const int trigPin = 32;  // Bepaalt dat de trigPin aan GPIO 32 is verbonden
const int echoPin = 35;  // Bepaalt dat de echoPin aan GPIO 35 is verbonden
#define SOUND_SPEED 0.034
// Aanmaken van variabelen om de afstand te meten
long tijd;
float afstandCM;
String level;

// LDR
#define LDRPIN 13  // Bepaalt dat de datapin v.d. LDR aan GPIO 13 is verbonden
int ldr;

//LED (simulatie verwarming)
#define verwarming 25  // Bepaalt dat de LED aan GPIO 25 is verbonden

//Ventilators
#define ventilatie 16  // Bepaalt dat de Ventilator aan GPIO 16 is verbonden

//LCD met I2C
#include <LiquidCrystal_I2C.h>       //Voegt de LiquidCrystal_I2C Library toe
LiquidCrystal_I2C lcd(0x27, 20, 4);  // stel het LCD-adres in op 0x27 voor een weergave van 16 tekens en 2 regels
String kaart;

//Waterpomp
#define pomp 4  // Bepaalt dat de waterpomp aan GPIO 17 is verbonden

//Ledstrip
#define strip 27  // Bepaalt dat de DS18B20 aan GPIO 27 is verbonden


// constanten om de instellingen van bepaalde plant te verkiezen
int max_grondwater = 200;  //Geeft het maximum aan benodigde water weer
int min_grondwater = 0;    //Geeft het minimum aan benodigde water weer
int max_temp = 0;          //Geeft de maximum temperatuur van de serre weer
int min_temp = 0;          //Geeft de minimum temperatuur van de serre weer
int max_lichttijd = 0;
int dag = 0;  // 0 = begin van de dag --> na 24 uren = 86400000 milliseconden


//----------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(9600);  // starten seriële communicatie op 9600 baud
  dht.begin();         // Starten met lezen van DHT11
  SPI.begin();         //Initialiseer SPI-bus
  mfrc522.PCD_Init();  //Initialiseer mfrc522 (RFID)
  sensors.begin();     //Initialiseer DS18B20

  //Voorbepaling vvan pinnen
  pinMode(DHTPIN, INPUT);             //DHTPIN als INPUT bepalen (DHT11)
  pinMode(soil_moisture_pin, INPUT);  //soil_moisture_pin als INPUT bepalen (Bodem_vocht_sensor)
  pinMode(trigPin, OUTPUT);           //trigPin als OUTPUT bepalen (UltraSone)
  pinMode(echoPin, INPUT);            //echoPin als INPUT bepalen  (UltraSone)
  pinMode(LDRPIN, INPUT);             // LDRPIN als INPUT bepalen   (LDR)
  pinMode(verwarming, OUTPUT);        //verwarming als OUTPUT bepalen (verwarming)
  pinMode(strip, OUTPUT);             //strip als OUTPUT bepalen (Ledstrip)
  pinMode(pomp, OUTPUT);

  //Initialisatie LCD
  lcd.init();
  lcd.backlight();  //Achtergrond licht aanzetten
}

//----------------------------------------------------------------------------------------------------------

// Aangemaakte functies per sensor om de void loop overzichtelijk en simpel te houden

void RFID() {
  // Zoekt voor nieuwe kaart + leest het
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

    // Leest de UID + laat het zien op de serial monitor
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
    //Checkt welke kaart er wordt gescand aan de hand van de gelezen UID ("XX XX XX XX");
    if (content.substring(1) == "53 8A F6 27") {  // witte kaart

      kaart = "Waterkers";
      //Instellingen tuinkers
      max_grondwater = 30;
      min_grondwater = 5;
      max_temp = 22;
      min_temp = 20;
      max_lichttijd = 57600000;  //16 uren = 57600000 milliseconden
    }

    if (content.substring(1) == "63 6C 58 14") {  // blauwe kaart
      //Op LCD schrijven dat de blauwee badge is gescand
      kaart = "Aardbei";
      //Instellingen aardbei
      max_grondwater = 200;
      min_grondwater = 50;
      max_temp = 24;
      min_temp = 20;
      max_lichttijd = 32400000;  //9 uren = 32400000 milliseconden
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////

void DHT() {
  hum = dht.readHumidity();      //float hum die de vochtigheidswaarden meet
  temp = dht.readTemperature();  //float tem die de temperatuurwaarden meet

  // Schrijven op Serial Monitor
  // Vochtigheid: 'waarde hum'
  //Temperatuur: 'waarde tem'
  Serial.print("Vochtigheid: " + String(hum));
  Serial.println("  Temperatuur: " + String(temp) + "°C");
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void DS18B20() {
  //Leest de waarde van de DS18B20 via OneWire
  sensors.requestTemperatures();
  bodem_temp = sensors.getTempCByIndex(0);
  //Geeft de waarde in de Seriële Monitor("Bodemtemperatuur: ...°C")
  Serial.println("Bodemtemperatuur: " + String(bodem_temp) + "ºC");
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void Vochtsensor() {
  //Leest de analoge waarde van de vochtigheidspin
  grondwater = analogRead(soil_moisture_pin);
  //Geeft de waarde in de Seriële monitor weer
  Serial.println(grondwater);
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void UltraSone() {
  // Maakt trigPin leeg
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Stelt trigPin in op HOOG voor 10 microseconden
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Leest echoPin, retourneert de reistijd van de geluidsgolf in microseconden
  tijd = pulseIn(echoPin, HIGH);
  // Bereken de afstand
  afstandCM = tijd * SOUND_SPEED / 2;
  // Print de afstand in de seriële monitor
  Serial.print("Afstand (cm): ");
  Serial.println(afstandCM);

  if (afstandCM <= 5) {
    level = "VOL";
  }
  if (afstandCM > 10) {
    level = "!!LEEG !!";
  }
  if (5 < afstandCM < 10) {
    level = "Voldoende";
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void LDR() {
  //Lezen van de analoge LDRPIN
  ldr = analogRead(LDRPIN);
  //Geeft de waarde in de Seriële monitor weer
  Serial.println("Lichtwaarde: " + String(ldr));
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void temp_regeling() {
  //temp == te laag; verwarming aan + ventilatie uit
  if (temp < min_temp) {
    digitalWrite(verwarming, HIGH);  //Aanzetten verwarming(LED)
    pinMode(ventilatie, INPUT);      //Uitzetten ventilatie
  }
  //temp == te hoog; verwarming uit + ventilatie aan
  if (temp > max_temp) {
    digitalWrite(verwarming, LOW);  //Uitzetten verwarming(LED)
    pinMode(ventilatie, OUTPUT);    //Aanzetten ventilatie
  }
  //temp == in orde; verwarming uit + ventilatie uit
  if (min_temp <= temp <= max_temp) {
    digitalWrite(verwarming, LOW);  //Uitzetten verwarming(LED)
    pinMode(ventilatie, INPUT);     //Uitzetten ventilatie
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void licht_regeling() {
  //Zolang de plant, minder licht heeft gekregen dan de maximum lichttijd, dan zal er gekeken worden of er genoeg natuurlijk licht is.
  if (dag < max_lichttijd) {
    // Indien het te donker is, dan gaat de ledstrip aan
    if (ldr > 2048) {
      digitalWrite(strip, HIGH);
    } else {  //Anders blijft de LED uit
      digitalWrite(strip, LOW);
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void water_regeling() {
  if (grondwater > min_grondwater) {
    digitalWrite(pomp, HIGH);
    Serial.println("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  } else {
    digitalWrite(pomp, LOW);
    Serial.println("ooooooooooooooooooooooooooooooooooooooooooooooooooo");
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void LCD() {
  //Display
  //"Gekweekt product"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Instelling");
  lcd.setCursor(0, 1);
  lcd.print(kaart);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String(temp) + "C");
  lcd.setCursor(0, 1);
  lcd.print(String(hum) + "%");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Belichting:");
  lcd.setCursor(0, 1);
  lcd.print(ldr);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DS18B20: " + String(bodem_temp));
  lcd.setCursor(0, 1);
  lcd.print("Grond: " + String(grondwater));
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waterbak:");
  lcd.setCursor(0, 1);
  lcd.print(level);
  delay(1000);
}
//////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------

void loop() {
  RFID();            //Code "void RFID" oproepen
  DHT();             //Code "void DHT" oproepen
  DS18B20();         //Code "void DS18B20" oproepen
  Vochtsensor();     //Code "void Vochtsensor" oproepen
  UltraSone();       //Code "void UltraSone" oproepen
  LDR();             //Code "void LDR" oproepen
  temp_regeling();   //Code "void temp_regeling" oproepen
  licht_regeling();  //Code "void LDR" oproepen
  water_regeling();  //Code "void water_regeling" oproepen

  LCD();


  delay(1000);                             //Een delay van een halve seconde vooraleer de code heropgestart wordt
  dag = dag + 5000;                        //Na elke lus zijn we 500 ms verder doorheen de dag;
  Serial.println("TIJD: " + String(dag));  //Ter controle hoeveel millis we zitten
  if (dag == 86400000) {                   // Als het einde van de dag is bereikt (24 uur = 86400000 in ms)
    dag = 0;                               // Dan wordt de dag gereset en begint de volgende
  }
}