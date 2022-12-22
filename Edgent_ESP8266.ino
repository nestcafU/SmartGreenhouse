
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLbwewaqlR"
#define BLYNK_DEVICE_NAME "FarmSmart"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>

#define APP_DEBUG

#include "BlynkEdgent.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>

#define ON HIGH
#define OFF LOW
#define BLYNK_PRINT1 Serial
#define PIN_Soil A0
#define PIN_PUMP D6
#define PIN_DHT D5
#define DHTPIN PIN_DHT
#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t event;

float tempSensorData = 0;
float humiSensorData = 0;

int sensorValue = 0;
int outputValue = 0;

int TempValue = 45;
int SoilValue = 40;

bool autoCondition;

BlynkTimer timer;

BLYNK_WRITE(V6) {
  autoCondition = param.asInt();
}

BLYNK_WRITE(V4) {
  if(!autoCondition)
  {
    if (param.asInt()) {
      Serial.println("Manual : Pump = ON ");
      digitalWrite(PIN_PUMP, ON);
    }
    else {
      Serial.println("Manual : Pump = OFF ");
      digitalWrite(PIN_PUMP, OFF);
    }
  }
}


BLYNK_CONNECTED()
{
  Serial.print("BLYNK SERVER CONNECTED !!!");
  Blynk.syncAll();
}


void ProcessDesign() {
  if (autoCondition)
  {
    Serial.print(F("Temperature: "));
    Serial.print(tempSensorData);
    Serial.println(F(" C"));
    Serial.print(F("Humidity: "));
    Serial.print(humiSensorData);
    Serial.println(F("%"));
    Serial.print(F("Soil Moisture: "));
    Serial.print(outputValue);
    Serial.println(F(" %"));
    Serial.println();


    if (sensorValue <= 700 )//(sensorValue <= 700 )//(outputValue <= SoilValue)
    {
      Serial.println("Automatic : Pump = ON ");
      Serial.println();
      digitalWrite(PIN_PUMP, ON);
      Blynk.virtualWrite(V4 , ON);
    }

    if (sensorValue > 700)//(sensorValue > 700)//(outputValue > SoilValue)
    {
      Serial.println("Automatic : Pump = OFF ");
      Serial.println();
      digitalWrite(PIN_PUMP, OFF);
      Blynk.virtualWrite(V4 , OFF);
    }
  }
}

void TempHumidity() {

  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    tempSensorData = event.temperature;
    Serial.print(F("Temperature: "));
    Serial.print(tempSensorData);
    Serial.println(F(" C"));

    Blynk.virtualWrite(V0, tempSensorData);
  }

  //
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity"));
  }
  else {
    humiSensorData = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(humiSensorData);
    Serial.println(F("%"));
    
    Blynk.virtualWrite(V1, humiSensorData);
  }
}

void SoilMoisture() {

  sensorValue = analogRead(PIN_Soil);
  outputValue = map(sensorValue, 0, 1023, 100, 0);

  Serial.print(F("Soil Moisture = "));
  Serial.print(outputValue);
  Serial.println(F(" %"));
  Serial.println();

  Blynk.virtualWrite(V3, outputValue);
}


void setup()
{
    Serial.begin(115200);
    
    BlynkEdgent.begin();

    dht.begin();

    Wire.begin();
    
    pinMode(PIN_PUMP, OUTPUT);

    digitalWrite(PIN_PUMP, OFF);
    
    TempHumidity();
    SoilMoisture();
    ProcessDesign();

    timer.setInterval(15000L, TempHumidity);
    timer.setInterval(3000L, SoilMoisture);
    timer.setInterval(2000L, ProcessDesign);
}


void loop()
{
  Blynk.run();
  timer.run();
  BlynkEdgent.run();
}
