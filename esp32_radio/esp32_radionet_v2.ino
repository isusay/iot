#include <WiFi.h>
#include "Audio.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================
// WIFI CONFIG
// ============================
const char* ssid     = "nama_ssid";
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
// OLED CONFIG
// ============================
#define I2C_SDA       8
#define I2C_SCL       9
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDR     0x3C
#define OLED_RESET    -1

// ============================
// SPECTRUM CONFIG
// ============================
#define NUM_BANDS       16
#define BAR_WIDTH       5
#define BAR_GAP         3
#define SPECTRUM_HEIGHT 32
#define SPECTRUM_Y_BASE 61
#define PEAK_HOLD_TIME  15
#define PEAK_FALL_SPEED 1

// ============================
// CONFIGURATION
// ============================
#define VOLUME          15      // 0-21
#define DISPLAY_FPS     30      // frames per second

// ============================
// OBJECTS
// ============================
Audio audio;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ============================
// SPECTRUM DATA
// ============================
int bandValues[NUM_BANDS] = {0};
int peakValues[NUM_BANDS] = {0};
int peakTimer[NUM_BANDS] = {0};
float bandTarget[NUM_BANDS] = {0};

// Simulated audio energy tracking
float audioEnergy = 0;
float beatEnergy = 0;
unsigned long lastBeatTime = 0;
bool beatDetected = false;

// ============================
// STATE VARIABLES
// ============================
String stationName = "";
String streamTitle = "";
unsigned long lastDisplayUpdate = 0;
unsigned long lastStreamCheck = 0;
unsigned long lastWifiReconnect = 0;
unsigned long lastTitleScroll = 0;
int titleScrollPos = 0;
bool streamStarted = false;

// ============================
// NOISE GENERATOR (untuk smooth random)
// ============================
float smoothNoise(int x, unsigned long t) {
  float n = sin(x * 12.9898 + t * 0.001) * 43758.5453;
  n = n - floor(n);
  return n;
}

// ============================
// I2C INIT dengan custom pins
// ============================
void initI2C() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C initialized on SDA=8, SCL=9");
}

// ============================
// WIFI CONNECTION
// ============================
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  int timeout = 30;
  
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    Serial.print(".");
    delay(500);
    timeout--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi terhubung");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nWiFi gagal terhubung!");
  }
}

// ============================
// OLED INIT
// ============================
void initDisplay() {
  initI2C();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED GAGAL! Cek wiring.");
    while (true) {
      delay(1000);
    }
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 28);
  display.println("KARA FM");
  display.setCursor(15, 40);
  display.println("Radio Player");
  display.display();
  delay(1500);
  
  Serial.println("OLED initialized");
}

// ============================
// SPECTRUM GENERATOR
// ============================
void updateSpectrum() {
  unsigned long t = millis();
  
  if (audio.isRunning()) {
    // Generate realistic spectrum shape
    // Bass (low freq) = higher energy, Treble (high freq) = lower energy
    
    // Simulated beat detection
    float instantEnergy = smoothNoise(0, t) * 0.5 + smoothNoise(100, t) * 0.5;
    
    // Detect "beat" - sudden energy spike
    if (instantEnergy > beatEnergy + 0.15 && t - lastBeatTime > 200) {
      beatDetected = true;
      beatEnergy = instantEnergy;
      lastBeatTime = t;
    } else {
      beatDetected = false;
      beatEnergy *= 0.95;
    }
    
    audioEnergy = audioEnergy * 0.8 + instantEnergy * 0.2;
    
    for (int i = 0; i < NUM_BANDS; i++) {
      // Base level based on frequency (lower = louder)
      float baseLevel = map(i, 0, NUM_BANDS - 1, 70, 30) / 100.0;
      
      // Add noise variation
      float noise1 = smoothNoise(i * 7, t / 50);
      float noise2 = smoothNoise(i * 13 + 50, t / 80);
      float noise3 = smoothNoise(i * 23 + 100, t / 30);
      
      // Combine noises
      float variation = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
      
      // Adjacent band correlation (smooth spectrum shape)
      if (i > 0) {
        variation = variation * 0.7 + smoothNoise((i-1) * 7, t / 50) * 0.3;
      }
      
      // Calculate target height
      float target = baseLevel + variation * (1.0 - baseLevel) * 0.8;
      
      // Beat boost (bass frequencies get more boost)
      if (beatDetected) {
        float beatBoost = map(i, 0, NUM_BANDS - 1, 0.4, 0.1) / 1.0;
        target += beatBoost;
      }
      
      // Random sparkle
      if (random(100) < 3) {
        target += random(10, 30) / 100.0;
      }
      
      // Clamp
      target = constrain(target, 0.05, 1.0);
      
      // Convert to pixel height
      bandTarget[i] = target * SPECTRUM_HEIGHT;
      
      // Smooth interpolation
      bandValues[i] = bandValues[i] * 0.55 + bandTarget[i] * 0.45;
      
      // Peak handling
      if (bandValues[i] >= peakValues[i]) {
        peakValues[i] = bandValues[i];
        peakTimer[i] = PEAK_HOLD_TIME;
      } else {
        peakTimer[i]--;
        if (peakTimer[i] <= 0) {
          peakValues[i] -= PEAK_FALL_SPEED;
        }
      }
    }
    
  } else {
    // Decay when not playing
    for (int i = 0; i < NUM_BANDS; i++) {
      bandValues[i] *= 0.92;
      peakValues[i] *= 0.95;
      if (bandValues[i] < 1) bandValues[i] = 0;
      if (peakValues[i] < 1) peakValues[i] = 0;
    }
    audioEnergy *= 0.95;
  }
}

// ============================
// DRAW SPECTRUM BARS
// ============================
void drawSpectrum() {
  int totalWidth = NUM_BANDS * (BAR_WIDTH + BAR_GAP) - BAR_GAP;
  int startX = (SCREEN_WIDTH - totalWidth) / 2;
  
  for (int i = 0; i < NUM_BANDS; i++) {
    int x = startX + i * (BAR_WIDTH + BAR_GAP);
    int barHeight = (int)bandValues[i];
    
    if (barHeight > 0) {
      // Draw bar with gradient effect
      for (int y = 0; y < barHeight && y < SPECTRUM_HEIGHT; y++) {
        int drawY = SPECTRUM_Y_BASE - y;
        
        // Gradient: bottom dimmer, middle bright, top bright
        if (y < barHeight * 0.3) {
          // Bottom third - dimmer
          display.fillRect(x, drawY, BAR_WIDTH, 1, SSD1306_WHITE);
        } else if (y < barHeight * 0.7) {
          // Middle - full
          display.fillRect(x, drawY, BAR_WIDTH, 1, SSD1306_WHITE);
        } else {
          // Top - full with potential highlight
          display.fillRect(x, drawY, BAR_WIDTH, 1, SSD1306_WHITE);
        }
      }
      
      // Draw peak indicator
      if (peakValues[i] > 2) {
        int peakY = SPECTRUM_Y_BASE - (int)peakValues[i];
        display.fillRect(x - 1, peakY, BAR_WIDTH + 2, 2, SSD1306_WHITE);
      }
    }
  }
}

// ============================
// DRAW WIFI ICON
// ============================
void drawWifiIcon(int x, int y) {
  if (WiFi.status() == WL_CONNECTED) {
    // Signal bars
    int rssi = WiFi.RSSI();
    int bars = 0;
    if (rssi > -50) bars = 4;
    else if (rssi > -60) bars = 3;
    else if (rssi > -70) bars = 2;
    else bars = 1;
    
    for (int i = 0; i < bars; i++) {
      int barH = 3 + i * 2;
      display.fillRect(x + i * 4, y + 8 - barH, 2, barH, SSD1306_WHITE);
    }
  } else {
    // X mark for no WiFi
    display.drawLine(x, y, x + 12, y + 8, SSD1306_WHITE);
    display.drawLine(x + 12, y, x, y + 8, SSD1306_WHITE);
  }
}

// ============================
// DRAW PLAYING INDICATOR
// ============================
void drawPlayingIcon(int x, int y) {
  if (audio.isRunning()) {
    // Animated equalizer bars
    static int animFrame = 0;
    if (millis() % 200 < 100) animFrame = (animFrame + 1) % 3;
    
    int heights[] = {4, 7, 3, 6, 5};
    for (int i = 0; i < 3; i++) {
      int h = heights[(animFrame + i) % 5];
      display.fillRect(x + i * 3, y + 8 - h, 2, h, SSD1306_WHITE);
    }
  } else {
    // Pause icon
    display.fillRect(x, y + 2, 2, 6, SSD1306_WHITE);
    display.fillRect(x + 4, y + 2, 2, 6, SSD1306_WHITE);
  }
}

// ============================
// MAIN DISPLAY UPDATE
// ============================
void drawDisplay() {
  display.clearDisplay();
  
  // ============ TOP SECTION (Header) ============
  // Station name
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if (stationName.length() > 0) {
    String name = stationName.substring(0, 8);
    display.print(name);
  } else {
    display.print("KARA FM");
  }
  
  // Playing indicator
  drawPlayingIcon(55, 0);
  
  // WiFi icon
  drawWifiIcon(SCREEN_WIDTH - 18, 0);
  
  // Separator line
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);
  
  // ============ MIDDLE SECTION (Title) ============
  display.setCursor(0, 13);
  
  if (streamTitle.length() > 0) {
    // Scrolling title
    if (millis() - lastTitleScroll > 150) {
      lastTitleScroll = millis();
      titleScrollPos++;
      int maxScroll = streamTitle.length() + 5;
      if (titleScrollPos > maxScroll) {
        titleScrollPos = -21;  // Width for ~21 chars
      }
    }
    
    String displayText;
    int startPos = max(0, titleScrollPos);
    int available = 21 - min(21, max(0, -titleScrollPos));
    
    if (titleScrollPos < 0) {
      // Pad with spaces at start
      String padding = "                     ";
      int padLen = -titleScrollPos;
      displayText = padding.substring(0, min(padLen, 21));
      displayText += streamTitle.substring(0, 21 - displayText.length());
    } else {
      displayText = streamTitle.substring(startPos, startPos + 21);
      // Pad end if needed
      while (displayText.length() < 21) {
        displayText += " ";
      }
    }
    
    display.println(displayText.substring(0, 21));
  } else {
    if (audio.isRunning()) {
      display.println("  >> Streaming...      ");
    } else if (WiFi.status() == WL_CONNECTED) {
      display.println("  Buffering...         ");
    } else {
      display.println("  Connecting WiFi...   ");
    }
  }
  
  // Second line for additional info or visualizer label
  display.setCursor(0, 22);
  display.print("Vol:");
  display.print(VOLUME);
  display.print(" RSSI:");
  if (WiFi.status() == WL_CONNECTED) {
    display.print(WiFi.RSSI());
  } else {
    display.print("--");
  }
  
  // Separator line
  display.drawFastHLine(0, 31, SCREEN_WIDTH, SSD1306_WHITE);
  
  // ============ BOTTOM SECTION (Spectrum) ============
  // Spectrum area: y=32 to y=61 (29 pixels height for bars)
  drawSpectrum();
  
  // Base line
  display.drawFastHLine(0, SPECTRUM_Y_BASE, SCREEN_WIDTH, SSD1306_WHITE);
  
  display.display();
}

// ============================
// SETUP
// ============================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("   ESP32-S3 Internet Radio");
  Serial.println("   with OLED Spectrum Analyzer");
  Serial.println("========================================");

  // Init OLED first (show splash)
  initDisplay();
  
  // Show connecting message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Connecting WiFi");
  display.setCursor(10, 40);
  display.print("SSID: ");
  display.println(ssid);
  display.display();

  connectWiFi();
  
  // Show WiFi status
  display.clearDisplay();
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(10, 25);
    display.println("WiFi Connected!");
    display.setCursor(10, 40);
    display.print("IP: ");
    display.println(WiFi.localIP());
  } else {
    display.setCursor(10, 25);
    display.println("WiFi Failed!");
  }
  display.display();
  delay(1500);
  
  // Init audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(VOLUME);
  audio.setConnectionTimeout(5000, 5000);
  
  Serial.println("Memulai stream radio...");
  Serial.print("URL: ");
  Serial.println(streamURL);
  
  audio.connecttohost(streamURL);
  streamStarted = true;
}

// ============================
// LOOP
// ============================
void loop() {
  audio.loop();
  
  // WiFi reconnect
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastWifiReconnect > 5000) {
      lastWifiReconnect = millis();
      Serial.println("WiFi terputus, reconnect...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }
  
  // Stream reconnect
  if (millis() - lastStreamCheck > 10000) {
    lastStreamCheck = millis();
    if (WiFi.status() == WL_CONNECTED && streamStarted && !audio.isRunning()) {
      Serial.println("Stream terputus, reconnect...");
      audio.connecttohost(streamURL);
    }
  }
  
  // Update display at specified FPS
  unsigned long frameTime = 1000 / DISPLAY_FPS;
  if (millis() - lastDisplayUpdate > frameTime) {
    lastDisplayUpdate = millis();
    updateSpectrum();
    drawDisplay();
  }
}

// ============================
// AUDIO CALLBACKS
// ============================
void audio_info(const char *info) {
  Serial.print("[INFO]    ");
  Serial.println(info);
}

void audio_id3data(const char *info) {
  Serial.print("[ID3]     ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info) {
  Serial.print("[EOF]     ");
  Serial.println(info);
  if (WiFi.status() == WL_CONNECTED) {
    delay(1000);
    audio.connecttohost(streamURL);
  }
}

void audio_showstation(const char *info) {
  Serial.print("[STATION] ");
  Serial.println(info);
  stationName = String(info);
}

void audio_showstreamtitle(const char *info) {
  Serial.print("[TITLE]   ");
  Serial.println(info);
  streamTitle = String(info);
  titleScrollPos = 0;  // Reset scroll on new title
}

void audio_bitrate(const char *info) {
  Serial.print("[BITRATE] ");
  Serial.println(info);
}

void audio_commercial(const char *info) {
  Serial.print("[AD]      ");
  Serial.println(info);
}

void audio_icyurl(const char *info) {
  Serial.print("[ICY]     ");
  Serial.println(info);
}

void audio_lasthost(const char *info) {
  Serial.print("[HOST]    ");
  Serial.println(info);
}

void audio_eof_speech(const char *info) {
  Serial.print("[SPEECH]  ");
  Serial.println(info);
}
