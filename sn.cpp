/* sn.cpp
 * ServiceNow functionality for Arduino.
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "sn.h"
#include "NetworkCredentials.h"

const char *SN::fingerprint = "960164aa4d4a99c6d42e5ef9f6f0678bbd1586b5";
const char *SN::host = "drummonds.service-now.com";
const int SN::httpsPort = 443;
const char *SN::url = "/api/now/v1/clotho/put?align_time";

void SN::init()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(HOMESSID, HOMEPASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/* addMetric
 * Adds a floating point metric to MetricBase.
 * sysId: the sysID of the IoT device.
 * table: the table holding all IoT devices
 * metric: the time series registered with MetricBase for that table
 * value: the value to store
 */
void SN::addMetric (const char *sysId, const char *table, const char *metric, float value)
{
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
                "User-Agent: BuildFailureDetectorESP8266\r\n" +
                "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }

  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
