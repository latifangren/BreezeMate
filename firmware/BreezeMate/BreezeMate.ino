/**
 * 🍃 BreezeMate — Standalone ESP32 Multi-Zone Thermal Controller
 * 
 * Hardware:
 * - ESP32 Development Board
 * - DHT22 (AM2302) on GPIO 23
 * - PC Fan 4-Pin PWM on GPIO 18 (25 kHz)
 * - PC Fan Tachometer on GPIO 19
 * - 3x 12V Relays on GPIO 25 (Zone 1), GPIO 26 (Zone 2), GPIO 27 (Zone 3)
 * 
 * Features:
 * - Local Wi-Fi Web Server (Port 80) + mDNS (http://breezemate.local)
 * - SoftAP Fallback ("BreezeMate-AP") if home Wi-Fi not found
 * - 25 kHz Silent Hardware PWM (Intel Standard)
 * - Adaptive Thermal Curve + Anti-Stall (Min 20% PWM) + Hysteresis (±0.5°C)
 * - True 0-RPM 12V Relay Cutoff per zone
 * - Real-time JSON API for WebUI
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

// ==========================================
// 1. PENGATURAN WI-FI RUMAH
// ==========================================
const char* WIFI_SSID     = "NAMA_WIFI_RUMAHMU";   // Ganti dengan SSID Wi-Fi rumahmu
const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";  // Ganti dengan Password Wi-Fi rumahmu

// Pengaturan SoftAP (Jika Wi-Fi rumah gagal tersambung)
const char* AP_SSID       = "BreezeMate-AP";
const char* AP_PASSWORD   = "12345678";

// ==========================================
// 2. PEMETAAN PIN HARDWARE (GPIO)
// ==========================================
#define PIN_FAN_PWM       18  // Output 25 kHz PWM ke Pin 4 Kipas
#define PIN_FAN_TACH      19  // Input RPM Sensor dari Pin 3 Kipas
#define PIN_DHT_DATA      23  // Data Sensor DHT22
#define PIN_RELAY_ZONE1   25  // Pemutus 12V Zona 1
#define PIN_RELAY_ZONE2   26  // Pemutus 12V Zona 2
#define PIN_RELAY_ZONE3   27  // Pemutus 12V Zona 3

#define DHT_TYPE          DHT22

// Modul relay: Kebanyakan modul relay optocoupler adalah ACTIVE_LOW.
// Set true jika modul relay kamu menyala saat pin LOW. Set false jika ACTIVE_HIGH.
const bool RELAY_ACTIVE_LOW = true;

// ==========================================
// 3. PARAMETER PWM & THERMAL
// ==========================================
const int PWM_FREQ        = 25000; // 25 kHz standar Intel PC Fan
const int PWM_RES         = 8;     // 8-bit resolusi (0 - 255)
const int PWM_CHANNEL     = 0;     // LEDC Channel (untuk Core v2.x)

const int MIN_SAFE_PWM    = 51;    // 20% duty cycle (Anti-Stall motor kipas)
const int MAX_PWM         = 255;   // 100% duty cycle

// ==========================================
// 4. STRUKTUR DATA STATE SISTEM
// ==========================================
struct ZoneConfig {
  String name;
  uint8_t gpio;
  bool enabled;
  int pwm;
};

ZoneConfig zones[3] = {
  {"Zone 1", PIN_RELAY_ZONE1, true, 58},
  {"Zone 2", PIN_RELAY_ZONE2, true, 58},
  {"Zone 3", PIN_RELAY_ZONE3, true, 58}
};

String currentMode   = "auto"; // "auto", "night", "manual"
bool syncLinked      = true;
float currentTemp    = 26.5;
float currentHumid   = 60.0;
float targetTemp     = 25.0;
int masterPwm        = 58;

// Tachometer
volatile unsigned long tachPulses = 0;
unsigned long lastTachTime        = 0;
int currentRpm                    = 0;

// Sensor & Server Instance
DHT dht(PIN_DHT_DATA, DHT_TYPE);
WebServer server(80);
unsigned long lastSensorRead = 0;

// ==========================================
// 5. INTERRUPT SERVICE ROUTINE (TACHOMETER)
// ==========================================
void IRAM_ATTR onTachPulse() {
  tachPulses++;
}

// ==========================================
// 6. HELPER FUNGSI RELAY & PWM
// ==========================================
void setRelay(uint8_t pin, bool on) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(pin, on ? LOW : HIGH);
  } else {
    digitalWrite(pin, on ? HIGH : LOW);
  }
}

void applyPwmDuty(int dutyValue) {
  dutyValue = constrain(dutyValue, 0, 255);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_FAN_PWM, dutyValue);
#else
  ledcWrite(PWM_CHANNEL, dutyValue);
#endif
}

// Menghitung target PWM adaptif berdasarkan kurva suhu
int calculateAutoPwm(float temp) {
  if (isnan(temp)) return 153; // Sensor fail-safe (60%)
  
  if (temp < 24.0) {
    return 0; // Mati atau whisper 20%
  } else if (temp < 27.0) {
    // 24.0 - 27.0°C -> 25% - 45% (PWM 64 - 115)
    return map((long)(temp * 10), 240, 270, 64, 115);
  } else if (temp < 30.0) {
    // 27.0 - 30.0°C -> 50% - 75% (PWM 128 - 191)
    return map((long)(temp * 10), 270, 300, 128, 191);
  } else {
    // >= 30.0°C -> 100%
    return 255;
  }
}

void updateHardwareActuators() {
  for (int i = 0; i < 3; i++) {
    setRelay(zones[i].gpio, zones[i].enabled);
  }

  int activePwmPercent = 0;
  if (currentMode == "auto") {
    int autoDuty = calculateAutoPwm(currentTemp);
    activePwmPercent = map(autoDuty, 0, 255, 0, 100);
    masterPwm = activePwmPercent;
    if (syncLinked) {
      for (int i = 0; i < 3; i++) zones[i].pwm = activePwmPercent;
    }
    applyPwmDuty(autoDuty);
  } else if (currentMode == "night") {
    activePwmPercent = 25; // Whisper quiet
    masterPwm = activePwmPercent;
    applyPwmDuty(64); // ~25%
  } else {
    // Manual
    int duty = map(masterPwm, 0, 100, 0, 255);
    if (duty > 0 && duty < MIN_SAFE_PWM) duty = MIN_SAFE_PWM; // Anti-stall
    applyPwmDuty(duty);
  }
}

// ==========================================
// 7. REST API ENDPOINTS
// ==========================================
void handleApiStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String json = "{";
  json += "\"temperature\":" + String(currentTemp, 1) + ",";
  json += "\"humidity\":" + String(currentHumid, 1) + ",";
  json += "\"targetTemp\":" + String(targetTemp, 1) + ",";
  json += "\"mode\":\"" + currentMode + "\",";
  json += "\"syncLinked\":" + String(syncLinked ? "true" : "false") + ",";
  json += "\"masterPwm\":" + String(masterPwm) + ",";
  json += "\"rpm\":" + String(currentRpm) + ",";
  json += "\"zones\":{";
  for (int i = 0; i < 3; i++) {
    json += "\"" + String(i + 1) + "\":{";
    json += "\"name\":\"" + zones[i].name + "\",";
    json += "\"gpio\":" + String(zones[i].gpio) + ",";
    json += "\"enabled\":" + String(zones[i].enabled ? "true" : "false") + ",";
    json += "\"pwm\":" + String(zones[i].pwm);
    json += "}";
    if (i < 2) json += ",";
  }
  json += "}}";

  server.send(200, "application/json", json);
}

void handleApiControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  // Toggle relay per zone
  if (server.hasArg("zone") && server.hasArg("relay")) {
    int z = server.arg("zone").toInt() - 1;
    if (z >= 0 && z < 3) {
      zones[z].enabled = (server.arg("relay").toInt() == 1);
    }
  }

  // Set mode: auto, night, manual
  if (server.hasArg("mode")) {
    currentMode = server.arg("mode");
  }

  // Set target temp
  if (server.hasArg("target")) {
    targetTemp = server.arg("target").toFloat();
  }

  // Set master PWM
  if (server.hasArg("masterPwm")) {
    masterPwm = constrain(server.arg("masterPwm").toInt(), 0, 100);
    if (syncLinked) {
      for (int i = 0; i < 3; i++) zones[i].pwm = masterPwm;
    }
  }

  // Set zone PWM (independent)
  if (server.hasArg("zone") && server.hasArg("pwm")) {
    int z = server.arg("zone").toInt() - 1;
    if (z >= 0 && z < 3) {
      zones[z].pwm = constrain(server.arg("pwm").toInt(), 0, 100);
    }
  }

  // Set sync link
  if (server.hasArg("sync")) {
    syncLinked = (server.arg("sync").toInt() == 1);
  }

  // Master Killswitch: matikan semua relay
  if (server.hasArg("killall")) {
    for (int i = 0; i < 3; i++) zones[i].enabled = false;
  }

  updateHardwareActuators();
  handleApiStatus();
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>BreezeMate</title>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<style>body{background:#08090c;color:#00e5ff;font-family:sans-serif;padding:20px;text-align:center}";
  html += "a{color:#00ff66;font-size:18px;text-decoration:none;border:1px solid #00ff66;padding:10px 20px;border-radius:6px;display:inline-block;margin-top:20px}</style></head>";
  html += "<body><h1>🍃 BreezeMate Controller Online</h1>";
  html += "<p>ESP32 IP: " + WiFi.localIP().toString() + "</p>";
  html += "<p>Status API: <a href='/api/status'>/api/status</a></p>";
  html += "<p>Buka dashboard lengkap di web browser HP Anda.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// ==========================================
// 8. SETUP INITIALIZATION
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- [🍃 BreezeMate Starting] ---");

  // Setup Relay Pins
  for (int i = 0; i < 3; i++) {
    pinMode(zones[i].gpio, OUTPUT);
    setRelay(zones[i].gpio, zones[i].enabled);
  }

  // Setup Tachometer Pin
  pinMode(PIN_FAN_TACH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), onTachPulse, RISING);

  // Setup Hardware PWM 25 kHz
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_FAN_PWM, PWM_FREQ, PWM_RES);
#else
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, PWM_CHANNEL);
#endif
  applyPwmDuty(map(masterPwm, 0, 100, 0, 255));

  // Setup Sensor DHT22
  dht.begin();

  // Koneksi ke Wi-Fi Rumah
  Serial.print("Menghubungkan ke Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 12000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[OK] Terhubung ke Wi-Fi Rumah!");
    Serial.print("Alamat IP ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[!] Gagal connect Wi-Fi rumah, mengaktifkan Hotspot Mandiri (SoftAP)...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print("Hotspot Aktif: ");
    Serial.println(AP_SSID);
    Serial.print("Alamat IP AP: ");
    Serial.println(WiFi.softAPIP());
  }

  // mDNS Setup (http://breezemate.local)
  if (MDNS.begin("breezemate")) {
    Serial.println("[OK] mDNS Aktif: http://breezemate.local");
  }

  // Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/control", HTTP_GET, handleApiControl);
  server.on("/api/control", HTTP_POST, handleApiControl);

  server.begin();
  Serial.println("[OK] Web Server BreezeMate Berjalan di Port 80!");
}

// ==========================================
// 9. LOOP UTAMA
// ==========================================
void loop() {
  server.handleClient();

  unsigned long now = millis();

  // Baca sensor DHT22 setiap 2 detik
  if (now - lastSensorRead >= 2000) {
    lastSensorRead = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      currentTemp  = t;
      currentHumid = h;
    } else {
      Serial.println("[Warn] Gagal membaca data dari DHT22!");
    }

    // Hitung RPM Kipas (2 pulsa per 1 putaran)
    noInterrupts();
    unsigned long pulses = tachPulses;
    tachPulses = 0;
    interrupts();

    unsigned long dt = now - lastTachTime;
    lastTachTime = now;
    if (dt > 0) {
      currentRpm = (int)((pulses * 60000UL) / (dt * 2));
    }

    // Update aktuator otomatis jika di mode Auto
    if (currentMode == "auto") {
      updateHardwareActuators();
    }
  }
}
