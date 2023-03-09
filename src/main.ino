#include <WiFi.h>
#include <time.h>
#include <TimeAlarms.h>
#include "common.h"
#include "esp_sntp.h"
#include <Adafruit_NeoPixel.h>

// See https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
// for other timezones
#define TZ_Europe_Zurich	PSTR("CET-1CEST,M3.5.0,M10.5.0/3")

const char* ssid       = "CHANGEME";
const char* password   = "CHANGEME";

const char* ntpServer = "pool.ntp.org";

const int   led_pin = 4;
const int   bat_pin = 3;
const int   but_pin = 1;

Adafruit_NeoPixel pixels(1, led_pin, NEO_RGB + NEO_KHZ800);

// Handle the button press
//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0;

bool isr_alarm_disable = false;
bool is_alarming = false;

void IRAM_ATTR isr() {
  button_time = millis();
  if (button_time - last_button_time > 500) {
    // Set the alarm for disable if alarming
    if (is_alarming) {
      isr_alarm_disable = true;
    }

    last_button_time = button_time;
  }
}

void AlarmEnable() {
  is_alarming = true;
  Serial.print("Enabled the Alarm: ");
  printLocalTime();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  delay(1000);

  // Serial.print("Wakeup Time: ");
  // printLocalTime();

  // Start WiFi and connect
  wifi_connect(ssid, password);

  // init NTP and get the time
  configTzTime(TZ_Europe_Zurich, ntpServer);

  while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
    delay(1000); // Wait for NTP to get synchronized
  }
  setSyncProvider(getNTPTimestamp);

  Serial.print("Updated time: ");
  printLocalTime();

  // wifi_disconnect();

  // Setup RGB Pixels
  pixels.begin();

  // init Repeating Alarm
  Alarm.alarmRepeat(18,00,0, AlarmEnable);  // 18:00 every day
  
  // setup interrupt handling
  // disable the alarm once the button has been pressed
  pinMode(but_pin, INPUT);
	attachInterrupt(but_pin, isr, HIGH);
}

void loop() {
  //float batteryLevel = map(analogRead(bat_pin), 0.0f, 4095.0f, 0, 100);

  if (isr_alarm_disable) {
    for (int fadeOut = 255; fadeOut > 0; fadeOut--) {
      pixels.setPixelColor(0, pixels.Color(fadeOut, 0, 0));
      pixels.show();
      delay(10);
    }
    pixels.clear();
    pixels.show();
    Serial.print("Button pressed, disabled alarm: ");
    printLocalTime();
    isr_alarm_disable = false;
    is_alarming = false;


    // // Blink 10 times if the battery is low
    // if (batteryLevel <= 40) {
    //   for (int count = 0; count <= 9; count++) {
    //     pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    //     pixels.show();
    //     delay(500);
    //     pixels.clear();
    //     pixels.show();
    //     delay(500);
    //   }
    // }
  }

  Alarm.delay(1000);
  // Serial.println("Going to sleep for 3 minutes");
  // esp_sleep_enable_timer_wakeup(1000000 * 60); // 1 sec * 3600 = 1h
  // esp_deep_sleep_start();
}