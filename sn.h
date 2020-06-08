/* sn.h
 */

namespace SN {
  void init();
  void addMetric (const char*, const char*, const char*, float);
  const char* buildJson (const char *, const char *, const char *);

  extern const char *fingerprint;
  extern const char *host;
  extern const int httpsPort;
  extern const char *url;
}
