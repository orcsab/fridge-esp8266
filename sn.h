/* sn.h
 */

namespace SN {
  void init();
  void addMetric (const char*, const char*, const char*, float);
  const char* buildMetricJson (const char*, const char*, const char*, float);

  extern const char *fingerprint;
  extern const char *host;
  extern const int httpsPort;
  extern const char *url;
  extern char jsonBuf[];
  const extern int jsonBufSize;
}
