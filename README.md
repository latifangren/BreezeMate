# 🍃 BreezeMate

> **Smart Low-Power Modular Multi-Zone Room Cooler** — Sistem pendingin ruangan mandiri (DIY) berbasis tegangan rendah (12V DC) dengan kendali putaran otomatis adaptif terhadap suhu ruangan dan matriks relay multi-zona.

BreezeMate adalah alternatif pendingin kamar tidur dan ruang kerja hemat daya (konsep *AC Kere Hore* modern). Dirancang agar kamu tidak lagi kepanasan di siang terik dan tidak menggigil atau masuk angin saat suhu turun menjelang subuh.

100% aman dirakit sendiri di rumah karena menggunakan arus DC tegangan rendah tanpa menyentuh kabel listrik AC 220V secara langsung.

---

## ✨ Fitur Utama

- ⚡ **Zero High-Voltage Risk:** Menggunakan adaptor terisolasi 12V DC murni. Seluruh modul hanya bekerja pada tegangan 12V dan 5V/3.3V.
- 🎛️ **Modular Multi-Zone Relay Matrix:** Kontrol modular hingga 3 zona kipas independen (Zone 1, Zone 2, Zone 3). Setiap zona dapat dimatikan total daya 12V-nya via relay fisik untuk *True 0-RPM* dan nol daya standby.
- 🔇 **Silent 25 kHz PWM Driving:** Menggunakan frekuensi standar kipas PC (25 kHz) sehingga kumparan motor kipas hening total tanpa mendengung pada duty cycle rendah.
- 🌡️ **Adaptive Thermal Regulation:** Putaran kipas menyesuaikan suhu ruangan secara dinamis dilengkapi proteksi *anti-stall* (ambang batas minimum 20%), *hysteresis* (±0.5°C), dan *sensor fail-safe* otomatis.
- 💻 **Interactive Cyberpunk Web Deck:** Dashboard WebUI modern (`web/index.html`) dengan audio feedback klik relay mekanikal, kalkulasi daya real-time (Watt), estimasi hembusan udara (CFM), dan master emergency killswitch.
- 💡 **Hemat Energi Ekstrem:** Konsumsi daya total hanya sekitar 10W–30W (jauh lebih hemat dibandingkan AC konvensional yang memakan 350W–800W).

---

## 🛠️ Komponen yang Dibutuhkan (BOM)

| Komponen | Spesifikasi Rekomendasi | Jumlah |
| :--- | :--- | :--- |
| **Mikrokontroler** | ESP32 Development Board (30 / 38 Pin) | 1 unit |
| **Kipas DC** | Kipas PC 120mm / 140mm 4-Pin PWM (Hydraulic / FDB bearing) | 2–6 unit |
| **Sensor Suhu** | DHT22 (AM2302) untuk akurasi suhu ruangan & kelembaban | 1 unit |
| **Power Supply** | Adaptor DC 12V (Minimal 2A – 3A, sesuaikan jumlah kipas) | 1 unit |
| **Step-Down Module** | Buck Converter LM2596 (Step-down 12V ke 5V untuk ESP32) | 1 unit |
| **Modul Relay / MOSFET** | 3-Channel Relay Module 5V/12V atau 3x MOSFET Switch | 1 unit |
| **Kapasitor Buffer** | Elektrolit 470µF – 1000µF 25V (Peredam noise motor 12V) | 1 unit |
| **Resistor Pull-Up** | 10kΩ (Tachometer) & 4.7kΩ (Data DHT22) | 1–2 pcs |
| **Sekring (Opsional)** | Fuse DC 3A–5A + Dioda Schottky 1N5822 | 1 set |
| **Dudukan / Frame** | Bracket 3D Print / Akrilik (Formasi 1x2 atau 1x3 per zona) | 1–3 set |

---

## 📌 Ringkasan Pinout Hardware

| Komponen / Sinyal | Pin ESP32 | Keterangan |
| :--- | :--- | :--- |
| **PWM Fan Control** | `GPIO 18` | Sinyal 25 kHz ke Pin 4 Kipas (Diparalel) |
| **Tachometer (RPM)** | `GPIO 19` | Sinyal RPM dari Pin 3 Kipas (Pull-up ke 3.3V) |
| **Data Sensor DHT22** | `GPIO 23` | Sinyal Suhu & Kelembaban (Pull-up ke 3.3V) |
| **Zone 1 Relay Control** | `GPIO 25` | Pemutus Daya 12V Kipas Zona 1 |
| **Zone 2 Relay Control** | `GPIO 26` | Pemutus Daya 12V Kipas Zona 2 |
| **Zone 3 Relay Control** | `GPIO 27` | Pemutus Daya 12V Kipas Zona 3 |
| **ESP32 Power Input** | `VIN` | Menerima 5.0V dari output Buck Converter |
| **Common Ground** | `GND` | Wajib disatukan ke GND Adaptor 12V & Kipas |

> *Detail lengkap pinout dan diagram pin kipas 4-pin dapat dibaca di [docs/pinout.md](docs/pinout.md).*

---

## ⚡ Skema Pengkabelan (Wiring)

```text
[Adaptor 12V (+)] --------+---> [Sekring 3A] ---+---> [Buck Converter IN (+)]
                          |                     +---> [Kapasitor 1000µF (+)]
                          |                     +---> [Relay VCC Input 12V]
                          |
[Adaptor 12V (-)] --------+---> [Buck Converter IN (-)]
                          +---> [Kapasitor 1000µF (-)]
                          +---> [GND Seluruh Kipas]
                          +---> [ESP32 GND]  <=== (COMMON GROUND WAJIB!)

[Buck OUT 5V (+)] ------------> [ESP32 VIN / 5V]
[Buck OUT GND (-)] -----------> [ESP32 GND]

[ESP32 GPIO 25] --------------> [Relay IN 1] ---> [Zone 1 Kipas 12V (+)]
[ESP32 GPIO 26] --------------> [Relay IN 2] ---> [Zone 2 Kipas 12V (+)]
[ESP32 GPIO 27] --------------> [Relay IN 3] ---> [Zone 3 Kipas 12V (+)]

[ESP32 GPIO 18] --------------> [Pin 4 PWM Kipas] (Paralel)
[ESP32 GPIO 19] --------------> [Pin 3 TACH Kipas] (Interrupt RPM)
[ESP32 GPIO 23] --------------> [Pin 2 Data DHT22]
[ESP32 3.3V] -----------------> [VCC DHT22] & Pull-up Resistor
```

> *Panduan lengkap perakitan langkah demi langkah dan proteksi induksi motor tersedia di [docs/wiring.md](docs/wiring.md).*

---

## 📊 Kurva Penyesuaian Suhu Otomatis

BreezeMate memetakan suhu ruangan ke duty cycle kipas secara adaptif:

| Suhu Ruangan (°C) | Status Kondisi | Target PWM | Perilaku Kipas |
| :--- | :--- | :--- | :--- |
| **< 24.0°C** | Sejuk / Subuh Dingin | 0% (Cut Relay) atau 20% | Mati total / Putaran hening minimum |
| **24.0°C – 26.9°C** | Nyaman / Suhu Kamar Ideal | 25% – 45% | Sirkulasi lembut dan stabil |
| **27.0°C – 29.9°C** | Hangat / Gerah Siang | 50% – 75% | Aliran udara aktif mendinginkan |
| **≥ 30.0°C** | Sangat Panas | 100% | Full blast (Performa maksimal) |
| **Sensor Error (NaN)** | Sensor Disconnected | 60% | Mode darurat fail-safe |

---

## 📂 Struktur Direktori Proyek

```text
BreezeMate/
├── docs/                      # Dokumentasi teknis hardware
│   ├── pinout.md              # Pemetaan GPIO ESP32 & soket kipas 4-pin
│   └── wiring.md              # Diagram skematik & panduan perakitan
├── firmware/                  # Sumber kode mikrokontroler (Arduino C++ & ESPHome)
├── hardware/                  # Desain bracket 3D printing & mounting
├── web/                       # WebUI Dashboard BreezeMate
│   └── index.html             # Thermal Deck interaktif & telemetri
├── .gitignore                 # Filter file build & environment
└── README.md                  # Dokumentasi utama proyek
```

---

## 🚀 Panduan Memulai Cepat (Quick Start)

### 1. Buka Web Dashboard
Cukup buka file `web/index.html` di browser favoritmu (Chrome, Edge, Firefox, Safari). Dashboard ini langsung berjalan secara mandiri:
- Klik tombol **CUT 12V** pada salah satu zona untuk menyimulasikan pemutusan daya relay.
- Centang atau lepas centang **LINK CHANNELS** untuk mencoba mode sinkron vs kontrol slider per zona.
- Coba tombol **MASTER EMERGENCY CUTOFF** untuk mematikan seluruh zona seketika.

### 2. Kalibrasi Hardware & Buck Converter
1. Sambungkan Adaptor 12V ke input Buck Converter LM2596.
2. Putar trimpot potensiometer hingga multimeter membaca tegangan output tepat **5.0V**.
3. Hubungkan sirkuit sesuai panduan di [docs/wiring.md](docs/wiring.md).
