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

// Relay Pins
#define RELAY_PUMP1_PIN 10
#define RELAY_PUMP2_PIN 11
#define RELAY_PUMP3_PIN 12
#define RELAY_PUMP4_PIN 13

// Pin State
int pinState1 = LOW;
int pinState2 = LOW;
int pinState3 = LOW;
int pinState4 = LOW;

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
const int HUMID_THRESHOLD_UPPER = 100;
const int HUMID_THRESHOLD_LOWER = 70;

void towers() {
  tower1();
  tower2();
  tower3();
}

void tower1() {
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

void tower2() {
  unsigned long currentMillis2 = millis();
  if ((pinState2 == HIGH) && (currentMillis2 - previousMillis2 >= OnTime1)) {
    pinState2 = LOW;
    previousMillis2 = currentMillis2;
    digitalWrite(RELAY_PUMP2_PIN, pinState2);
  }
  else if ((pinState2 == LOW) && (currentMillis2 - previousMillis2 >= OffTime1)) {
    pinState2 = HIGH;
    previousMillis2 = currentMillis2;
    digitalWrite(RELAY_PUMP2_PIN, pinState2);
  }
}
  
void tower3() {
  unsigned long currentMillis3 = millis();
  if ((pinState3 == HIGH) && (currentMillis3 - previousMillis3 >= OnTime1)) {
    pinState3 = LOW;
    previousMillis3 = currentMillis3;
    digitalWrite(RELAY_PUMP3_PIN, pinState3);
  }
  else if ((pinState3 == LOW) && (currentMillis3 - previousMillis3 >= OffTime1)) {
    pinState3 = HIGH;
    previousMillis3 = currentMillis3;
    digitalWrite(RELAY_PUMP3_PIN, pinState3);
  }
}

void dht22() {
  unsigned long currentMillis4 = millis();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (h < HUMID_THRESHOLD_LOWER) {
    if ((pinState4 == HIGH) && (currentMillis4 - previousMillis4 >= OnTime2)) {
      pinState4 == LOW;
      previousMillis4 = currentMillis4;
      digitalWrite(RELAY_PUMP4_PIN, pinState4);    
    }
    else if ((pinState4 == LOW) && (currentMillis4 - previousMillis4 >= OffTime2)) {
      pinState4 = HIGH;
      previousMillis4 = currentMillis4;
      digitalWrite(RELAY_PUMP4_PIN, pinState4);
    }
  }
  else if (h >= HUMID_THRESHOLD_LOWER) {
    //SerialMon.println("The humidity is at 100%.");
    digitalWrite(RELAY_PUMP4_PIN, LOW);
  }
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
  displayLCD();
}
