# 📌 BreezeMate — Pinout & Hardware Mapping

Dokumen ini berisi pemetaan pin GPIO mikrokontroler ESP32, standar pinout kipas PC 4-Pin, sensor DHT22, dan modul relay/MOSFET multi-zona.

---

## 1. Pemetaan GPIO ESP32

| Komponen / Fungsi | Pin ESP32 | Tipe Sinyal | Keterangan Tegangan & Resistor |
| :--- | :--- | :--- | :--- |
| **PWM Fan Control** | `GPIO 18` | Digital Output (PWM) | Frekuensi 25 kHz standar Intel (Logika 3.3V) |
| **Tachometer (RPM)** | `GPIO 19` | Digital Input (Interrupt) | *Open-Collector*, aktifkan `INPUT_PULLUP` atau pasang 10kΩ ke 3.3V |
| **Sensor DHT22 Data** | `GPIO 23` | Digital I/O | Pull-up 4.7kΩ–10kΩ ke 3.3V |
| **Zone 1 Relay Control** | `GPIO 25` | Digital Output | Kontrol daya 12V Kipas Zona 1 |
| **Zone 2 Relay Control** | `GPIO 26` | Digital Output | Kontrol daya 12V Kipas Zona 2 |
| **Zone 3 Relay Control** | `GPIO 27` | Digital Output | Kontrol daya 12V Kipas Zona 3 |
| **VCC Sensor DHT22** | `3.3V` | Power Output | Gunakan 3.3V agar logika data DHT22 aman untuk ESP32 |
| **Power Input ESP32** | `VIN` / `5V` | Power Input | Dari output 5V Buck Converter LM2596 |
| **Common Ground** | `GND` | Ground Reference | Wajib terhubung ke ground adaptor 12V & kipas |

---

## 2. Standar Pinout Kipas PC 4-Pin (Intel Form Factor)

> ⚠️ **Catatan Penting:** Banyak kipas aftermarket (misal Noctua chromax, Arctic BioniX, kipas OEM) menggunakan kabel **serba hitam**. Selalu gunakan nomor urut pin pada konektor soket sebagai acuan, bukan warna kabel.

| Nomor Pin | Nama Pin | Warna Kabel Standar | Deskripsi & Fungsi |
| :---: | :--- | :--- | :--- |
| **1** | **GND** | Hitam | Ground utama motor kipas (12V) |
| **2** | **+12V** | Kuning / Merah | Tegangan daya utama DC 12V dari adaptor (via Relay/MOSFET) |
| **3** | **Sense / TACH** | Hijau | Sinyal tachometer (2 pulsa per 1 putaran penuh). Open-collector. |
| **4** | **Control / PWM** | Biru | Sinyal PWM kontrol kecepatan (25 kHz, target duty cycle 20%–100%) |

---

## 3. Modul Relay 12V / MOSFET Matrix (3-Channel)

BreezeMate menggunakan relay atau modul MOSFET high-side/low-side switch untuk memutus daya 12V secara fisik:

| Zona | Pin Kontrol ESP32 | Beban Kipas (Default) | Kapasitas Arus Maksimal |
| :--- | :--- | :--- | :--- |
| **Zone 1** | `GPIO 25` | 2x Kipas PC 120mm / 140mm | Sesuai rating relay (umumnya 10A 12V DC) |
| **Zone 2** | `GPIO 26` | 2x Kipas PC 120mm / 140mm | Sesuai rating relay (umumnya 10A 12V DC) |
| **Zone 3** | `GPIO 27` | 2x Kipas PC 120mm / 140mm | Sesuai rating relay (umumnya 10A 12V DC) |

- **Logika Aktuasi Relay:**
  - Sebagian besar modul relay optocoupler bekerja dengan logika **Active LOW** (ESP32 `LOW` = Relay Menyala / 12V Terhubung; `HIGH` = Relay Terputus / 12V Mati).
  - Jika menggunakan modul MOSFET (seperti IRF520 / AOD4184), logika adalah **Active HIGH** (`HIGH` = ON, `LOW` = OFF).
