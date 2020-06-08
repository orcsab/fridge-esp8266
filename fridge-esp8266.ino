/* terrarium-esp8266.ino
 * This sketch drives my NodeMCU controller for my nook-based terrarium.
 */
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WifiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "NetworkCredentials.h"

#define ONE_WIRE_BUS D2

WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 28800;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();

  Serial.print("TEMPERATURE = ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print("*C");
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
}
