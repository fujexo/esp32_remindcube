#include <WiFi.h>

void wifi_connect(const char* ssid, const char* password) {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

void wifi_disconnect() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

/* Time related functions */
// Return a unix epoch for syncing with Timelib
// Use with `setSyncProvider(getNTPTimestamp);`
time_t getNTPTimestamp() {
  time_t now;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 0;
  }

  time(&now);
  
  // TODO BEWARE TIMEZONE HACK RIGHT HERE
  // unix timestamp seems to be in UTC so I'm adding an hour for Swiss time. Also adding an extra hour if the daylight saving time flag is set
  now += 3600;
  if (timeinfo.tm_isdst > 0) {
    now += 3600;
  }
  
  return now;
}

// Print the current local time
void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}