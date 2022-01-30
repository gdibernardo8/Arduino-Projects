/*
  REQUIRES the following Arduino libraries:
  - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
  - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
  - LiquidCrystal.h
*/

#define DHTBOX_PIN 8                 // Digital pin connected to the DHT sensor (box)
#define DHTENV_PIN 7                 // Digital pin connected to the DHT sensor (enviroment)
#define DHTTYPE DHT11                // DHT 11
#define POWER_PIN 10                 // Signal Pin of Relay for power
#define HOTCOLD_PIN 9                // Signal Pin of Relay for heather/cooler (polarization)
#define TRIMMER_PIN A0               // Signal from potentiometer (set)
#define TOLLERANCE 0.5               // Tollerance for temperature set
#define DELAY_TIME 500

#include "DHT.h"
#include <LiquidCrystal.h>
DHT dht_box(DHTBOX_PIN, DHTTYPE);
DHT dht_env(DHTENV_PIN, DHTTYPE);

//Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int count = 0;
float desiredTemperature = 0;
String currentStatus = "";

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(HOTCOLD_PIN, OUTPUT);
  dht_box.begin();
  dht_env.begin();
  lcd.begin(16, 2);
  lcd.print("DibeProofingBox");
  lcd.setCursor(0, 1);
  lcd.print("Version 1.0");
  delay(DELAY_TIME);
}

void loop() {
  count++;
  delay(DELAY_TIME);
  double t_box = dht_box.readTemperature();
  int h_box = dht_box.readHumidity();
  double t_env = dht_env.readTemperature();
  int h_env = dht_env.readHumidity();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h_box) || isnan(t_box)) {
    Serial.println("Sensor BOX ERROR! ");
    lcd.setCursor(0, 0);
    lcd.print("Sensor BOX ERROR! ");
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    return;
  }
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h_env) || isnan(t_env)) {
    Serial.println("Sensor ENV ERROR! ");
    lcd.setCursor(0, 0);
    lcd.print("Sensor ENV ERROR! ");
    lcd.setCursor(0, 1);
    lcd.print("                  ");
    return;
  }

  //Line 1
  char tF_box[6];
  dtostrf(t_box, 2, 1, tF_box);
  char tF_env[6];
  dtostrf(t_env, 2, 1, tF_env);
  String t_init = "";
  String line_1_a = t_init  + "Box: " + tF_box + (char)223 +" (" + h_box + "%)";
  String line_1_b = t_init  + "Env: " + tF_env + (char)223 +" (" + h_env + "%)";
  lcd.setCursor(0, 0);
  lcd.print(line_1_a);
  lcd.print(line_1_b);
  //Show the Environment temperature every now and then
  if (false && count > 20 && count <= 24) {  
    lcd.setCursor(0, 0);
    lcd.print(line_1_b);
    if (count >= 24) count = 0;
  }

  //Line 2
  Serial.println(line_1_a);
  lcd.setCursor(0, 1);
  int sensorValue = analogRead(TRIMMER_PIN);
  Serial.println(sensorValue);
  desiredTemperature = 0.010 * sensorValue + 18; //desiredTemperature = 0.025 * sensorValue + desiredTemperatureMin; // parameter = (desiredTemperatureMax - desiredTemperatureMin) / 1000 
  Serial.println(desiredTemperature);
  char tF_desidered[6];
  dtostrf(desiredTemperature, 2, 1, tF_desidered);
  String line_2 = t_init + "Desired: " + tF_desidered + (char)223;

  //heading/cooling logic
  if ((currentStatus != "m" && t_box < desiredTemperature) || (currentStatus == "m" && t_box < desiredTemperature - TOLLERANCE)) {
    Serial.println("Heating");
    if (currentStatus == "c") {
        digitalWrite(POWER_PIN, LOW); 
        delay(DELAY_TIME/2); 
    }
    digitalWrite(HOTCOLD_PIN, HIGH); //heating
    delay(DELAY_TIME/4); 
    digitalWrite(POWER_PIN, HIGH);
    line_2 += " H  ";
    currentStatus = "h";
  }
  if ((currentStatus != "m" && t_box >= desiredTemperature) || (currentStatus == "m" && t_box >= desiredTemperature + TOLLERANCE)) {
    Serial.println("Cooling");
    if (currentStatus == "h") {
        digitalWrite(POWER_PIN, LOW); 
        delay(DELAY_TIME/2); 
    }
    digitalWrite(HOTCOLD_PIN, LOW); //coolings
    delay(DELAY_TIME/4); 
    digitalWrite(POWER_PIN, HIGH);
    line_2 += " C  ";
    currentStatus = "c";
  }
  if (currentStatus == "h" && t_box >= desiredTemperature) {
    Serial.println("Maintaining");
    currentStatus = "m";
  }
  if (currentStatus == "c" && t_box <= desiredTemperature) {
    Serial.println("Maintaining");
    digitalWrite(POWER_PIN, LOW);
    digitalWrite(HOTCOLD_PIN, LOW);
    line_2 += " M  ";
    currentStatus = "m";
  }
  Serial.println("CurrentStatus:" + currentStatus);
  lcd.print(line_2);
}
