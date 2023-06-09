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

// DS18B20 Library
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 Variables
#define DS18PIN 3
OneWire oneWire(DS18PIN);
DallasTemperature sensors(&oneWire);

// Relay Pins
/*
#define PERIS1 7
#define PERIS2 8
#define PERIS3 9
*/
#define RELAY_PUMP1_PIN 10
#define RELAY_PUMP2_PIN 11
#define RELAY_FAN_PIN 12

// Pin State
int pinState1 = LOW;
int pinState2 = LOW;
int fanState = LOW;

// Millis Function
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;

// Intervals
unsigned long OnTime = 180000;
unsigned long OffTime = 720000;

// Threshold Values
const int HUMID_THRESHOLD = 70;

// HCSR04
#define trigPin 4
#define echoPin 5
long duration;
int distance;

// GSM Library
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

// Your GPRS credentials
char auth[] = "YourAuthToken";
char apn[]  = "YourAPN";
char user[] = "";
char pass[] = "";

#define SerialAT Serial1 //Connect SIM_RX to 18, SIM_TX to 19 of MEGA
TinyGsm modem(SerialAT);
BlynkTimer timer;

void ultrasonic() {
  digitalWrite(trigPin, LOW);
  delay(2000);
  digitalWrite(trigPin, HIGH);
  delay(10000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Blynk.virtualWrite(V0, distance);
  tower();
  /*
  lcd.setCursor(8, 1);
  lcd.print("D: ");
  lcd.print(distance);
  */
}

void tower() {
  unsigned long currentMillis1 = millis();
  if (distance == 80) {
    digitalWrite(RELAY_PUMP1_PIN, LOW);
    // insert mixing code
  }
  else {
    if ((pinState1 == HIGH) && (currentMillis1 - previousMillis1 >= OnTime)) {
      pinState1 = LOW;
      previousMillis1 = currentMillis1;
      digitalWrite(RELAY_PUMP1_PIN, pinState1);
    }
    else if ((pinState1 == LOW) && (currentMillis1 - previousMillis1 >= OffTime)) {
      pinState1 = HIGH;
      previousMillis1 = currentMillis1;
      digitalWrite(RELAY_PUMP1_PIN, pinState1);
    }
  }
}

void dht22() {
  unsigned long currentMillis2 = millis();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (h < HUMID_THRESHOLD) {
    if ((pinState2 == HIGH) && (currentMillis2 - previousMillis2 >= 5000)) {
      pinState2 = LOW;
      previousMillis2 = currentMillis2;
      digitalWrite(RELAY_PUMP2_PIN, pinState2);    
    }
    else if ((pinState2 == LOW) && (currentMillis2 - previousMillis2 >= 10000)) {
      pinState2 = HIGH;
      previousMillis2 = currentMillis2;
      digitalWrite(RELAY_PUMP2_PIN, pinState2);
    }
  }
  else if (h >= HUMID_THRESHOLD) {
    //SerialMon.println("The humidity is at 100%.");
    digitalWrite(RELAY_PUMP2_PIN, LOW);
  }
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, t);
  /*
  lcd.setCursor(0, 0);
  lcd.print("H: ");
  lcd.print(h);

  lcd.setCursor(8, 0);
  lcd.print("ET: ");
  lcd.print(t);
  */
}

void ds18b20() {
  unsigned long currentMillis3 = millis();
  sensors.requestTemperatures();
  float st = sensors.getTempCByIndex(0);
  if (st > 33) {
    if ((fanState == HIGH) && (currentMillis3 - previousMillis3 >= 5000)) {
      fanState = LOW;
      previousMillis3 = currentMillis3;
      digitalWrite(RELAY_FAN_PIN, fanState);
    }
    else if ((fanState == LOW) && (currentMillis3 - previousMillis3 >= 10000)) {
      fanState = HIGH;
      previousMillis3 = currentMillis3;
      digitalWrite(RELAY_FAN_PIN, fanState);
    }
  }
  Blynk.virtualWrite(V3, st);
  /*
  lcd.setCursor(0, 1);
  lcd.print("ST: ");
  lcd.print(st);
  */
}

void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("H: "));
  lcd.print(dht.readHumidity());
  lcd.setCursor(8, 0);
  lcd.print(F("ET: "));
  lcd.print(dht.readTemperature());
  lcd.setCursor(0, 1);
  lcd.print(F("ST: "));
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor(8, 1);
  lcd.print(F("D: "));
  lcd.print(distance);
}


void setup() {
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
  // Initialize Submersible Pump
  pinMode(RELAY_PUMP1_PIN, OUTPUT);
  pinMode(RELAY_PUMP2_PIN, OUTPUT);
  pinMode(RELAY_FAN_PIN, OUTPUT);
  // Blynk
  Blynk.begin(auth, modem, apn, user, pass);
  timer.setInterval(100L, ultrasonic);
  timer.setInterval(100L, dht22);
  timer.setInterval(100L, ds18b20);
  timer.setInterval(100L, displayLCD);
}

void loop() {
  Blynk.run();
  timer.run();
}
