#include <Arduino.h>
// Arduino Serial Monitor
#define SerialMon Serial
// LCD Library
#include <LiquidCrystal_I2C.h>
// LCD Address
LiquidCrystal_I2C lcd(0x27, 16, 2);
// DHT22 Library
#include "DHT.h"
// DHT22
#define DHTPIN 2
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
float h = 0;
float t = 0;
// DS18B20 Library
#include <OneWire.h>
#include <DallasTemperature.h>
// DS18B20
#define DS18PIN 3
OneWire oneWire(DS18PIN);
DallasTemperature sensors(&oneWire);
float tempC;
// HC-SR04 Pins
#define trigPin 4
#define echoPin 5
long duration;
int distance;
// Relay Pins
#define RELAY_MIX1_PIN 6
#define RELAY_MIX2_PIN 7
#define RELAY_MIX3_PIN 8
#define RELAY_PUMP1_PIN 9
#define RELAY_PUMP2_PIN 10
#define RELAY_FAN_PIN 11
// Pin State
int pinState1 = LOW;
int pinState2 = LOW;
int fanState = LOW;
int pumpState = LOW;
// Millis Function
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;
// Intervals
unsigned long OnTime1 = 180000;
unsigned long OffTime1 = 720000;
unsigned long OnTime2 = 5000;
unsigned long OffTime2 = 10000;
// Threshold Values
const int TEMP_THRESHOLD = 30;
const int DIST_THRESHOLD = 10;
const int DIST_THRESHOLD_REFILL = 25;
const int HUMID_THRESHOLD = 70;
//GSM Library
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
// GPRS credentials
const char auth[] = "hMsJBZu0Ndwo_Zc8BR_wMkB88BiFcnPZ";
const char apn[] = "internet";
const char user[] = "";
const char pass[] = "";
// GSM Variables
#define SerialAT Serial1
TinyGsm modem(SerialAT);
BlynkTimer timer;
// Load Cell Library
#include "HX711_ADC.h"
// Load Cell
#define HX711_dout 12
#define HX711_sck 13
HX711_ADC LoadCell(HX711_dout, HX711_sck);
// Load Cell Variables
const int calVal_calVal_eepromAdress = 0;
float i = 0;
static boolean newDataReady = 0;
const int Water = 10000;
const int SolA = 10010;
const int SolB = 10020;

void ultrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
}

void mixing() {
  if ((LoadCell.getData() <= SolB)&&(distance >= DIST_THRESHOLD_REFILL)) {
    digitalWrite(RELAY_MIX1_PIN, LOW);
    digitalWrite(RELAY_MIX2_PIN, LOW);
    digitalWrite(RELAY_MIX3_PIN, LOW);
    if (LoadCell.getData() <= Water) {
      digitalWrite(RELAY_MIX1_PIN, HIGH);
      digitalWrite(RELAY_MIX2_PIN, LOW);
      digitalWrite(RELAY_MIX3_PIN, LOW);
    }
    if ((LoadCell.getData() > Water) && (LoadCell.getData() < SolA)) {
      digitalWrite(RELAY_MIX1_PIN, LOW);
      digitalWrite(RELAY_MIX2_PIN, HIGH);
      digitalWrite(RELAY_MIX3_PIN, LOW);
    }
    if ((LoadCell.getData() > SolA) && (LoadCell.getData() < SolB)) {
      digitalWrite(RELAY_MIX1_PIN, LOW);
      digitalWrite(RELAY_MIX2_PIN, LOW);
      digitalWrite(RELAY_MIX3_PIN, HIGH);
    }
    if (LoadCell.getData() >= SolB) {
      digitalWrite(RELAY_MIX1_PIN, LOW);
      digitalWrite(RELAY_MIX2_PIN, LOW);
      digitalWrite(RELAY_MIX3_PIN, LOW);
    }
  }
}

void pump() {
  unsigned long currentMillis1 = millis();
  if ((pinState1 == HIGH) && (currentMillis1 - previousMillis1 >= OnTime1)) {
    pinState1 = LOW;
    previousMillis1 = currentMillis1;
    digitalWrite(RELAY_PUMP1_PIN, pinState1);
  }
  else if ((pinState1 == LOW) && (currentMillis1 - previousMillis1 >= OffTime1)) {
    pinState1 = HIGH;
    previousMillis1 = currentMillis1;
    digitalWrite(RELAY_PUMP1_PIN, pinState1);
  }
}

void tower() {
  newDataReady = 0;
  if (LoadCell.update()) newDataReady = true;
  
  if (newDataReady) {
    i = LoadCell.getData();
    newDataReady = 0;
  }

  if (pumpState == LOW) { 
    digitalWrite(RELAY_PUMP1_PIN, LOW);
    mixing();
    if (distance <= DIST_THRESHOLD) {
      pumpState = HIGH;
    }
  }

  if (pumpState == HIGH) {
    digitalWrite(RELAY_MIX1_PIN, LOW);
    digitalWrite(RELAY_MIX2_PIN, LOW);
    digitalWrite(RELAY_MIX3_PIN, LOW);
    pump();
    if (distance >= DIST_THRESHOLD_REFILL) {
      pumpState = LOW;
    }
  }
}

void dht22() {
  unsigned long currentMillis2 = millis();
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (h < HUMID_THRESHOLD) {
    if ((pinState2 == HIGH) && (currentMillis2 - previousMillis2 >= OnTime2)) {
      pinState2 = LOW;
      previousMillis2 = currentMillis2;
      digitalWrite(RELAY_PUMP2_PIN, pinState2);
    }
    else if ((pinState2 == LOW) && (currentMillis2 - previousMillis2 >= OffTime2)) {
      pinState2 = HIGH;
      previousMillis2 = currentMillis2;
      digitalWrite(RELAY_PUMP2_PIN, pinState2);
    }
  }
  else if (h >= HUMID_THRESHOLD) {
    digitalWrite(RELAY_PUMP2_PIN, LOW);
  }
}

void ds18b20() {
  unsigned long currentMillis3 = millis();
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  if (tempC > TEMP_THRESHOLD) {
    if ((fanState == HIGH) && (currentMillis3 - previousMillis3 >= OnTime2)) {
      fanState = LOW;
      previousMillis3 = currentMillis3;
      digitalWrite(RELAY_FAN_PIN, fanState);
    }
    else if ((fanState == LOW) && (currentMillis3 - previousMillis3 >= OffTime2)) {
      fanState = HIGH;
      previousMillis3 = currentMillis3;
      digitalWrite(RELAY_FAN_PIN, fanState);
    }
  }
}

void sendHumidity() {
  Blynk.virtualWrite(V0, dht.readHumidity());
}

void sendETemp() {
  Blynk.virtualWrite(V1, dht.readTemperature());
}

void sendSTemp() {
  Blynk.virtualWrite(V2, sensors.getTempCByIndex(0));
}

void sendDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Blynk.virtualWrite(V3, distance);
}

void sendWeight() {
  Blynk.virtualWrite(V4, LoadCell.getData());
}

void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("H:"));
  lcd.print(h);
  lcd.setCursor(8, 0);
  lcd.print(F("ET:"));
  lcd.print(t);
  lcd.setCursor(0, 1);
  lcd.print(F("ST:"));
  lcd.print(tempC);
  lcd.setCursor(10, 1);
  lcd.print(F("D:"));
  lcd.print(distance);
}

void setup() 
{
  // Start Serial Monitor
  SerialMon.begin(9600);
  // Start LCD
  lcd.init();
  lcd.backlight();
  // Set GSM module baud rate
  SerialAT.begin(115200);
  modem.init(); // or modem.restart()
  // DHT22 Connect
  dht.begin();
  // Initialize HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // DS18B20 Connect
  sensors.begin();
  // Initialize Submersible Pumps
  pinMode(RELAY_PUMP1_PIN, OUTPUT);
  pinMode(RELAY_PUMP2_PIN, OUTPUT);
  // Initialize Fan
  pinMode(RELAY_FAN_PIN, OUTPUT);
  // Initialize Peristaltic Pumps
  LoadCell.begin();
  float calibrationValue;
  calibrationValue = -100.73;
  unsigned long stabilizingtime = 1000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue);
  pinMode(RELAY_MIX1_PIN, OUTPUT);
  pinMode(RELAY_MIX2_PIN, OUTPUT);
  pinMode(RELAY_MIX3_PIN, OUTPUT);
  // Blynk
  Blynk.begin(auth, modem, apn, user, pass);
  timer.setInterval(100L, sendHumidity);
  timer.setInterval(100L, sendETemp);
  timer.setInterval(100L, sendSTemp);
  timer.setInterval(100L, sendDistance);
  timer.setInterval(100L, sendWeight);
}

void loop() {
  ultrasonic();
  tower();
  dht22();
  ds18b20();
  displayLCD();
  Blynk.run();
  timer.run();
}
