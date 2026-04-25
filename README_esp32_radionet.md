📻 ESP32-S3 Internet Radio
Streaming radio online menggunakan ESP32-S3 dengan output audio I2S ke MAX98357A.

ESP32LibraryLicense

📋 Daftar Isi
Gambaran Proyek
Fitur
Komponen yang Dibutuhkan
Wiring Diagram
Instalasi
Konfigurasi
Cara Penggunaan
Callback Reference
Troubleshooting
Struktur Folder
License
🎯 Gambaran Proyek
Proyek ini membuat Internet Radio sederhana menggunakan:

ESP32-S3 sebagai mikrokontroler utama
WiFi untuk koneksi internet
I2S untuk output audio digital
MAX98357A sebagai DAC + Amplifier
┌─────────────┐ WiFi ┌─────────────┐ I2S ┌─────────────┐
│ │ ◄───────────► │ │ ───────────► │ │
│ Internet │ │ ESP32-S3 │ │ MAX98357A │
│ (Server │ │ │ │ (DAC+Amp) │
│ Radio) │ └─────────────┘ └──────┬──────┘
└─────────────┘ │
▼
┌─────────────┐
│ Speaker │
│ 3W 4Ω │
└─────────────┘

text


---

## ✨ Fitur

| Fitur | Status |
|-------|--------|
| Streaming MP3 dari URL | ✅ |
| Auto reconnect WiFi | ✅ |
| Auto reconnect stream | ✅ |
| Volume control (software) | ✅ |
| Metadata display (judul, station) | ✅ |
| Bitrate info | ✅ |
| ICY metadata support | ✅ |

---

## 🛒 Komponen yang Dibutuhkan

| No | Komponen | Jumlah | Keterangan |
|----|----------|--------|------------|
| 1 | ESP32-S3 Zero | 1 | Board utama |
| 2 | MAX98357A | 1 | Modul DAC + Amplifier I2S |
| 3 | Speaker | 1 | 3W, 4Ω |
| 4 | Kabel Jumper | 5 | Female-to-Female |
| 5 | Power Supply | 1 | 5V USB |

---

## 🔌 Wiring Diagram

### Skema Koneksi

┌──────────────────────────────────────────────────────────────┐
│ │
│ ┌─────────────────┐ │
│ │ ESP32-S3 │ │
│ │ Zero │ │
│ │ │ │
│ 3V3──┤ VIN │ │
│ GND──┤ GND 3V3├──┐ │
│ │ │ │ │
│ │ GPIO1├──┼──────── LRC │
│ │ GPIO2├──┼──────── BCLK │
│ │ GPIO3├──┼──────── DOUT │
│ │ │ │ │
│ └─────────────────┘ │ │
│ │ │
│ ┌─────────────────┐ │ │
│ │ MAX98357A │ │ │
│ │ │ │ │
│ 5V───┤ VIN │ │ │
│ GND───┤ GND │ │ │
│ │ │ │ │
│ │ LRC──┼──┘ │
│ │ BCLK──┼──┘ │
│ │ DIN───┼──┘ │
│ │ │ │
│ │ SD──┼── Ke 3V3 (Aktif) │
│ │ GAIN──┼── Ke GND (15dB) │
│ │ │ │
│ └────────┬────────┘ │
│ │ │
│ ┌──┴──┐ │
│ │ + - │ │
│ │Spk │ │
│ └─────┘ │
│ │
└──────────────────────────────────────────────────────────────┘

text


### Tabel Pin

| ESP32-S3 Zero | MAX98357A | Keterangan |
|---------------|-----------|------------|
| GPIO 2 | BCLK | Bit Clock I2S |
| GPIO 1 | LRC | Left/Right Channel |
| GPIO 3 | DIN | Data Audio I2S |
| 3V3 | SD | Shutdown (HIGH = ON) |
| 5V | VIN | Power MAX98357A |
| GND | GND | Ground |

> ⚠️ **Catatan:** 
> - Pin SD harus HIGH untuk mengaktifkan MAX98357A
> - GAIN ke GND = penguatan 15dB (rekomendasi untuk speaker kecil)

---

## 📦 Instalasi

### 1. Install Arduino IDE

Download dari [arduino.cc](https://www.arduino.cc/en/software)

### 2. Tambahkan Board ESP32

File → Preferences → Additional Board Manager URLs:

https://espressif.github.io/arduino-esp32/package_esp32_index.json

text


Kemudian install board:
Tools → Board → Boards Manager → Cari "esp32" → Install

text


### 3. Install Library

Buka **Library Manager**:
Tools → Manage Libraries

text


Cari dan install:

| Library | Author | Versi |
|---------|--------|-------|
| ESP8266Audio | Earle F. Philhower, III | Latest |

> 💡 **Alternative:** Jika library di atas bermasalah di ESP32-S3, gunakan fork khusus:
> ```
> https://github.com/earlephilhower/ESP8266Audio
> ```

### 4. Pilih Board

Tools → Board → ESP32 Arduino → ESP32S3 Dev Module

Settings:
├── USB CDC On Boot: Enabled
├── CPU Frequency: 240MHz
├── Flash Mode: QIO or QOUT
├── Flash Size: 4MB
├── Partition Scheme: Default 4MB with spiffs
├── PSRAM: Disabled (atau OPI PSRAM jika ada)
└── Upload Speed: 921600

text


### 5. Upload Kode

Hubungkan ESP32-S3 ke komputer
Pilih port yang benar
Klik Upload
Buka Serial Monitor (115200 baud)
text


---

## ⚙️ Konfigurasi

### WiFi

Ubah sesuai jaringan Anda:

```cpp
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
Stream URL
Ganti dengan URL radio favorit Anda:

cpp

const char* streamURL = "https://URL_RADIO/radio.mp3";
Daftar Radio Streaming Indonesia
Radio
URL
Kara FM	https://radio.isusay.my.id/listen/kara-fm/radio.mp3
Prambors	https://streaming.pramborsfm.com/prambors_jakarta
Radio Elshinta	http://live.elshinta.com:8000/elshinta
Jak FM	http://jak-fm.mediascorp.co.id/jakfm
ZETA FM	http://streaming.zetafm.com/zeta

Daftar Radio Internasional
Radio
URL
BBC World Service	http://stream.live.vc.bbcmedia.co.uk/bbc_world_service
Jazz FM	https://jazz-wr01.ice.infomaniak.ch/jazz-wr01-128.mp3
Classic FM	https://media-ice.musicradio.com/ClassicFMMP3

Volume
Atur volume antara 0 (mute) sampai 21 (maksimal):

cpp

audio.setVolume(15);  // 0 - 21
Pin I2S
Sesuaikan jika menggunakan pin berbeda:

cpp

#define I2S_BCLK  2
#define I2S_LRC   1
#define I2S_DOUT  3
🚀 Cara Penggunaan
Alur Kerja
text

1. Hubungkan hardware sesuai wiring
2. Upload kode ke ESP32-S3
3. Buka Serial Monitor (115200 baud)
4. Tunggu koneksi WiFi
5. Radio akan mulai streaming otomatis
6. Lihat info di Serial Monitor
Output Serial Monitor
text

Menghubungkan ke WiFi.......
WiFi terhubung
IP Address: 192.168.1.100
Memulai stream radio...
info        : Connect to "https://radio.isusay.my.id/listen/kara-fm/radio.mp3"
info        : ICY name="Kara FM"
info        : ICY genre="Various"
info        : ICY metabr=16000
info        : ICY metaint=16000
info        : sr=44100, ch=2, bits=16
bitrate     : 128 kbps
station     : Kara FM
title       : Lagu Sedang Diputar - Artis
📡 Callback Reference
Program ini menggunakan callback untuk menampilkan informasi streaming:

Callback
Dipanggil Saat
Contoh Output
audio_info()	Status umum	sr=44100, ch=2, bits=16
audio_showstation()	Nama station diterima	Kara FM
audio_showstreamtitle()	Judul lagu berubah	Artist - Title
audio_bitrate()	Info bitrate	128 kbps
audio_id3data()	Data ID3 tag	ID3 data...
audio_eof_mp3()	File MP3 selesai	End of MP3
audio_icyurl()	URL ICY diterima	https://...
audio_commercial()	Deteksi commercial	Commercial
audio_lasthost()	Host terakhir	server.com
audio_eof_speech()	Speech end	End of speech

Contoh: Menampilkan Judul Lagu di OLED
cpp

#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void audio_showstreamtitle(const char *info) {
  Serial.print("title       : ");
  Serial.println(info);
  
  // Tampilkan di OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Now Playing:");
  display.println(info);
  display.display();
}
🔧 Troubleshooting
❌ Tidak Ada Suara
Masalah
Solusi
Speaker tidak terhubung	Cek koneksi speaker ke MAX98357A
Pin salah	Pastikan BCLK, LRC, DOUT benar
SD tidak HIGH	Hubungkan pin SD ke 3V3
Volume 0	Set volume antara 1-21
URL mati	Coba URL radio lain

❌ WiFi Tidak Terhubung
Masalah
Solusi
SSID/Password salah	Cek kembali kredensial WiFi
Jarak terlalu jauh	Dekatkan ke router
2.4GHz only	ESP32 hanya support WiFi 2.4GHz
Hidden SSID	ESP32 sulit konek ke hidden SSID

❌ Stream Tidak Putar
Masalah
Solusi
URL salah	Cek URL di browser dulu
HTTPS problem	Coba ganti ke HTTP
Server down	Coba radio/station lain
Buffer terlalu kecil	Tambah timeout connection

❌ Suara Patah-patah
Masalah
Solusi
WiFi lemah	Dekatkan ke router
Clock tidak stabil	Gunakan pin I2S yang direkomendasikan
Power tidak cukup	Gunakan power supply 5V yang stabil
Speaker terlalu besar	Gunakan speaker 3W 4Ω

❌ Compile Error
text

Error: 'Audio' was not declared

Solusi:
1. Pastikan library ESP8266Audio sudah terinstall
2. Cek versi library (update ke terbaru)
3. Restart Arduino IDE
📁 Struktur Folder
text

esp32-internet-radio/
│
├── esp32-internet-radio.ino    ← File utama
├── README.md                   ← Dokumentasi ini
│
└── docs/
    ├── wiring-diagram.png      ← Gambar wiring
    └── schematic.pdf           ← Skema lengkap
📝 Changelog
v1.0.0 - 2024
✅ Streaming radio MP3
✅ Auto reconnect WiFi
✅ Auto reconnect stream
✅ Callback metadata
✅ Support ESP32-S3 Zero
🤝 Kontribusi
Kontribusi terbuka! Silakan:

Fork repository ini
Buat branch fitur baru
Commit perubahan
Push ke branch
Buat Pull Request
📜 License
Proyek ini dilisensikan under MIT License.

text

MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
🙏 Credits
Earle F. Philhower - Library Audio
Espressif - ESP32-S3
Maxim Integrated - MAX98357A

