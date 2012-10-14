#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

#define DEFAULT_TEMP 64.0
#define TEMPERATURE_PRECISION 10
#define TEMP_STEP 1.0
#define RELAY_PIN 2
#define TEMP_PIN 3
#define DELTA 0.1
#define WARM_TIME 1
#define ON 1
#define OFF 0

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

int state = OFF;
int startWarming = 0;
float targetTemp = DEFAULT_TEMP;

// read the buttons
int readLCDButtons()
{
 int adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this...
}

void updateTargetTemp(int btn) {
  switch (btn) {
    case btnUP: {
      targetTemp = targetTemp + TEMP_STEP;
      break;
    }
    case btnDOWN: {
      targetTemp = targetTemp - TEMP_STEP;
      break;
    }
  }
}

void printState(float targetTemp, float currentTemp, int state) {
  lcd.setCursor(0, 1);
  lcd.print(targetTemp);
  lcd.setCursor(10, 1);
  lcd.print(currentTemp);
  lcd.setCursor(15, 1);
  switch(state) {
    case ON: {
      lcd.print("+");
      break;
    }
    case OFF: {
      lcd.print("-");
      break;
    }
  }
}

void setup() {
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0, 0);
  lcd.print("target   current"); // print a simple message
  sensors.begin();
  sensors.setResolution(TEMPERATURE_PRECISION);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  int lcdKey = readLCDButtons();  // read the buttons
  sensors.requestTemperatures();
  float currentTemp = sensors.getTempCByIndex(0);

  updateTargetTemp(lcdKey);

  // decide wether we nee to warm more
  if ((state == ON)
      && ((startWarming + WARM_TIME) < millis()/1000)) {
    digitalWrite(RELAY_PIN, LOW);
    state = OFF;
  } 
  if ((state == OFF)
      && (currentTemp < (targetTemp - DELTA))) {
    digitalWrite(RELAY_PIN, HIGH);
    startWarming = millis()/1000;
    state = ON;
  }
  
  printState(targetTemp, currentTemp, state);
}
