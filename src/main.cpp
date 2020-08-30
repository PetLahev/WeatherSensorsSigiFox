#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

const int sigFoxTXpin = 8;
const int sigFoxRXpin = 9;
const int pinSensors = 4;

OneWire oneWireDS(pinSensors);
DallasTemperature sensorsDS(&oneWireDS);
SoftwareSerial Sigfox(sigFoxRXpin, sigFoxTXpin);

void setup() {
  Serial.begin(9600);
  sensorsDS.begin();
  Sigfox.begin(9600);
  Serial.println("Vse nastaveno!");
}

void loop() {

  sensorsDS.requestTemperatures();
  Serial.print("Teplota cidla kurt: ");
  Serial.print(sensorsDS.getTempCByIndex(0));
  Serial.println(" C");

  Serial.print("Teplota cidla venek: ");
  Serial.print(sensorsDS.getTempCByIndex(1));
  Serial.println(" C");

  // Test SigFox
  if (Sigfox.available()) {
    Serial.write(Sigfox.read());

  delay(1000);
}