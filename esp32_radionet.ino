#include <WiFi.h>
#include "Audio.h"

// ============================
// WIFI CONFIG
// ============================
const char* ssid     = "ssid_name";
const char* password = "pass_ssid";

// ============================
// STREAM URL
// ============================
const char* streamURL = "https://radio.isusay.my.id/listen/kara-fm/radio.mp3";

// ============================
// I2S PIN CONFIG (ESP32-S3 Zero)
// ============================
#define I2S_BCLK  2
#define I2S_LRC   1
#define I2S_DOUT  3

// ============================
// AUDIO OBJECT
// ============================
Audio audio;

void connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  connectWiFi();

  // Set pin I2S: BCLK, LRC, DOUT
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Volume 0 - 21
  audio.setVolume(15);

  // Optional: reconnect otomatis jika stream putus
  audio.setConnectionTimeout(5000, 5000);

  Serial.println("Memulai stream radio...");
  audio.connecttohost(streamURL);
}

void loop()
{
  audio.loop();

  // Jika WiFi putus, reconnect
  static unsigned long lastReconnectAttempt = 0;
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = millis();
      Serial.println("WiFi terputus, reconnect...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }
}

// ============================
// CALLBACK INFO
// ============================
void audio_info(const char *info) {
  Serial.print("info        : ");
  Serial.println(info);
}

void audio_id3data(const char *info) {
  Serial.print("id3data     : ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info) {
  Serial.print("eof_mp3     : ");
  Serial.println(info);
}

void audio_showstation(const char *info) {
  Serial.print("station     : ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info) {
  Serial.print("title       : ");
  Serial.println(info);
}

void audio_bitrate(const char *info) {
  Serial.print("bitrate     : ");
  Serial.println(info);
}

void audio_commercial(const char *info) {
  Serial.print("commercial  : ");
  Serial.println(info);
}

void audio_icyurl(const char *info) {
  Serial.print("icyurl      : ");
  Serial.println(info);
}

void audio_lasthost(const char *info) {
  Serial.print("lasthost    : ");
  Serial.println(info);
}

void audio_eof_speech(const char *info) {
  Serial.print("eof_speech  : ");
  Serial.println(info);
}
