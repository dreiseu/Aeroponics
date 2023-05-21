#include <Arduino.h>

// Ultrasonic
// DS18
// GSM
// Peristaltic

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

// DS18B20 Library
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 Variables
#define DS18PIN 3
OneWire oneWire(DS18PIN);
DallasTemperature sensors(&oneWire);

// Relay Pins
#define RELAY_PUMP_PIN 10
#define RELAY_FAN_PIN 11

// Pin State
int pinState1 = LOW;
int pinState2 = LOW;

// Millis Function
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

// Intervals
unsigned long OnTime = 180000;
unsigned long OffTime = 720000;

// Threshold Values
const int HUMID_THRESHOLD_UPPER = 100;
const int HUMID_THRESHOLD_LOWER = 70;

void towers() {
  tower();
}

void tower() {
  unsigned long currentMillis1 = millis();
  if ((pinState1 == HIGH) && (currentMillis1 - previousMillis1 >= OnTime)) {
    pinState1 = LOW;
    previousMillis1 = currentMillis1;
    digitalWrite(RELAY_PUMP_PIN, pinState1);
  }
  else if ((pinState1 == LOW) && (currentMillis1 - previousMillis1 >= OffTime)) {
    pinState1 = HIGH;
    previousMillis1 = currentMillis1;
    digitalWrite(RELAY_PUMP_PIN, pinState1);
  }
}

void dht22() {
  unsigned long currentMillis2 = millis();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if
}

void ds18b20() {
  sensors.requestTemperatures();
}

void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Humidity: "));
  lcd.print(dht.readHumidity());
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(F("E.Temp: "));
  lcd.print(dht.readTemperature());
  lcd.print(" C");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("S.Temp: ");
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print(" C");
  delay(2000);
}

void setup() {
  // Start Serial Monitor
  SerialMon.begin(9600);
  // Start LCD
  lcd.begin();
  lcd.backlight();
  // DHT22 Connect
  dht.begin();
  // Initialize HC-SR04
  /*pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);*/
  // DS18B20 Connect
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // Initialize Submersible Pump
  pinMode(RELAY_PUMP1_PIN, OUTPUT);
  pinMode(RELAY_PUMP2_PIN, OUTPUT);
  pinMode(RELAY_PUMP3_PIN, OUTPUT);
  pinMode(RELAY_PUMP4_PIN, OUTPUT);
}

void loop() {
  towers();
  dht22();
  ds18b20();
  displayLCD();
}
