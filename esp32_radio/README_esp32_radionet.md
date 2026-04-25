# ESP32 Internet Radio Streamer

Proyek ini adalah program internet radio berbasis ESP32 (khususnya dikonfigurasi untuk board **ESP32-S3 Zero**) yang memutar aliran audio MP3 dari internet menggunakan protokol I2S untuk output audio. Program ini memanfaatkan library `ESP32-audioI2S` untuk memproses dan mendecode aliran audio.

## Fitur
- Memutar streaming radio internet (MP3) secara langsung.
- Output audio digital menggunakan antarmuka I2S.
- Auto-reconnect jika koneksi WiFi terputus.
- Menampilkan metadata stream (seperti nama stasiun, judul lagu, bitrate, dll.) melalui Serial Monitor.

## Perangkat Keras yang Dibutuhkan
- Board ESP32 (konfigurasi default untuk **ESP32-S3 Zero**).
- Modul DAC I2S (misalnya MAX98357A) atau amplifier yang mendukung input I2S.
- Speaker.

## Konfigurasi Pin I2S
Secara default, pin I2S dikonfigurasi sebagai berikut (dapat diubah pada kode `audio2.ino`):
- **BCLK** (Bit Clock): Pin 2
- **LRC** (Left/Right Clock / Word Select): Pin 1
- **DOUT** (Data Out): Pin 3

## Instalasi dan Kebutuhan Library
1. Pastikan Anda telah menginstal dukungan board ESP32 di Arduino IDE.
2. Anda membutuhkan library **ESP32-audioI2S** (oleh Schreibfaul1). Anda dapat mengunduh atau menginstalnya dari [GitHub ESP32-audioI2S](https://github.com/schreibfaul1/ESP32-audioI2S).
3. Buka file `audio2.ino` di Arduino IDE.

## Cara Penggunaan
1. Buka file `audio2.ino`.
2. Ubah konfigurasi WiFi dengan memasukkan SSID dan Password jaringan Anda:
   ```cpp
   const char* ssid     = "ssid_name";
   const char* password = "pass_ssid";
   ```
3. (Opsional) Jika Anda ingin mengganti stasiun radio, ubah URL stream pada variabel `streamURL`:
   ```cpp
   const char* streamURL = "https://radio.isusay.my.id/listen/kara-fm/radio.mp3";
   ```
4. Hubungkan modul I2S ke ESP32 sesuai dengan pin yang dikonfigurasi.
5. Upload kode ke board ESP32 Anda.
6. Buka **Serial Monitor** (baud rate 115200) untuk melihat status koneksi WiFi, proses streaming, dan metadata audio (seperti judul lagu atau nama stasiun).

## Troubleshooting
- **Tidak ada suara / Serial Monitor tidak menampilkan "Memulai stream radio..."**: Pastikan ESP32 berhasil terhubung ke WiFi. Periksa kredensial WiFi.
- **Suara putus-putus**: Pastikan koneksi internet stabil. Program memiliki fitur reconnect otomatis, namun koneksi yang buruk akan menyebabkan buffering.
- **Error saat compile**: Pastikan library `Audio.h` (ESP32-audioI2S) sudah terinstal dengan benar.

