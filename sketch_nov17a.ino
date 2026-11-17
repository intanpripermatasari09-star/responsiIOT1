
#include <WiFi.h>
#include "DHT.h"

// ==========================================
// KONFIGURASI WIFI
// ==========================================
const char* WIFI_SSID = "MR-FK-01";
const char* WIFI_PASSWORD = "adzanmasjid";

const unsigned long TIMEOUT_MS = 20000;
unsigned long waktuSebelumnya = 0;

// ==========================================
// KONFIGURASI SENSOR
// ==========================================
#define DHTPIN 4
#define LDRPIN 34
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LDRPIN, INPUT);
  dht.begin();

  Serial.println();
  Serial.println("====================================");
  Serial.println("     SMART ROOM MONITORING ESP32");
  Serial.println("====================================");

  // Inisialisasi WiFi
  WiFi.mode(WIFI_STA);

  Serial.println("[INIT] Mode WiFi Station berhasil.");
  Serial.print("[INIT] MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Menghubungkan ke WiFi
  Serial.print("[CONNECT] Menghubungkan ke ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  // Verifikasi koneksi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[SUCCESS] WiFi berhasil terhubung!");

    Serial.print("SSID         : ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address   : ");
    Serial.println(WiFi.localIP());

    Serial.print("Gateway      : ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("Subnet Mask  : ");
    Serial.println(WiFi.subnetMask());

    Serial.print("MAC Address  : ");
    Serial.println(WiFi.macAddress());

    Serial.print("Channel      : ");
    Serial.println(WiFi.channel());
  } else {
    Serial.println("[ERROR] Gagal terhubung ke WiFi!");
    Serial.println("Periksa SSID dan password.");
  }

  Serial.println("====================================");
  Serial.println("MEMULAI PEMANTAUAN SENSOR");
}

// ==========================================
// LOOP
// ==========================================
void loop() {
  unsigned long sekarang = millis();

  // Pemantauan setiap 2 detik
  if (sekarang - waktuSebelumnya >= 2000) {
    waktuSebelumnya = sekarang;

    Serial.println();
    Serial.println("------------------------------------");

    Serial.print("Waktu       : ");
    Serial.print(sekarang);
    Serial.println(" ms");

    // ======================================
    // A. MEMBACA SENSOR DHT22
    // ======================================
    float suhu = dht.readTemperature();
    float kelembapan = dht.readHumidity();

    if (isnan(suhu) || isnan(kelembapan)) {
      Serial.println("DHT22       : Gagal membaca sensor!");
    } else {
      Serial.print("Suhu        : ");
      Serial.print(suhu);
      Serial.println(" °C");

      Serial.print("Kelembapan  : ");
      Serial.print(kelembapan);
      Serial.println(" %");
    }

    // ======================================
    // B. MEMBACA SENSOR LDR
    // ======================================
    int cahaya = digitalRead(LDRPIN);

    Serial.print("Nilai LDR   : ");
    Serial.println(cahaya);

    if (cahaya == LOW) {
      Serial.println("Cahaya      : TERANG");
    } else {
      Serial.println("Cahaya      : GELAP");
    }

    // ======================================
    // C. MEMANTAU KONEKSI WIFI
    // ======================================
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi        : TERPUTUS");
      Serial.println("Status      : Mencoba reconnect...");

      WiFi.reconnect();
    } else {
      Serial.println("WiFi        : TERHUBUNG");

      Serial.print("SSID        : ");
      Serial.println(WiFi.SSID());

      Serial.print("IP Address  : ");
      Serial.println(WiFi.localIP());

      // ====================================
      // D. MEMBACA KEKUATAN SINYAL RSSI
      // ====================================
      int rssi = WiFi.RSSI();

      Serial.print("RSSI        : ");
      Serial.print(rssi);
      Serial.println(" dBm");

      if (rssi >= -50) {
        Serial.println("Kualitas    : SANGAT BAGUS");
      } else if (rssi >= -67) {
        Serial.println("Kualitas    : BAGUS");
      } else if (rssi >= -75) {
        Serial.println("Kualitas    : CUKUP");
      } else if (rssi >= -85) {
        Serial.println("Kualitas    : BURUK");
      } else {
        Serial.println("Kualitas    : KRITIS");
      }
    }

    Serial.println("------------------------------------");
  }
}
