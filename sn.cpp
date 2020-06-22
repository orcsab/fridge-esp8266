/* sn.cpp
 * ServiceNow functionality for Arduino.
 */
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WifiUdp.h>
#include <sstream>
#include <time.h>

#include "sn.h"
#include "private.h"

const char *SN::fingerprint = "960164aa4d4a99c6d42e5ef9f6f0678bbd1586b5";
const char *SN::host = "drummonds.service-now.com";
const int SN::httpsPort = 443;
const char *SN::url = "/api/now/v1/clotho/put";
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
  char length[8];
  sprintf (length, "%d", strlen(json));

  String post;
  post = String("POST ") + url + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Authorization: Basic " + CREDBASE64 + "\r\n" +
          "User-Agent: fridge-esp8266\r\n" +
//          "Accept: */*\r\n" +
          "Content-Type: application/json\r\n" +
          "Content-Length: " + length + "\r\n" +
          "\r\n" +
          json;

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


  char time[24];

//  sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d",
//          2020, 6, 22, timeClient.getHours(),
//          timeClient.getMinutes(), timeClient.getSeconds());

  time_t now = timeClient.getEpochTime();
  struct tm *ts;
  ts = localtime(&now);
  sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02d",
          ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, timeClient.getHours(),
          timeClient.getMinutes(), timeClient.getSeconds());


  String json ("{\r\n");
  json += "\"seriesRef\": {\r\n";
  json += "\"subject\": \"" + String(sysId) + "\",\r\n";
  json += "\"table\": \"" + String(table) + "\",\r\n";
  json += "\"metric\": \"" + String(metric) + "\"\r\n";
  json += "},\r\n";
  json += "\"values\":  [\r\n";
  json += "{\r\n";
  json += "\"timestamp\": \"" + String(time) + "\",\r\n";
  json += "\"value\": " + String(value) + "\r\n";
  json += "}\r\n";
  json += "]\r\n";
  json += "}\r\n";

  strcpy (jsonBuf, json.c_str());
  return jsonBuf;
}
