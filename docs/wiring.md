# ⚡ BreezeMate — Skema Pengkabelan & Perakitan (Wiring Guide)

Panduan instalasi elektrikal sistem pendingin BreezeMate untuk mode **Budget 1-Zona** (paling hemat) maupun **Multi-Zona (2–3 Zona)** berbasis tegangan rendah (12V DC).

---

## 🔌 1. Skema Pengkabelan Versi Hemat (Budget 1-Zona)
> *Gunakan skema ini jika ingin merakit dengan 1 modul relay saja untuk memutus daya semua kipas sekaligus.*

```text
[Adaptor DC 12V (+)] --------+---> [Sekring 3A] ---> [Buck Converter IN (+)]
                             |                  ---> [Kapasitor 1000µF 25V (+)]
                             +---------------------> [Relay Input (COM)]

[Adaptor DC 12V (-)] --------+---> [Buck Converter IN (-)]
                             +---> [Kapasitor 1000µF 25V (-)]
                             +---> [Pin 1 (GND) Seluruh Kipas]
                             +---> [Relay Modul GND]
                             +---> [ESP32 GND]  <=== (COMMON GROUND WAJIB!)

--- DAYA KE KONTROLER ---
[Buck Converter OUT (+) 5V] -------> [ESP32 VIN / 5V]
[Buck Converter OUT (-) GND] ------> [ESP32 GND]

--- DAYA KE KIPAS VIA RELAY (TRUE 0-RPM) ---
[Relay Output (NO)] ---------------> [Pin 2 (+12V) Kipas] (Bisa diparalel ke 2-4 kipas)

--- KONTROL LOGIKA DARI ESP32 ---
[ESP32 GPIO 25] -------------------> [Relay IN (Trigger Pemutus 12V)]
[ESP32 GPIO 18] -------------------> [Pin 4 (PWM) Kipas] (Sinyal 25 kHz)
[ESP32 GPIO 19] -------------------> [Pin 3 (TACH) Kipas Utama] (Sensor RPM)
[ESP32 GPIO 23] -------------------> [Pin 2 (Data) DHT22] (+ Resistor Pull-up ke 3.3V)
[ESP32 3.3V] ----------------------> [VCC DHT22]
```

---

## 🔌 2. Skema Pengkabelan Multi-Zona Penuh (2–3 Zona)
> *Gunakan skema ini jika kipas dibagi dalam beberapa kelompok zona terpisah.*

```text
[Adaptor DC 12V (+)] --------+---> [Sekring 5A] ---+---> [Buck Converter IN (+)]
                             |                     +---> [Kapasitor 1000µF 25V (+)]
                             |                     +---> [Relay 1, 2, 3 Terminal COM]
                             |
[Adaptor DC 12V (-)] --------+---> [Buck Converter IN (-)]
                             +---> [Kapasitor 1000µF 25V (-)]
                             +---> [Pin 1 GND Semua Kipas di Semua Zona]
                             +---> [ESP32 GND]  <=== (COMMON GROUND WAJIB!)

--- RELAY MATRIX (ZONE 1, 2, 3) ---
[Relay 1 Output NO] ---------------> [Pin 2 (+12V) Kipas Zone 1]
[Relay 2 Output NO] ---------------> [Pin 2 (+12V) Kipas Zone 2]
[Relay 3 Output NO] ---------------> [Pin 2 (+12V) Kipas Zone 3]

--- KONTROL LOGIKA GPIO ---
[ESP32 GPIO 25] -------------------> [Relay 1 Trigger (Zone 1)]
[ESP32 GPIO 26] -------------------> [Relay 2 Trigger (Zone 2)]
[ESP32 GPIO 27] -------------------> [Relay 3 Trigger (Zone 3)]

[ESP32 GPIO 18] -------------------> [Pin 4 PWM Seluruh Kipas] (Diparalel)
[ESP32 GPIO 19] -------------------> [Pin 3 TACH Kipas Utama]
[ESP32 GPIO 23] -------------------> [Pin 2 Data DHT22]
[ESP32 3.3V] ----------------------> [VCC Sensor DHT22] & Pull-Up
```

---

## 🛡️ 3. Aturan Perakitan Wajib

1. **Common Ground Wajib:** Semua kabel ground (-) harus bertemu di satu titik bersama agar sinyal PWM 3.3V ESP32 terbaca stabil oleh chip kipas.
2. **Kapasitor Buffer 1000µF:** Pasang paralel pada jalur 12V dekat kipas/relay untuk meredam hentakan arus motor dan mencegah ESP32 restart mendadak (*brownout*).
3. **Setel Buck Converter Sebelum Disambung ke ESP32:** Putar baut trimpot buck converter hingga multimeter membaca output tepat **5.0V – 5.1V** baru tancapkan ke pin VIN ESP32.
4. **Resistor Pull-Up:** Pasang resistor 10kΩ dari pin 3.3V ESP32 ke pin TACH (GPIO 19) dan 4.7kΩ ke pin Data DHT22 (GPIO 23).
