# ESP32 Internet Radio dengan Layar OLED & Spectrum Analyzer (V2)

Proyek ini adalah versi lanjutan (V2) dari program internet radio berbasis ESP32 (dikonfigurasi untuk **ESP32-S3 Zero**). Pada versi ini, fitur utamanya adalah penambahan dukungan layar **OLED 128x64** (I2C) yang menampilkan informasi radio, status WiFi, animasi equalizer/spectrum analyzer, serta teks judul lagu yang berjalan (scrolling).

## Fitur Baru (Versi 2)
- **Layar OLED 128x64**: Menampilkan antarmuka pengguna yang informatif dan dinamis.
- **Spectrum Analyzer Visual**: Simulasi visualizer (equalizer) berupa bar spektrum animasi dengan efek *peak hold* berdasarkan energi audio.
- **Scrolling Text**: Judul lagu / metadata dari stasiun radio yang berjalan secara horizontal (scrolling) di layar jika teks terlalu panjang.
- **Indikator Visual**: Terdapat ikon sinyal WiFi (menunjukkan kekuatan sinyal/RSSI) dan ikon status pemutaran (animasi play/pause).
- Masih mempertahankan fitur memutar MP3 via I2S dan auto-reconnect WiFi dari versi sebelumnya.

## Perangkat Keras yang Dibutuhkan
- Board ESP32 (konfigurasi default untuk **ESP32-S3 Zero**).
- Modul DAC I2S (misalnya MAX98357A) atau amplifier pendukung input I2S + Speaker.
- **Layar OLED 128x64** (I2C) berbasis SSD1306.

## Konfigurasi Pin
**1. I2S Audio Output:**
- **BCLK** (Bit Clock): Pin 2
- **LRC** (Word Select): Pin 1
- **DOUT** (Data Out): Pin 3

**2. OLED Display (I2C):**
- **SDA** (Serial Data): Pin 8
- **SCL** (Serial Clock): Pin 9
*(Pin dapat diubah sesuai kebutuhan pada bagian konstanta `I2C_SDA` dan `I2C_SCL`)*

## Instalasi dan Kebutuhan Library
Pastikan Anda telah menginstal library-library berikut melalui **Library Manager** di Arduino IDE:
1. **ESP32-audioI2S** (oleh Schreibfaul1) - *untuk decoding audio.*
2. **Adafruit GFX Library** - *untuk grafis dasar layar OLED.*
3. **Adafruit SSD1306** - *driver khusus layar OLED.*
4. Library standar bawaan ESP32: `WiFi.h`, `Wire.h`.

## Cara Penggunaan
1. Buka file `esp32_radionet_v2.ino`.
2. Sesuaikan konfigurasi WiFi dengan jaringan Anda:
   ```cpp
   const char *ssid = "nama_ssid";
   const char *password = "pass_ssid";
   ```
3. Sesuaikan juga URL stream radio jika ingin mengubah stasiun:
   ```cpp
   const char *streamURL = "https://radio.isusay.my.id/listen/kara-fm/radio.mp3";
   ```
4. Rakit perangkat keras sesuai dengan *Konfigurasi Pin* di atas.
5. Upload program ke board ESP32 Anda.
6. Pada saat menyala, layar OLED akan menampilkan pesan *"Connecting WiFi"* beserta nama SSID. Setelah terhubung, layar akan masuk ke mode utama dan mulai memutar radio.

## Troubleshooting
- **Layar OLED gelap / "OLED GAGAL! Cek wiring" di Serial Monitor**: Pastikan pin I2C (SDA=8, SCL=9) sudah terhubung ke layar dengan benar dan tidak terbalik. Pastikan alamat I2C layar Anda `0x3C` (umumnya SSD1306 menggunakan alamat ini).
- **Restart terus-menerus (Boot loop)**: Cek suplai daya/power ESP32. Memutar audio dan menyalakan layar sekaligus bisa menarik arus (ampere) yang cukup besar, terutama jika menggunakan amplifier eksternal bertenaga dari board.
- **Suara tidak keluar atau visualizer bergerak tapi tidak ada suara**: Pastikan modul DAC (MAX98357A) terhubung dengan benar dan mendapatkan power yang memadai.

