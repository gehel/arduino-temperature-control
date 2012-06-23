#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

OneWire oneWire(2);
DallasTemperature sensors(&oneWire);

const float TARGET_TEMP = 30.0;
const int RELAY_PIN = 13;
const float DELTA = 0.1;
const int ON = 1;
const int OFF = 0;
int state = OFF;

void setup() {
  lcd.begin(20, 4);
  lcd.print("current temp:");
  lcd.setCursor(0, 1);
  lcd.print("target temp: ");
  lcd.setCursor(15, 1);
  lcd.print(TARGET_TEMP);
  lcd.setCursor(0, 2);
  lcd.print("state:");
  lcd.setCursor(17, 2);
  lcd.print("OFF");
  sensors.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {  
  // read current temperature
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  lcd.setCursor(15, 0);
  lcd.print(temp);
  
  // decide wether we nee to warm more
  if (state == ON) {
   if (temp > (TARGET_TEMP + DELTA)) {
    digitalWrite(RELAY_PIN, LOW);
    state = OFF;
    lcd.setCursor(17, 2);
    lcd.print("OFF");
   }     
  } else {
   if (temp < (TARGET_TEMP - DELTA)) {
    digitalWrite(RELAY_PIN, HIGH);
    state = ON;
    lcd.setCursor(17, 2);
    lcd.print(" ON");
   }     
  }

}
