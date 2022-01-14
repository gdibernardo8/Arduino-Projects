/*
 REQUIRES the following Arduino libraries:
 - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
 - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 - LiquidCrystal.h 
*/

#define DHTPIN 13                 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11             // DHT 11
#define desiredTemperatureInit 26 // Set initial value of Desired Temperature
#define sigPin 10                 // Signal Pin of Relay (for heather/cooler)
#define trimmerPin A0              // Signal from potentiometer

#include "DHT.h"
#include <LiquidCrystal.h>
DHT dht(DHTPIN, DHTTYPE);

//Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Buttons
int buttonState = 0;
int desiredTemperature = desiredTemperatureInit;  

void setup() {
  Serial.begin(9600);
  pinMode(sigPin, OUTPUT); 
  dht.begin();
  lcd.begin(16, 2);
  lcd.print("DibeProofBox0.1");
}

void loop() {
  delay(500);
  int h = dht.readHumidity();
  double t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //formatting temperature float
  char tF[6]; 
  dtostrf(t, 2, 1, tF);
  
  Serial.print(F(" H: "));
  Serial.print(h);
  Serial.print(F("%  T: "));
  Serial.print(tF);
  Serial.println(F("°C"));
  
  lcd.setCursor(0, 0);
  //lcd.print(F("                   "));
  lcd.print(F("T: "));
  lcd.print(tF);
  //lcd.print(char(176));
  lcd.print("C ");
  lcd.print(F("H: "));
  lcd.print(h);
  lcd.print("%  ");


  // set the cursor to column 0, line 1
  int sensorValue = analogRead(trimmerPin);
  lcd.setCursor(0, 1);
  desiredTemperature = 0.025 * sensorValue+7;  //range between 7 and 32

  lcd.print("Desired  T: ");
  lcd.print(desiredTemperature);
  lcd.print("C  ");
  

  if (t < desiredTemperature) {
    digitalWrite(sigPin, HIGH);//play
  }
  else {
    digitalWrite(sigPin, LOW);//play
  }  
}
