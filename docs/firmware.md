# 🛠️ BreezeMate — Panduan Flash & Setup Firmware ESP32

Panduan langkah demi langkah memprogram ESP32 menggunakan **Arduino IDE** agar terhubung ke Wi-Fi rumah dan bisa dikontrol via browser HP.

---

## 1. Persiapan Software & Library

### A. Install Arduino IDE
Pastikan komputermu sudah terpasang [Arduino IDE](https://www.arduino.cc/en/software) (versi 2.x direkomendasikan).

### B. Tambahkan Board ESP32 ke Arduino IDE
1. Buka Arduino IDE -> **File** -> **Preferences**.
2. Pada kolom **Additional Boards Manager URLs**, tempelkan tautan berikut:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Buka menu **Tools** -> **Board** -> **Boards Manager...**.
4. Cari `esp32` oleh **Espressif Systems**, lalu klik **Install**.

### C. Install Library Sensor
Buka menu **Tools** -> **Manage Libraries...**, cari dan install library:
1. `DHT sensor library` oleh **Adafruit** (pilih *Install All* jika meminta dependensi `Adafruit Unified Sensor`).
> *Catatan: Library `WiFi.h`, `WebServer.h`, dan `ESPmDNS.h` sudah otomatis terpasang sebagai bagian dari core board ESP32.*

---

## 2. Konfigurasi Kode Wi-Fi

1. Buka file firmware utama:
   ```text
   firmware/BreezeMate/BreezeMate.ino
   ```
2. Temukan baris konfigurasi Wi-Fi di bagian atas file (baris 25–26):
   ```cpp
   const char* WIFI_SSID     = "NAMA_WIFI_RUMAHMU";   // Ganti dengan nama Wi-Fi rumahmu
   const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";  // Ganti dengan password Wi-Fi rumahmu
   ```
3. Simpan perubahan file (`Ctrl + S`).

---

## 3. Proses Flashing ke ESP32

1. Hubungkan modul ESP32 ke laptop/PC menggunakan kabel data Micro-USB atau USB-C.
2. Di Arduino IDE:
   - Pilih Board: **Tools** -> **Board** -> **ESP32 Arduino** -> **ESP32 Dev Module** (atau tipe board ESP32 yang kamu pakai).
   - Pilih Port: **Tools** -> **Port** -> Pilih port COM ESP32 (contoh: `COM3`, `COM5`).
   - Upload Speed: `921600` (atau `115200` jika gagal upload).
3. Klik tombol **Upload** (ikon panah ke kanan `→`).
4. Tunggu hingga proses kompilasi dan upload selesai (`Done uploading`).
   *(Jika muncul pesan `Connecting......`, tekan dan tahan tombol **BOOT** di modul ESP32 selama 2 detik hingga proses flash berjalan).*

---

## 4. Membuka Dashboard di HP

1. Buka **Serial Monitor** di Arduino IDE (**Tools** -> **Serial Monitor**), atur baud rate ke `115200`.
2. Tekan tombol **EN / RST** pada ESP32 satu kali.
3. ESP32 akan mencetak informasi:
   ```text
   Menghubungkan ke Wi-Fi: MyHome_WiFi......
   [OK] Terhubung ke Wi-Fi Rumah!
   Alamat IP ESP32: 192.168.1.50
   [OK] mDNS Aktif: http://breezemate.local
   [OK] Web Server BreezeMate Berjalan di Port 80!
   ```
4. Pastikan HP kamu tersambung ke jaringan Wi-Fi rumah yang sama.
5. Buka browser di HP (Chrome / Safari / Edge), lalu akses salah satu alamat:
   - **Metode mDNS:** Ketik `http://breezemate.local` *(Sangat praktis di iOS & Windows/Mac)*
   - **Metode IP Langsung:** Ketik `http://192.168.1.50` *(Sesuaikan dengan IP yang muncul di Serial Monitor)*

---

## 5. Fitur Fail-Safe Hotspot (SoftAP)

Jika Wi-Fi rumahmu mati, ganti password, atau tidak terjangkau:
- ESP32 otomatis menyalakan hotspot darurat sendiri bernama **`BreezeMate-AP`** (Password: `12345678`).
- Sambungkan Wi-Fi HP-mu ke hotspot tersebut, lalu buka alamat:
  ```text
  http://192.168.4.1
  ```
- Dashboard kontrol tetap bisa diakses 100% tanpa internet.

---

## 6. Menggunakan WebUI Offline / File Lokal

Kamu juga bisa membuka file `web/index.html` langsung di laptop:
- Cukup dobel-klik file `web/index.html`.
- Klik badge **DEMO MODE** di kanan atas header, lalu masukkan IP ESP32 kamu (misal: `192.168.1.50`).
- Dashboard langsung terhubung live ke hardware ESP32.
