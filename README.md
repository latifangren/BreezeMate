# 🍃 BreezeMate

> **Smart Low-Power Modular Multi-Zone Room Cooler** — Sistem pendingin ruangan mandiri (DIY) berbasis tegangan rendah (12V DC) dengan kendali putaran otomatis adaptif terhadap suhu ruangan, fleksibilitas rakitan modular (Budget 1-Zona hingga 3-Zona), dan dashboard WebUI tertanam langsung di ESP32.

BreezeMate adalah alternatif pendingin kamar tidur dan ruang kerja hemat daya (konsep *AC Kere Hore* modern). Dirancang agar kamu tidak lagi kepanasan di siang terik dan tidak menggigil atau masuk angin saat suhu turun menjelang subuh.

100% aman dirakit sendiri di rumah karena menggunakan arus DC tegangan rendah tanpa menyentuh kabel listrik AC 220V secara langsung.

---

## ✨ Fitur Utama

- ⚡ **Zero High-Voltage Risk:** Menggunakan adaptor terisolasi 12V DC murni. Seluruh modul hanya bekerja pada tegangan 12V dan 5V/3.3V.
- 🧱 **Fleksibilitas Arsitektur Modular:** Bisa dirakit mulai dari versi paling hemat (**Budget 1-Zona** dengan 1 relay saja dan board murah seperti ESP32-C3 SuperMini) hingga versi penuh (**Multi-Zona 2–3 Relay**).
- 🌀 **Support Semua Jenis Kipas PC (1200 – 3000+ RPM):** Bekerja pada standar Intel 25 kHz PWM. Bebas menggunakan kipas casing murah (~Rp 20.000) hingga kipas high-performance.
- 🔇 **Silent 25 kHz PWM Driving:** Kumparan motor kipas hening total tanpa mendengung pada duty cycle rendah.
- 🌡️ **Adaptive Thermal Regulation:** Putaran kipas menyesuaikan suhu ruangan secara dinamis dengan proteksi *anti-stall* (ambang batas minimum 20%), *hysteresis* (±0.5°C), dan *sensor fail-safe* otomatis.
- 📱 **Embedded Cyberpunk WebUI (Mobile & Desktop Friendly):** Dashboard WebUI tersimpan langsung di flash memori ESP32 (Gzip PROGMEM 9.7 KB). Cukup buka `http://breezemate.local` di HP, tanpa perlu install server atau Home Assistant.
- 🔌 **True 0-RPM & Zero Standby Draw:** Fitur pemutus daya 12V fisik via relay optocoupler / MOSFET untuk mematikan kipas sepenuhnya saat suhu sejuk.
- 💡 **Konsumsi Daya Ekstrem Rendah:** Hanya memakan daya sekitar 5W – 25W (jauh lebih hemat dibandingkan AC konvensional 350W – 800W).

---

## 🏗️ Pilihan Varian Rakitan: Budget vs Full Setup

Kamu bisa merakit BreezeMate sesuai budget dan kebutuhan:

| Komponen | Opsi A: Budget 1-Zona (Paling Murah) | Opsi B: Full Setup 3-Zona |
| :--- | :--- | :--- |
| **Mikrokontroler** | ESP32-C3 SuperMini (~Rp 25.000) / ESP32 DevKit | ESP32 DevKit 30/38 Pin (~Rp 45.000) |
| **Modul Relay** | 1-Channel Relay 5V (~Rp 6.000) | 3-Channel / 4-Channel Relay 5V (~Rp 20.000) |
| **Kipas DC 12V** | 2x Kipas PC 120mm Standar (~Rp 25.000/pc) | 4–6x Kipas PC 120mm / 140mm PWM |
| **Kebutuhan Pin GPIO** | **Hanya 4 Pin GPIO** | 6 Pin GPIO |
| **Tampilan WebUI** | **1 Kartu Hero Zona** (Otomatis menyesuaikan) | **3 Kartu Zona Mandiri** |
| **Estimasi Total Biaya** | **± Rp 120.000 – Rp 150.000** | **± Rp 200.000 – Rp 280.000** |

---

## 🌪️ Kompatibilitas Kipas: Bebas Pilih RPM

BreezeMate menggunakan sinyal kontrol persentase (duty cycle 0%–100%) dan membaca putaran aktual fisik via pin tachometer:

- **1200 – 1500 RPM (Budget / Silent):** ~45 CFM, 2.2W per kipas. *(Sangat direkomendasikan untuk kamar tidur karena nyaris tanpa suara).*
- **1800 RPM (Standard / Balanced):** ~58 CFM, 3.2W per kipas (contoh: Arctic P12 PWM).
- **2200 RPM (Performance):** ~72 CFM, 4.8W per kipas.
- **3000 RPM (High-Speed / Server):** ~115 CFM, 8.5W per kipas (hanya jika butuh angin sangat kencang).

> *Di WebUI tersedia dropdown **FAN SPEC RATING** untuk memilih profil kipas yang kamu pakai agar kalkulasi Watt dan CFM di layar otomatis akurat.*

---

## 🛠️ Daftar Komponen Lengkap (BOM)

| Komponen | Spesifikasi Rekomendasi | Jumlah |
| :--- | :--- | :---: |
| **Mikrokontroler** | ESP32 Dev Module atau ESP32-C3 SuperMini | 1 unit |
| **Kipas DC** | Kipas PC 120mm / 140mm 4-Pin PWM | 2–6 unit |
| **Sensor Suhu** | DHT22 (AM2302) Suhu & Kelembaban | 1 unit |
| **Power Supply** | Adaptor DC 12V (Minimal 2A – 3A) | 1 unit |
| **Step-Down Module** | Buck Converter LM2596 (12V ke 5.0V untuk ESP32) | 1 unit |
| **Modul Relay** | 1-Channel atau 3-Channel 5V Relay Optocoupler | 1 unit |
| **Kapasitor Buffer** | Elektrolit 470µF – 1000µF 25V (Peredam noise motor 12V) | 1 unit |
| **Resistor Pull-Up** | 10kΩ (Tachometer RPM) & 4.7kΩ (Data DHT22) | 1–2 pcs |
| **Sekring (Opsional)** | Fuse DC 3A–5A inline | 1 set |
| **Frame / Dudukan** | Bracket 3D Print / Akrilik (1x2 atau 1x3 per zona) | 1–3 set |

---

## 📌 Ringkasan Pinout Hardware

| Komponen / Sinyal | ESP32 Standar | ESP32-C3 SuperMini | Keterangan |
| :--- | :---: | :---: | :--- |
| **PWM Fan Control** | `GPIO 18` | `GPIO 6` | Sinyal 25 kHz ke Pin 4 Kipas |
| **Tachometer (RPM)** | `GPIO 19` | `GPIO 7` | Sinyal RPM dari Pin 3 Kipas (Pull-up ke 3.3V) |
| **Data Sensor DHT22** | `GPIO 23` | `GPIO 2` | Sinyal Suhu & Kelembaban (Pull-up ke 3.3V) |
| **Zone 1 Relay Control**| `GPIO 25` | `GPIO 3` | Pemutus Daya 12V Kipas Zona 1 (Wajib) |
| **Zone 2 Relay Control**| `GPIO 26` | `GPIO 4` | Pemutus Daya 12V Kipas Zona 2 *(Opsional)* |
| **Zone 3 Relay Control**| `GPIO 27` | `GPIO 5` | Pemutus Daya 12V Kipas Zona 3 *(Opsional)* |
| **Power Input ESP32** | `VIN` | `5V` | Menerima 5.0V dari Buck Converter |
| **Common Ground** | `GND` | `GND` | Wajib disatukan ke GND Adaptor 12V & Kipas |

> *Panduan lengkap pinout soket kipas 4-pin dapat dibaca di [docs/pinout.md](docs/pinout.md).*

---

## ⚡ Skema Pengkabelan (Wiring)

```text
[Adaptor 12V (+)] --------+---> [Sekring 3A] ---+---> [Buck Converter IN (+)]
                          |                     +---> [Kapasitor 1000µF (+)]
                          |                     +---> [Relay VCC / Terminal COM]
                          |
[Adaptor 12V (-)] --------+---> [Buck Converter IN (-)]
                          +---> [Kapasitor 1000µF (-)]
                          +---> [GND Seluruh Kipas]
                          +---> [ESP32 GND]  <=== (COMMON GROUND WAJIB!)

[Buck OUT 5V (+)] ------------> [ESP32 VIN / 5V]
[Buck OUT GND (-)] -----------> [ESP32 GND]

[ESP32 GPIO 25] --------------> [Relay 1 Trigger] ---> [Zone 1 Kipas 12V (+)]
[ESP32 GPIO 18] --------------> [Pin 4 PWM Kipas] (Diparalel ke semua kipas)
[ESP32 GPIO 19] --------------> [Pin 3 TACH Kipas] (Interrupt RPM)
[ESP32 GPIO 23] --------------> [Pin 2 Data DHT22] (+ Resistor Pull-up ke 3.3V)
[ESP32 3.3V] -----------------> [VCC DHT22] & Pull-up
```

> *Skematik kabel terpisah untuk varian Budget 1-Zona dan Multi-Zona tersedia di [docs/wiring.md](docs/wiring.md).*

---

## 📊 Kurva Penyesuaian Suhu Otomatis

BreezeMate memetakan target suhu terhadap hembusan udara secara bertahap:

| Suhu Ruangan (°C) | Status Kondisi | Target PWM | Perilaku Kipas |
| :--- | :--- | :--- | :--- |
| **< 24.0°C** | Sejuk / Subuh Dingin | 0% (Cut Relay) atau 20% | Mati total / Putaran hening minimum |
| **24.0°C – 26.9°C** | Nyaman / Suhu Kamar Ideal | 25% – 45% | Sirkulasi lembut dan stabil |
| **27.0°C – 29.9°C** | Hangat / Gerah Siang | 50% – 75% | Aliran udara aktif mendinginkan |
| **≥ 30.0°C** | Sangat Panas | 100% | Full blast (Performa pendinginan maksimal) |
| **Sensor Error (NaN)** | Sensor Disconnected | 60% | Mode darurat fail-safe |

---

## 📁 Struktur Direktori Repositori

```text
BreezeMate/
├── docs/                      # Dokumentasi teknis hardware & panduan
│   ├── pinout.md              # Pemetaan GPIO ESP32 standar & ESP32-C3 SuperMini
│   ├── wiring.md              # Skematik kelistrikan Budget 1-Zona vs Multi-Zona
│   └── firmware.md            # Panduan compile & flash firmware via Arduino IDE
├── firmware/
│   └── BreezeMate/
│       ├── BreezeMate.ino     # Kode sumber C++ utama ESP32
│       └── index_html.h       # Dashboard WebUI Gzip PROGMEM tertanam
├── hardware/                  # Desain bracket 3D printing & mounting
├── web/
│   └── index.html             # Source code WebUI Cyberpunk mandiri
├── .gitignore                 # Filter build & file sensitif
└── README.md                  # Dokumentasi utama proyek
```

---

## 🚀 Panduan Memulai Cepat (Quick Start)

### 1. Flash Firmware ke ESP32
1. Buka file `firmware/BreezeMate/BreezeMate.ino` di **Arduino IDE**.
2. Atur jumlah zona di baris 22:
   ```cpp
   #define ACTIVE_ZONES 1  // 1 untuk Budget, 2 atau 3 untuk Multi-Zone
   ```
3. Masukkan nama dan password Wi-Fi rumahmu di baris 27–28:
   ```cpp
   const char* WIFI_SSID     = "NAMA_WIFI_RUMAHMU";
   const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";
   ```
4. Hubungkan ESP32 via kabel USB, pilih board **ESP32 Dev Module**, lalu klik **Upload**.
5. *Detail panduan instalasi library dan flashing ada di [docs/firmware.md](docs/firmware.md).*

### 2. Buka Dashboard di HP
1. Pastikan HP tersambung ke jaringan Wi-Fi rumah yang sama.
2. Buka browser di HP, ketik:
   ```text
   http://breezemate.local
   ```
   *(Atau masukkan IP ESP32 yang muncul di Serial Monitor Arduino IDE).*
3. Dashboard langsung aktif dengan kendali hembusan udara, relay cutoff, dan telemetri suhu real-time!
