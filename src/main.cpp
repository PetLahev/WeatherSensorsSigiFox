#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

const int sigFoxTXpin = 9;
const int sigFoxRXpin = 8;
const int pinSensors = 4;

const int halfMinuteDelay = 30000;
unsigned long lastSend = 0;

OneWire oneWireDS(pinSensors);
DallasTemperature sensorsDS(&oneWireDS);
SoftwareSerial Sigfox(sigFoxRXpin, sigFoxTXpin);

void sendData(unsigned int avgTemperature);

void setup() {
  Serial.begin(9600);
  sensorsDS.begin();
  Sigfox.begin(9600);
  lastSend = 0;
  Serial.println("Vse nastaveno!");
}

void loop() {

  Serial.println("Zacinam merit 4x po 5 sekundach");
  float temperatures = 0;
  for (size_t i = 0; i < 4; i++)
  {
    sensorsDS.requestTemperatures();
    temperatures += sensorsDS.getTempCByIndex(0);
    delay(5000);
  }

  float avgTemperature = temperatures/(float)4.0;
  Serial.print("Teplota cidla kurt: ");
  Serial.print(avgTemperature);
  Serial.println(" C");

  // Test SigFox
  // testSigFox();

  // send the date ti SigFox only if the interval is equal or greater than 12 minutes (720 sec)
  if (lastSend == 0 || ((millis()/1000) - lastSend) >= 720) {
    sendData(avgTemperature * 100);
    Serial.println("Data sent");
    lastSend = millis()/1000;
  }

  delay(halfMinuteDelay);
}

void sendData(unsigned int avgTemperature) {

  char message[12];
  sprintf(message, "%04X%04X%04X", avgTemperature,0,0);
  Serial.print("Odeslani dat do Sigfox site, odesilam: ");
  Serial.print(avgTemperature);
  Serial.print(", hexa tvar: ");
  Serial.println(message);

  /* WATCH OUT
    While running via USB port with Arduino Nano
    these two SigFox commands below will cause overflow.
    It's running fine when not connected to USB
  */
  delay(500);
  Sigfox.print("AT$SF=");
  Sigfox.println(message);

  lastSend = millis()/1000;
  delay(500);
}

void testSigFox() {

  if (Sigfox.available()) {
    Serial.write(Sigfox.read());
  }

  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }

}