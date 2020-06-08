/* sn.cpp
 * ServiceNow functionality for Arduino.
 */
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WifiUdp.h>

#include "sn.h"
#include "private.h"

const char *SN::fingerprint = "960164aa4d4a99c6d42e5ef9f6f0678bbd1586b5";
const char *SN::host = "drummonds.service-now.com";
const int SN::httpsPort = 443;
const char *SN::url = "/api/now/v1/clotho/put?align_time";
const int SN::jsonBufSize = 512;
char SN::jsonBuf[SN::jsonBufSize];

WiFiUDP ntpUDP;
// const long utcOffsetInSeconds = 28800; // Singapore
const long utcOffsetInSeconds = 0; // GMT 
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

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

  const char *json = buildMetricJson(sysId, table, metric, value);

  String post;
  post = String("POST ") + url + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "User-Agent: fridge-esp8266\r\n" +
          "Content-Type: application/json\r\n" +
          "Accepts: application/json\r\n" +
          "Authorization: Basic " + CREDBASE64 + "\r\n" +
          "\r\n" +
          json +
          "Connection: close\r\n\r\n";  // is this needed??

  Serial.println("sending request:");
  Serial.println(post);

  client.print(post);
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
}

/* buildMetricJson
 * Build JSON string to send to the instance's time series.
 * sysId: the sys_id of the devices
 * table: the table in which that sys_id is stored
 * metric: the time series created in MetricBase
 * value: a floating point value to store in the time series
 */
const char *SN::buildMetricJson (const char *sysId,
                                 const char *table,
                                 const char *metric,
                                 float value)
{
  timeClient.update();
  String timestamp = "2020-06-08T";
  timestamp += timeClient.getHours();
  timestamp += ":";
  timestamp += timeClient.getMinutes();
  timestamp += ":";
  timestamp += timeClient.getSeconds();
  timestamp += "Z";

  String json ("{\r\n");
  json += "\"seriesRef\": {\r\n";
  json += "\"subject\": \"" + String(sysId) + "\",\r\n";
  json += "\"table\": \"" + String(table) + "\",\r\n";
  json += "\"metric\": \"" + String(metric) + "\"\r\n";
  json += "},\r\n";
  json += "\"values\":  [\r\n";
  json += "{\r\n";
  json += "\"timestamp\": \"" + timestamp + "\",\r\n";
  json += "\"value\": " + String(value) + "\r\n";
  json += "}\r\n";
  json += "]\r\n";
  json += "}\r\n";

  strcpy (jsonBuf, json.c_str());
  return jsonBuf;
}
