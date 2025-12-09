# 🔐 Smart Lock System with IoT

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Blynk](https://img.shields.io/badge/Blynk-2C7EF2?style=for-the-badge&logo=blynk&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

Smart Lock berbasis Arduino Mega dan ESP32-CAM dengan fitur RFID, Keypad, dan monitoring via Blynk IoT platform. Dilengkapi dengan **Restricted Mode** dan **PIN Check** untuk keamanan tingkat lanjut.

## 🆕 What's New (v3.0)

### ✨ Fitur Baru
- 🚨 **Restricted Mode** - Mode keamanan tingkat lanjut yang otomatis aktif setelah 3x percobaan gagal
- 🔑 **PIN Check & Display** - Validasi PIN ketat dan tampilan PIN real-time di Blynk
- 📊 **Attempt Counter** - Penghitung percobaan akses gagal dengan indikator visual
- 🔄 **Auto Security** - Sistem otomatis mengaktifkan Restricted Mode saat terdeteksi brute force

### 🔧 Perbaikan
- Validasi PIN lebih ketat (format dan panjang)
- Notifikasi lebih informatif dengan alasan aktivasi Restricted Mode
- Status display lebih jelas di Blynk dashboard
- LED indicator untuk Restricted Mode (merah berkedip)

---

## 📋 Daftar Isi
- [Fitur Utama](#-fitur-utama)
- [Komponen yang Dibutuhkan](#-komponen-yang-dibutuhkan)
- [Desain Rangkaian](#-desain-rangkaian)
- [Skema Wiring](#-skema-wiring)
- [Konfigurasi Blynk](#-konfigurasi-blynk)
- [Instalasi](#-instalasi)
- [Cara Penggunaan](#-cara-penggunaan)
- [Struktur Komunikasi](#-struktur-komunikasi)
- [Demo & Hasil](#-demo--hasil)
- [Troubleshooting](#-troubleshooting)

---

## ✨ Fitur Utama

### 🔓 Metode Unlock
- **RFID Card** - Akses cepat dengan kartu RFID terdaftar
- **Keypad PIN** - Input PIN 4-8 digit dengan validasi
- **Blynk App** - Remote unlock dari smartphone

### 🚨 Restricted Mode (Fitur Baru!)
- **Auto-Activation** - Otomatis aktif setelah 3x percobaan gagal:
  - 3x PIN salah → Restricted Mode aktif
  - 3x RFID tidak dikenal → Restricted Mode aktif
- **Manual Control** - Aktifkan/nonaktifkan via Blynk app (V5)
- **Behavior** - Saat aktif:
  - ❌ RFID dan Keypad **DISABLED**
  - ✅ Hanya unlock via **Blynk app** yang diizinkan
  - 🔴 LED merah berkedip sebagai indikator
  - 📱 Notifikasi push ke smartphone
- **Reset** - Nonaktifkan via Blynk atau setelah unlock berhasil

### 🔑 PIN Check & Display (Fitur Baru!)
- **PIN Validation** - Validasi ketat:
  - Panjang: 4-8 digit
  - Format: Hanya angka (0-9)
  - Real-time check saat input
- **PIN Display** - Tampilkan PIN saat ini di Blynk (V3)
- **Attempt Counter** - Hitung percobaan PIN salah
- **Security Feedback** - Tampilkan jumlah percobaan di LCD

### 🔔 Notifikasi Real-time
- ✅ **Info Notification** - Akses berhasil (RFID/Keypad)
- ⚠️ **Warning Notification** - Akses ditolak (Unknown RFID/Wrong PIN)
- 🚨 **Restricted Mode Alert** - Notifikasi saat mode terbatas aktif
- Push notification ke smartphone & web

### 📹 Monitoring
- Live video streaming dari ESP32-CAM
- Terminal log aktivitas real-time
- Status lock real-time di Blynk app
- PIN display di dashboard Blynk

### 🔧 Manajemen PIN
- Ganti PIN via Blynk app (V4)
- Ganti PIN via Keypad (mode unlock)
- Validasi PIN 4-8 digit dengan error handling
- Display PIN saat ini di Blynk dashboard

### 🔒 Keamanan
- Auto-lock setelah 10 detik
- RFID debouncing (mencegah pembacaan berulang)
- LED indikator (Merah: Locked, Hijau: Unlocked, Merah berkedip: Restricted)
- Buzzer feedback untuk setiap aktivitas
- **Attempt Limiting** - Batasi percobaan akses gagal
- **Restricted Mode** - Mode keamanan tingkat lanjut

---

## 🛠 Komponen yang Dibutuhkan

### Hardware Utama
| Komponen | Jumlah | Keterangan |
|----------|--------|------------|
| Arduino Mega 2560 | 1 | Mikrokontroler utama |
| ESP32-CAM | 1 | Camera & WiFi module |
| RFID RC522 | 1 | Reader RFID |
| Keypad 4x4 | 1 | Input PIN |
| LCD 16x2 | 1 | Display (tanpa I2C) |
| Relay 5V | 1 | Kontrol solenoid |
| Solenoid Lock 12V | 1 | Kunci elektrik |
| Buzzer | 1 | Audio feedback |
| LED Merah | 1 | Indikator locked |
| LED Hijau | 1 | Indikator unlocked |
| Resistor 220Ω | 2 | Untuk LED |
| Potensiometer 10kΩ | 1 | Kontras LCD |
| Power Supply 12V | 1 | Untuk solenoid |
| Breadboard & Kabel Jumper | - | Koneksi |

### Software & Platform
- Arduino IDE (v1.8.x atau v2.x)
- Blynk IoT Platform
- Library Arduino:
  - `LiquidCrystal.h`
  - `Keypad.h`
  - `MFRC522.h`
  - `SPI.h`
- Library ESP32:
  - `WiFi.h`
  - `WiFiManager.h`
  - `BlynkSimpleEsp32.h`
  - `esp_camera.h`

---

## 🎨 Desain Rangkaian
<img width="3000" height="2389" alt="circuit_image" src="https://github.com/user-attachments/assets/5570b4f6-9d0b-4982-b275-a7e0afff0b5e" />


### Cirkit Designer Project
Desain lengkap rangkaian dapat diakses di:
**[Cirkit Designer - Smart Lock](https://app.cirkitdesigner.com/project/d865b739-b00d-4da9-8378-f81116dbd3b7)**

Project ini mencakup:
- Skematik wiring lengkap
- Simulasi rangkaian
- Code template

### Diagram Blok Sistem
```
┌─────────────────┐
│   Blynk Cloud   │
│   (IoT Server)  │
└────────┬────────┘
         │ WiFi
         ▼
┌─────────────────┐
│   ESP32-CAM     │◄─── Camera Stream
│  - WiFi Module  │
│  - Notification │
└────────┬────────┘
         │ Serial (TX/RX)
         ▼
┌─────────────────┐
│  Arduino Mega   │
│  (Main Logic)   │
└─────────────────┘
    │   │   │   │
    ▼   ▼   ▼   ▼
  RFID Key LCD Relay
       pad      ├─► Solenoid Lock
                ├─► LED Indikator
                └─► Buzzer
```

### Skema Wiring

https://drive.google.com/file/d/10Kjfyzpf5sxFKVhkpaYQC_wO_D3xNfjp/view?usp=sharing

*Gambar skema lengkap tersedia di Google Drive*

---

## 🔌 Skema Wiring

### 1️⃣ Arduino Mega Pin Assignment

#### RFID RC522 (SPI)
```
RFID RC522          Arduino Mega
─────────────────────────────────
VCC          →      3.3V
RST          →      Pin 8
GND          →      GND
MISO         →      Pin 50
MOSI         →      Pin 51
SCK          →      Pin 52
SDA          →      Pin 53
```

#### Keypad 4x4
```
Keypad Pin          Arduino Mega
─────────────────────────────────
Row 1        →      Pin 22
Row 2        →      Pin 23
Row 3        →      Pin 24
Row 4        →      Pin 25
Col 1        →      Pin 26
Col 2        →      Pin 27
Col 3        →      Pin 28
Col 4        →      Pin 29
```

Layout Keypad:
```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ A │
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ B │
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ C │
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │
└───┴───┴───┴───┘
```

#### LCD 16x2 (Parallel Mode)
```
LCD Pin             Arduino Mega
─────────────────────────────────
VSS          →      GND
VDD          →      5V
VO           →      Potensiometer (Kontras)
RS           →      Pin 12
RW           →      GND
E            →      Pin 11
D0-D3        →      (Tidak dipakai)
D4           →      Pin 5
D5           →      Pin 4
D6           →      Pin 3
D7           →      Pin 2
A            →      5V (Backlight)
K            →      GND
```

#### Relay & Solenoid
```
Relay Module        Arduino Mega / Power
─────────────────────────────────────────
VCC          →      5V
GND          →      GND
IN (Data)    →      Pin 6

COM          →      12V+ (Power Supply)
NO           →      Solenoid VCC
Solenoid GND →      Power Supply GND
```

#### LED & Buzzer
```
Komponen            Arduino Mega
─────────────────────────────────
LED Hijau (+) →     Pin 31
LED Hijau (-) →     GND (via 220Ω)
LED Merah (+) →     Pin 30
LED Merah (-) →     GND (via 220Ω)
Buzzer (+)    →     Pin 7
Buzzer (-)    →     GND
```

#### ESP32-CAM
```
ESP32-CAM           Arduino Mega
─────────────────────────────────
U0T (TX)     →      RX1 (Pin 19)
U0R (RX)     →      TX1 (Pin 18)
GND          →      GND
5V           →      5V
```

### 2️⃣ Power Supply Diagram
```
Power Supply 12V
       │
       ├─► Relay COM
       │
       └─► (Step-down jika perlu)
              └─► Arduino 5V IN
```

⚠️ **PENTING**: 
- RFID RC522 menggunakan **3.3V**, bukan 5V!
- Solenoid lock membutuhkan **12V** dengan arus cukup besar
- Gunakan power supply terpisah untuk solenoid, jangan dari USB Arduino

---

## 📱 Konfigurasi Blynk

### Setup Blynk Template

1. **Buat Template Baru**
   - Login ke [Blynk Console](https://blynk.cloud/)
   - Buat template "SmartLock"
   - Device: ESP32
   - Connection: WiFi

2. **Template ID & Auth Token**
   ```cpp
   #define BLYNK_TEMPLATE_ID "TEMPLATE ID"
   #define BLYNK_TEMPLATE_NAME "YOUR TEMPLATE"
   #define BLYNK_AUTH_TOKEN "YOUR TOKEN"
   ```
   *Ganti dengan Auth Token Anda sendiri*

### Datastream Configuration

| Virtual Pin | Name | Type | Default | Widget | Description |
|-------------|------|------|---------|--------|-------------|
| V0 | Lock Control | Integer | 0 | Switch | 0=Lock, 1=Unlock |
| V1 | Status | String | "LOCKED" | Label | Status real-time (LOCKED/UNLOCKED/RESTRICTED) |
| V2 | Terminal | String | - | Terminal | Log aktivitas sistem |
| V3 | Current PIN | String | "112233" | Label | **PIN Display** - Tampilkan PIN saat ini |
| V4 | New PIN | String | - | Text Input | Input untuk ganti PIN baru |
| V5 | Restricted Mode | Integer | 0 | Switch | **Restricted Mode Control** (0=OFF, 1=ON) |

### Event Configuration

Buat 2 Event di Blynk Console:

#### 1. Event: "info"
- **Name**: `info`
- **Type**: Information
- **Notification**: ✅ Push Notification
- **Description**: Access Granted
- **Example**: "Access granted - RFID: 31E3F216"

#### 2. Event: "warning"  
- **Name**: `warning`
- **Type**: Warning
- **Notification**: ✅ Push Notification
- **Description**: Security Alert
- **Example**: "⚠️ Unknown RFID: 12345678"

### Web Dashboard and APP Layout
|Web|android|
|---|-------|
|<img width="1634" height="835" alt="Screenshot 2025-12-09 182500" src="https://github.com/user-attachments/assets/b8be7ac2-387d-4dec-8e70-2a1435b42ba2" />
| ![WhatsApp Image 2025-12-09 at 18 26 11_0563c7c5](https://github.com/user-attachments/assets/6cb58d02-f781-4409-bbda-373e6c503dcc)
|


### Mobile App Setup

1. Install **Blynk IoT** app (Android/iOS)
2. Login dengan akun yang sama
3. Template otomatis tersinkronisasi
4. Aktifkan Push Notification di pengaturan device

---

## 💾 Instalasi

### 1. Persiapan Arduino IDE

#### Install Board ESP32
```
File → Preferences → Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

```
Tools → Board → Boards Manager
Cari "ESP32" → Install
```

#### Install Library Arduino
Via Library Manager (`Sketch → Include Library → Manage Libraries`):
- LiquidCrystal (built-in)
- Keypad by Mark Stanley
- MFRC522 by GithubCommunity

#### Install Library ESP32
- WiFiManager by tzapu
- Blynk by Volodymyr Shymanskyy
- esp_camera (included in ESP32 board)

### 2. Upload Code

#### Arduino Mega
1. Buka file `SmartLock_Mega.ino`
2. Pilih board: **Arduino Mega 2560**
3. Pilih port COM yang sesuai
4. Klik **Upload**

#### ESP32-CAM
1. Buka file `SmartLock_ESP32CAM.ino`
2. Edit **Auth Token** Anda:
   ```cpp
   #define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"
   ```
3. Pilih board: **AI Thinker ESP32-CAM**
4. Setting:
   - Partition Scheme: **Huge APP (3MB No OTA)**
   - Upload Speed: **115200**
5. Koneksi ESP32-CAM ke programmer:
   - **IO0 → GND** (mode flash)
   - Upload code
   - Lepas IO0 dari GND setelah upload

### 3. WiFi Configuration

Setelah upload ESP32-CAM:

1. ESP32 akan membuat WiFi Access Point:
   ```
   SSID: SmartLock-XXXXXXXX
   Password: smartlock123
   ```

2. Koneksi ke AP tersebut dari smartphone

3. Browser otomatis terbuka (atau buka `192.168.4.1`)

4. Pilih WiFi rumah Anda dan masukkan password

5. ESP32 akan restart dan terhubung ke WiFi

### 4. Mendapatkan Stream URL

Setelah terhubung WiFi, buka Serial Monitor ESP32:
```
✅ WiFi connected
📍 IP: 192.168.1.100
✅ Camera server started
   Stream: http://192.168.1.100:81/stream
```

Salin URL stream dan paste ke **V5 (Browser Button)** di Blynk

---

## 📖 Cara Penggunaan

### 🔓 Unlock Methods

#### Method 1: RFID Card
1. Tempelkan kartu RFID terdaftar
2. Jika terdaftar:
   - LCD: "RFID ACCEPTED!"
   - Buzzer: 2 beep sukses
   - Solenoid unlock
   - Notifikasi info ke Blynk
3. Auto-lock setelah 10 detik

#### Method 2: Keypad PIN
1. Masukkan PIN (default: `112233`)
2. Tekan `*` untuk konfirmasi
3. Jika benar:
   - LCD: "UNLOCKED! Via Keypad"
   - Buzzer: beep sukses
   - Solenoid unlock
   - Notifikasi info ke Blynk
4. Auto-lock setelah 10 detik

#### Method 3: Blynk App
1. Buka Blynk app
2. Toggle switch **V0** ke ON
3. Sistem unlock dari remote
4. Auto-lock setelah 10 detik

### 🔐 Lock Methods

#### Lock via Keypad
- Tunggu 10 detik (auto-lock)

#### Lock via Blynk
- Toggle switch **V0** ke OFF
- Langsung lock

### 🔑 Mengganti PIN

#### Via Keypad
1. Unlock dulu (RFID atau PIN)
2. Tekan `#` (enter mode ganti PIN)
3. LCD: "New PIN:"
4. Ketik PIN baru (4-8 digit)
5. Tekan `*` untuk simpan
6. LCD: "PIN SAVED!"
7. Sistem auto-lock

#### Via Blynk
1. Buka Blynk app
2. Ketik PIN baru di **V4 (Text Input)**
3. PIN otomatis tersimpan
4. Terminal menampilkan konfirmasi

### 📹 Melihat Live Stream

#### Via Blynk App
1. Tekan button **V6 (Security Camera)**
2. Browser terbuka dengan stream URL
3. Lihat live feed dari ESP32-CAM

#### Via Browser Langsung
Buka: `http://[IP_ESP32]:81/stream`

Contoh: `http://192.168.1.100:81/stream`

### 🚨 Restricted Mode

#### Auto-Activation
Restricted Mode akan **otomatis aktif** jika:

1. **3x PIN Salah**
   - Setelah 3x percobaan PIN salah
   - LCD: "RESTRICTED MODE - Use Blynk Only!"
   - LED merah berkedip
   - Buzzer: 3x beep panjang
   - Notifikasi push: "🚨 RESTRICTED MODE: 3x wrong PIN attempts"

2. **3x Unknown RFID**
   - Setelah 3x kartu RFID tidak dikenal
   - LCD: "RESTRICTED MODE - Use Blynk Only!"
   - LED merah berkedip
   - Buzzer: 3x beep panjang
   - Notifikasi push: "🚨 RESTRICTED MODE: 3x unknown RFID attempts"

#### Manual Control via Blynk
1. Buka Blynk app
2. Toggle switch **V5 (Restricted Mode)** ke ON
3. Sistem masuk ke Restricted Mode
4. RFID dan Keypad otomatis disabled
5. Hanya unlock via Blynk yang diizinkan

#### Deactivate Restricted Mode
1. Buka Blynk app
2. Toggle switch **V5** ke OFF
3. Sistem kembali ke Normal Mode
4. RFID dan Keypad kembali aktif

#### Behavior saat Restricted Mode
- ❌ **RFID**: Tidak berfungsi (dibaca tapi diabaikan)
- ❌ **Keypad**: Tidak berfungsi (menampilkan pesan "Use Blynk Only!")
- ✅ **Blynk**: Tetap berfungsi normal
- 🔴 **LED**: Merah berkedip sebagai indikator
- 📱 **Notifikasi**: Push notification saat aktif/nonaktif

### 🔑 PIN Check & Display

#### Melihat PIN Saat Ini
1. Buka Blynk app
2. Lihat widget **V3 (Current PIN)**
3. PIN akan ditampilkan: "🔑 PIN: 112233"
4. PIN otomatis ter-update saat berubah

#### Validasi PIN
Sistem melakukan validasi ketat saat input PIN:
- ✅ **Panjang**: Harus 4-8 digit
- ✅ **Format**: Hanya angka (0-9)
- ❌ **Error Handling**: 
  - Jika < 4 atau > 8 digit → Error: "PIN must be 4-8 digits"
  - Jika mengandung huruf/simbol → Error: "PIN must contain only numbers"

#### Attempt Counter
- Setiap percobaan PIN salah dihitung
- Ditampilkan di LCD: "Attempt: X/3"
- Setelah 3x gagal → Restricted Mode aktif
- Counter reset setelah unlock berhasil

### ⚠️ Security Alerts

Sistem mengirim notifikasi warning saat:

1. **Unknown RFID Card**
   - Kartu tidak terdaftar ditempelkan
   - LCD: "ACCESS DENIED! Unknown Card"
   - LCD: "Attempt: X/3" (menampilkan counter)
   - Buzzer: long beep error
   - Push notification: "⚠️ Unknown RFID: [UID]"
   - Setelah 3x → Restricted Mode aktif

2. **Wrong PIN**
   - PIN salah diinput
   - LCD: "Wrong PIN!"
   - LCD: "Attempt: X/3" (menampilkan counter)
   - Buzzer: error beep
   - Push notification: "⚠️ Wrong PIN attempted!"
   - Setelah 3x → Restricted Mode aktif

3. **Restricted Mode Activated**
   - Push notification: "🚨 RESTRICTED MODE: [reason]"
   - Terminal log: Alasan aktivasi
   - Status di Blynk: "🚨 RESTRICTED"

---

## 🔄 Struktur Komunikasi

### Serial Protocol (Arduino ↔ ESP32)

#### Commands dari ESP32 → Arduino
```
UNLOCK              - Remote unlock
LOCK                - Remote lock
SETPIN:123456       - Set new PIN
STATUS              - Request status
PING                - Connection check
GET_PIN             - Request current PIN
RESTRICTED:ON       - Activate Restricted Mode
RESTRICTED:OFF      - Deactivate Restricted Mode
```

#### Response dari Arduino → ESP32
```
STATUS:LOCKED           - Current status (Locked)
STATUS:UNLOCKED         - Current status (Unlocked)
STATUS:RESTRICTED       - Current status (Restricted Mode)
STATUS:CHANGING_PIN     - Current status (Changing PIN)

EVENT:RFID_OK:31E3F216              - RFID success
EVENT:RFID_DENIED:12345678          - RFID denied
EVENT:KEYPAD_OK                     - PIN success
EVENT:WRONG_PIN                     - PIN wrong
EVENT:RESTRICTED:3x wrong PIN       - Restricted Mode activated
EVENT:RESTRICTED_OFF                - Restricted Mode deactivated
EVENT:PIN_CHANGED:123456            - PIN changed successfully

CURRENT_PIN:112233                  - Current PIN value

OK:PIN_CHANGED                      - Success message
ERROR:PIN must be 4-8 digits        - Error message
ERROR:PIN must be numbers only      - Error message

PONG                                - Response to PING
```

### Blynk Communication

```
ESP32 → Blynk Cloud → Mobile App/Web Dashboard
           ↓
    [Push Notification]
```

**Dataflow:**
1. Arduino deteksi event (RFID/Keypad)
2. Kirim ke ESP32 via Serial
3. ESP32 trigger `Blynk.logEvent()`
4. Blynk Cloud push notification
5. User menerima alert

---

## 🎬 Demo & Hasil

### 📹 Video Demo

[![Video Demo](thumbnail_dari_drive)](https://drive.google.com/file/d/10Kjfyzpf5sxFKVhkpaYQC_wO_D3xNfjp/view?usp=sharing)

**Video menampilkan:**
- ✅ Unlock dengan RFID
- ✅ Unlock dengan Keypad
- ✅ Remote unlock via Blynk
- ✅ Ganti PIN
- ✅ Notifikasi warning
- ✅ Live camera stream

### 📊 Serial Monitor Output

#### Arduino Mega
```
╔═══════════════════════════════════╗
║   Arduino Mega - RESTRICTED MODE ║
╚═══════════════════════════════════╝

MFRC522 Version: 0x92

═══════════════════════════════════
Card detected: 31E3F216
✅ ACCESS GRANTED
═══════════════════════════════════

═══════════════════════════════════
❌ WRONG PIN - Access denied
Entered: 123456
Expected: 112233
Wrong PIN attempts: 2/3
═══════════════════════════════════

╔════════════════════════════════════╗
║  🚨 RESTRICTED MODE ACTIVATED!    ║
╚════════════════════════════════════╝
Reason: 3x wrong PIN attempts

📥 From ESP32: RESTRICTED:ON
📥 From ESP32: STATUS
```

#### ESP32-CAM
```
╔════════════════════════════════════╗
║  ESP32-CAM SMARTLOCK - V3         ║
║  WITH RESTRICTED MODE              ║
╚════════════════════════════════════╝

✅ Camera initialized (stream only)
✅ WiFi connected
📍 IP: 192.168.1.100
✅ Camera server started
   Stream: http://192.168.1.100:81/stream
✅ Blynk connected

📥 Arduino: EVENT:RFID_OK:31E3F216
🔔 Event: RFID_OK
   Data: 31E3F216
✅ RFID Access Granted

📥 Arduino: EVENT:WRONG_PIN
🔔 Event: WRONG_PIN
⚠️ WRONG PIN - Sending notification...

📥 Arduino: EVENT:RESTRICTED:3x wrong PIN attempts
🔔 Event: RESTRICTED
🚨 SYSTEM ENTERED RESTRICTED MODE!

📥 Arduino: CURRENT_PIN:112233
📌 Current PIN updated: 112233
```

---

## 🔧 Troubleshooting

### ❌ Problem: RFID tidak terdeteksi

**Solution:**
1. Cek koneksi SPI (MISO, MOSI, SCK, SDA)
2. Pastikan VCC = **3.3V** (bukan 5V!)
3. Cek Serial Monitor: "MFRC522 Version: 0x92"
   - Jika `0x00` atau `0xFF` → wiring salah
4. Coba ganti kabel jumper
5. Pastikan kartu RFID 13.56MHz (bukan 125kHz)

### ❌ Problem: LCD blank / tidak tampil

**Solution:**
1. Putar potensiometer untuk kontras
2. Cek koneksi VDD (5V) dan VSS (GND)
3. Cek pin RS, E, D4-D7
4. Pastikan RW terhubung ke GND

### ❌ Problem: ESP32-CAM tidak terhubung WiFi

**Solution:**
1. Reset ESP32-CAM
2. Hubungkan ke AP "SmartLock-XXXXXXXX"
3. Konfigurasi ulang WiFi
4. Pastikan WiFi 2.4GHz (bukan 5GHz)
5. Cek jarak dari router

### ❌ Problem: Blynk offline

**Solution:**
1. Cek Auth Token sudah benar
2. Cek koneksi internet ESP32
3. Pastikan Template ID cocok
4. Restart ESP32-CAM
5. Cek status di Blynk Console

### ❌ Problem: Camera stream error

**Solution:**
1. Pastikan IP address benar
2. Port 81 tidak diblock firewall
3. Gunakan browser yang support MJPEG
4. Restart ESP32-CAM
5. Coba akses dari jaringan yang sama

### ❌ Problem: Solenoid tidak aktif

**Solution:**
1. Cek relay LED indikator (harus nyala saat unlock)
2. Cek koneksi relay COM dan NO
3. Pastikan power 12V cukup (min 1A)
4. Cek polaritas solenoid
5. Test relay dengan manual: `digitalWrite(6, LOW)`

### ❌ Problem: Notifikasi tidak muncul

**Solution:**
1. Pastikan event "info" dan "warning" sudah dibuat
2. Cek nama event sama persis (lowercase)
3. Aktifkan push notification di Blynk app
4. Cek internet connection ESP32
5. Test dengan: `Blynk.logEvent("warning", "Test")`

### ❌ Problem: Serial communication error

**Solution:**
1. Arduino → ESP32 harus **cross connection**:
   - Arduino TX1 → ESP32 RX
   - Arduino RX1 → ESP32 TX
2. Cek baud rate sama (9600)
3. Pastikan ground common
4. Cek kabel tidak putus

### ❌ Problem: Restricted Mode tidak bisa dinonaktifkan

**Solution:**
1. Pastikan Blynk app terhubung
2. Toggle switch **V5** ke OFF di Blynk
3. Cek Serial Monitor ESP32: "RESTRICTED:OFF" terkirim
4. Jika masih aktif, reset Arduino Mega
5. Restricted Mode akan reset setelah restart

### ❌ Problem: PIN tidak ter-update di Blynk

**Solution:**
1. Cek koneksi Serial Arduino ↔ ESP32
2. Pastikan command "GET_PIN" terkirim dari ESP32
3. Cek widget **V3** di Blynk sudah dikonfigurasi sebagai Label
4. Restart ESP32-CAM untuk refresh koneksi
5. Cek Serial Monitor: "CURRENT_PIN:..." terkirim

### ❌ Problem: Restricted Mode aktif terus-menerus

**Solution:**
1. Cek apakah ada percobaan gagal yang belum di-reset
2. Unlock via Blynk untuk reset attempt counter
3. Atau nonaktifkan manual via Blynk (V5)
4. Cek Serial Monitor untuk alasan aktivasi
5. Pastikan tidak ada kartu RFID tidak dikenal yang terus dibaca

---

## 📝 Notes

### Default Values
- **Default PIN**: `112233`
- **RFID UID Terdaftar**: `31E3F216`
- **Auto-lock Timer**: 10 detik
- **WiFi AP Password**: `smartlock123`
- **Max Attempts**: 3x (untuk Restricted Mode)
- **Restricted Mode**: Nonaktif (default)

### Keamanan
- Ganti PIN default setelah instalasi
- Tambahkan RFID card di array `allowedUIDs[]`
- Simpan Auth Token dengan aman
- Gunakan WiFi dengan password kuat
- **Restricted Mode** memberikan perlindungan ekstra saat ada percobaan brute force
- PIN ditampilkan di Blynk untuk kemudahan, pertimbangkan untuk mask sebagian digit jika diperlukan

### Restricted Mode Behavior
- **Auto-Activation**: Setelah 3x percobaan gagal (PIN atau RFID)
- **Manual Control**: Via Blynk app (V5)
- **Reset**: Unlock berhasil atau manual via Blynk
- **LED Indicator**: Merah berkedip saat aktif
- **Attempt Counter**: Reset setelah unlock berhasil

### PIN Management
- **Validasi**: 4-8 digit, hanya angka
- **Display**: Tampil di Blynk (V3) secara real-time
- **Change Methods**: Via Blynk (V4) atau Keypad (saat unlocked)
- **Security**: Setiap perubahan PIN tercatat di terminal log

### Modifikasi
Untuk menambah RFID card:
```cpp
String allowedUIDs[] = {
  "31E3F216",
  "ABCD1234",  // Tambahkan UID baru
  "12345678",
};
int totalAllowedCards = 3;  // Update jumlah
```

### Performance
- **Stream FPS**: ~15-20 FPS (SVGA)
- **Response Time**: < 500ms
- **RFID Range**: 3-5 cm
- **WiFi Range**: Tergantung router

---

## 🤝 Contributing

Pull requests are welcome! Untuk perubahan besar, buka issue terlebih dahulu.

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

## 👨‍💻 Author

**NOVIL M**
- Email: mohnovilm@gmail.com

---

## 🙏 Credits

- ESP32-CAM example code modified from official ESP32 Arduino examples
- WiFiManager library by tzapu
- Blynk IoT platform
- MFRC522 library by GithubCommunity
- Keypad library by Mark Stanley


⭐ **Jika project ini membantu, jangan lupa berikan star!** ⭐
