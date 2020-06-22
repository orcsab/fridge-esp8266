/* terrarium-esp8266.ino
 * This sketch drives my NodeMCU controller for my nook-based terrarium.
 */
//#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "sn.h"
#include "private.h"

#define ONE_WIRE_BUS D2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(NETSSID, NETPASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();

  Serial.print("TEMPERATURE = ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print("*C");
  Serial.println();

  SN::addMetric ("71a872a51b6990107d7bedf32a4bcbff", "u_fridge", "u_temp", sensors.getTempCByIndex(0));

  Serial.println("blinking");
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(10000);
}
