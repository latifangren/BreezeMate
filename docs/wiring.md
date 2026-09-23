# ⚡ BreezeMate — Skema Pengkabelan & Perakitan (Wiring Guide)

Panduan lengkap instalasi elektrikal sistem pendingin ruangan multi-zona BreezeMate berbasis tegangan rendah (12V DC).

---

## 🔌 1. Diagram Skematik Sirkuit Lengkap

```text
[Adaptor DC 12V (+)] --------+---> [Sekring 3A - 5A] ---+---> [Relay Modul VCC (12V In)]
                             |                          +---> [Buck Converter IN (+)]
                             |                          +---> [Kapasitor 1000µF 25V (+)]
                             |
[Adaptor DC 12V (-)] --------+---> [Buck Converter IN (-)]
                             +---> [Kapasitor 1000µF 25V (-)]
                             +---> [Kipas Zone 1, 2, 3 Pin 1 (GND)]
                             +---> [Relay Modul GND]
                             +---> [ESP32 GND]  <=== (COMMON GROUND WAJIB!)

--- DISTRIBUSI TEGANGAN KONTROLER (5V & 3.3V) ---
[Buck Converter OUT (+) 5V] -------> [ESP32 VIN / 5V]
[Buck Converter OUT (-) GND] ------> [ESP32 GND]

[ESP32 Pin 3.3V] ------------+-----> [VCC Sensor DHT22]
                             +--[Resistor 4.7k - 10k]---> [Data DHT22]
                             +--[Resistor 10k Pull-Up]--> [Pin 3 TACH Kipas] (Opsional)

--- MATRIX DAYA RELAY 12V (ZONE 1, 2, 3) ---
[Relay 1 NO (Normally Open)] ------> [Pin 2 (+12V) Kipas Zone 1]
[Relay 2 NO (Normally Open)] ------> [Pin 2 (+12V) Kipas Zone 2]
[Relay 3 NO (Normally Open)] ------> [Pin 2 (+12V) Kipas Zone 3]

--- KONTROL SINYAL DARI ESP32 ---
[ESP32 GPIO 25] -------------------> [Relay IN 1 (Zone 1 Trigger)]
[ESP32 GPIO 26] -------------------> [Relay IN 2 (Zone 2 Trigger)]
[ESP32 GPIO 27] -------------------> [Relay IN 3 (Zone 3 Trigger)]

[ESP32 GPIO 18] -------------------> [Pin 4 (PWM) Kipas Seluruh Zona] (Diparalel)
[ESP32 GPIO 19] -------------------> [Pin 3 (TACH) Kipas Utama] (Interrupt RPM)
[ESP32 GPIO 23] -------------------> [Pin 2 (Data) DHT22]
```

---

## 🛡️ 2. Aturan Keselamatan & Stabilitas Elektrikal

### A. Wajib Common Ground (Satu Titik Acuan Tegangan)
Ground dari Adaptor 12V, Buck Converter, Modul Relay, Kipas, dan ESP32 harus saling terhubung. Tanpa common ground:
- Pulsa PWM (3.3V) dari ESP32 tidak akan terbaca dengan benar oleh sirkuit logic kipas.
- Kipas bisa berputar liar atau mendengung tanpa kendali.

### B. Kapasitor Buffer 12V (Peredam Induktansi Motor)
- Pasang kapasitor elektrolit **470µF – 1000µF 25V** secara paralel tepat di jalur 12V sebelum masuk ke kipas.
- **Tujuan:** Saat kipas berputar kencang atau switching PWM aktif, kumparan motor menghasilkan fluktuasi arus (voltage ripple). Kapasitor ini menjaga tegangan tetap mulus sehingga ESP32 tidak mengalami restart tiba-tiba (*brownout*).

### C. Proteksi Pin Tachometer (RPM Sense)
- Pin 3 pada kipas PC berkarakteristik *Open-Collector / Open-Drain*.
- Gunakan internal pull-up ESP32 (`pinMode(19, INPUT_PULLUP)`) atau resistor eksternal 10kΩ yang ditarik ke **3.3V** ESP32.
- ⚠️ **Peringatan Keras:** Jangan pernah menarik pull-up ke rel 12V atau 5V. GPIO ESP32 hanya tahan maksimal 3.6V.

### D. Daya Sensor DHT22
- Hubungkan VCC DHT22 ke pin **3.3V** ESP32 (bukan 5V). Hal ini memastikan sinyal pulsa data dari DHT22 berada pada level tegangan 3.3V yang aman bagi GPIO 23 ESP32.

---

## 🧰 3. Langkah Perakitan Bertahap

1. **Setting Buck Converter Terlebih Dahulu:**
   - Sambungkan input Buck Converter ke Adaptor 12V.
   - Ukur output Buck Converter dengan multimeter, putar potensiometer hingga menghasilkan tepat **5.0V – 5.1V** sebelum dihubungkan ke ESP32 VIN.
2. **Sambungkan Jalur Ground Bersama:**
   - Buat terminal ground bersama (bisa menggunakan breadboard, PCB bolong, atau terminal block WAGO).
3. **Hubungkan Modul Relay:**
   - Hubungkan input daya modul relay (DC 12V/5V dan GND).
   - Pasang jalur kabel kontrol GPIO 25, 26, 27 ke pin input modul relay.
4. **Pasang Kipas & Sensor:**
   - Pasang kabel 12V kipas melewati terminal COM dan NO pada masing-masing channel relay.
   - Paralelkan pin 4 (PWM) semua kipas ke GPIO 18 ESP32.
   - Pasang sensor DHT22 ke GPIO 23 dengan pull-up 4.7kΩ ke 3.3V.
