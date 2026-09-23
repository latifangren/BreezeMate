# 📌 BreezeMate — Pinout & Hardware Mapping

Dokumen ini berisi pemetaan pin GPIO mikrokontroler ESP32, opsi konfigurasi hemat (Budget 1-Zone) hingga multi-zona penuh, standar pinout kipas PC 4-Pin, sensor DHT22, dan modul relay/MOSFET.

---

## 1. Opsi Arsitektur Hardware: Budget vs Multi-Zone

BreezeMate dirancang modular agar kamu bisa menyesuaikan dengan budget dan ketersediaan komponen:

| Mode Rakitan | Jumlah Relay | Kebutuhan GPIO | Rekomendasi Board ESP32 | Estimasi Tambahan Budget Relay |
| :--- | :---: | :---: | :--- | :--- |
| **Budget (1-Zone)** | 1 Relay | 4 GPIO | ESP32-C3 SuperMini / NodeMCU / DevKit | Paling murah (1x Relay ~Rp 6.000) |
| **Dual-Zone** | 2 Relay | 5 GPIO | ESP32 standard (30 / 38 pin) | Menengah (Modul 2-Ch Relay ~Rp 12.000) |
| **Tri-Zone (Full)** | 3 Relay | 6 GPIO | ESP32 standard (30 / 38 pin) | Lengkap (Modul 4-Ch Relay ~Rp 20.000) |

---

## 2. Pemetaan GPIO ESP32

| Komponen / Fungsi | Pin Default ESP32 | Pin ESP32-C3 SuperMini | Keterangan Tegangan & Resistor |
| :--- | :---: | :---: | :--- |
| **PWM Fan Control** | `GPIO 18` | `GPIO 6` | Frekuensi 25 kHz standar Intel (Logika 3.3V) |
| **Tachometer (RPM)** | `GPIO 19` | `GPIO 7` | *Open-Collector*, aktifkan `INPUT_PULLUP` atau resistor 10kΩ ke 3.3V |
| **Sensor DHT22 Data** | `GPIO 23` | `GPIO 2` | Pull-up 4.7kΩ–10kΩ ke 3.3V |
| **Zone 1 Relay Control** | `GPIO 25` | `GPIO 3` | Kontrol daya 12V Kipas Zona 1 (Wajib di semua mode) |
| **Zone 2 Relay Control** | `GPIO 26` | `GPIO 4` | Kontrol daya 12V Kipas Zona 2 *(Opsional)* |
| **Zone 3 Relay Control** | `GPIO 27` | `GPIO 5` | Kontrol daya 12V Kipas Zona 3 *(Opsional)* |
| **VCC Sensor DHT22** | `3.3V` | `3.3V` | Gunakan 3.3V agar logika data DHT22 aman untuk ESP32 |
| **Power Input ESP32** | `VIN` / `5V` | `5V` | Dari output 5V Buck Converter LM2596 |
| **Common Ground** | `GND` | `GND` | Wajib terhubung ke ground adaptor 12V & kipas |

---

## 3. Standar Pinout Kipas PC 4-Pin (Intel Form Factor)

> ⚠️ **Catatan Penting:** Banyak kipas aftermarket (misal Noctua chromax, Arctic BioniX, kipas OEM) menggunakan kabel **serba hitam**. Selalu gunakan nomor urut pin pada konektor soket sebagai acuan, bukan warna kabel.

| Nomor Pin | Nama Pin | Warna Kabel Standar | Deskripsi & Fungsi |
| :---: | :--- | :--- | :--- |
| **1** | **GND** | Hitam | Ground utama motor kipas (12V) |
| **2** | **+12V** | Kuning / Merah | Tegangan daya utama DC 12V dari adaptor (via Relay/MOSFET) |
| **3** | **Sense / TACH** | Hijau | Sinyal tachometer (2 pulsa per 1 putaran penuh). Open-collector. |
| **4** | **Control / PWM** | Biru | Sinyal PWM kontrol kecepatan (25 kHz, target duty cycle 20%–100%) |

---

## 4. Logika Modul Relay vs MOSFET

- **Modul Relay Optocoupler (Paling Umum):**
  - Umumnya **Active LOW** (ESP32 `LOW` = Relay Terhubung / 12V ON, `HIGH` = Relay Terputus / 12V OFF).
  - Sudah diatur di firmware: `const bool RELAY_ACTIVE_LOW = true;`.
- **Modul MOSFET Switch (misal LR7843 / AOD4184):**
  - Bekerja secara solid-state tanpa bunyi 'klik' mekanik dan hemat daya kontrol.
  - Umumnya **Active HIGH** (Ubah di firmware: `const bool RELAY_ACTIVE_LOW = false;`).
