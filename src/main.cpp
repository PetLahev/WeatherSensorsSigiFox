#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

const int sigFoxTXpin = 9;
const int sigFoxRXpin = 8;
const int pinSensorCourt = 4;
const int pinSensorOutside = 5;

const int halfMinuteDelay = 30000;
unsigned long lastSend = 0;

OneWire oneWireCourt(pinSensorCourt);
OneWire oneWireOutside(pinSensorOutside);
DallasTemperature sensorCourt(&oneWireCourt);
DallasTemperature sensorOutside(&oneWireOutside);
SoftwareSerial Sigfox(sigFoxRXpin, sigFoxTXpin);

void sendData(unsigned int tempCourt, unsigned int tempOutside);

void setup()
{
  Serial.begin(9600);
  sensorCourt.begin();
  sensorOutside.begin();
  Sigfox.begin(9600);
  lastSend = 0;
  Serial.println("Vse nastaveno!");
}

void loop()
{

  float temperatures[2] = {0, 0};
  Serial.println("Zacinam merit 4x po 5 sekundach");
  for (size_t i = 0; i < 4; i++)
  {
    sensorCourt.requestTemperatures();
    temperatures[0] += sensorCourt.getTempCByIndex(0);
    sensorOutside.requestTemperatures();
    temperatures[1] += sensorOutside.getTempCByIndex(0);
    delay(5000);
  }

  float avgTempCourt = temperatures[0] / (float)4.0;
  float avgTempOutside = temperatures[1] / (float)4.0;
  Serial.print("Teplota cidla kurt: ");
  Serial.print(avgTempCourt);
  Serial.println(" C");

  Serial.print("Teplota cidla vzduch: ");
  Serial.print(avgTempOutside);
  Serial.println(" C");

  // Test SigFox
  // testSigFox();
  Serial.print("Seconds ");
  Serial.println((millis() / 1000));
  Serial.print("Last send ");
  Serial.println(lastSend);

  // send the d ate ti SigFox only if the interval is equal or greater than 12 minutes (720 sec)
  if (lastSend == 0 || ((millis() / 1000) - lastSend) >= 720)
  {
    sendData(avgTempCourt * 100, avgTempOutside * 100);
    Serial.println("Data sent");
    lastSend = millis() / 1000;
  }

  delay(halfMinuteDelay);
}

void sendData(unsigned int tempCourt, unsigned int tempOutside)
{

  char message[12];
  sprintf(message, "%04X%04X%04X", tempCourt, tempOutside, 0);
  Serial.print("Odeslani dat do Sigfox site, odesilam: ");
  Serial.print(tempCourt);
  Serial.print(" kurt, ");
  Serial.print(tempOutside);
  Serial.print(" vzduch, hexa tvar: ");
  Serial.println(message);

  delay(1000);
  Sigfox.print("AT$SF=");
  Sigfox.println(message);
  delay(1000);
}

void testSigFox()
{
  if (Sigfox.available())
  {
    Serial.write(Sigfox.read());
  }

  if (Serial.available())
  {
    Sigfox.write(Serial.read());
  }
}