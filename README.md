# 🔐 Smart Lock System with IoT

Smart Lock berbasis Arduino Mega dan ESP32-CAM dengan fitur RFID, Keypad, dan monitoring via Blynk IoT platform.

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
- **Keypad PIN** - Input PIN 4-8 digit
- **Blynk App** - Remote unlock dari smartphone

### 🔔 Notifikasi Real-time
- ✅ **Info Notification** - Akses berhasil (RFID/Keypad)
- ⚠️ **Warning Notification** - Akses ditolak (Unknown RFID/Wrong PIN)
- Push notification ke smartphone & web

### 📹 Monitoring
- Live video streaming dari ESP32-CAM
- Terminal log aktivitas
- Status lock real-time di Blynk app

### 🔧 Manajemen PIN
- Ganti PIN via Blynk app
- Ganti PIN via Keypad (mode unlock)
- Validasi PIN 4-8 digit

### 🔒 Keamanan
- Auto-lock setelah 10 detik
- RFID debouncing (mencegah pembacaan berulang)
- LED indikator (Merah: Locked, Hijau: Unlocked)
- Buzzer feedback untuk setiap aktivitas

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

![Skema Wiring](https://drive.google.com/file/d/10Kjfyzpf5sxFKVhkpaYQC_wO_D3xNfjp/view?usp=sharing)

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

| Virtual Pin | Name | Type | Default | Widget |
|-------------|------|------|---------|--------|
| V0 | Lock Control | Integer | 0 | Switch (0=Lock, 1=Unlock) |
| V1 | Status | String | "LOCKED" | Label |
| V2 | Terminal | String | - | Terminal |
| V4 | New PIN | String | - | Text Input |
| V5 | Security Camera | String | - | Browser Button (Stream URL) |

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
|<img width="1918" height="927" alt="Screenshot 2025-12-05 162938" src="https://github.com/user-attachments/assets/2bf12aec-424e-4113-b4d8-5a4f0dea84a5" /> | ![WhatsApp Image 2025-12-05 at 16 30 47_fa259e20](https://github.com/user-attachments/assets/28acc727-2452-4d38-a9de-ce28003bb7e7)|


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
1. Tekan button **V5 (Security Camera)**
2. Browser terbuka dengan stream URL
3. Lihat live feed dari ESP32-CAM

#### Via Browser Langsung
Buka: `http://[IP_ESP32]:81/stream`

Contoh: `http://192.168.1.100:81/stream`

### ⚠️ Security Alerts

Sistem mengirim notifikasi warning saat:

1. **Unknown RFID Card**
   - Kartu tidak terdaftar ditempelkan
   - LCD: "ACCESS DENIED! Unknown Card"
   - Buzzer: long beep error
   - Push notification: "⚠️ Unknown RFID"

2. **Wrong PIN**
   - PIN salah diinput
   - LCD: "Wrong PIN!"
   - Buzzer: error beep
   - Push notification: "⚠️ Wrong PIN attempted"

---

## 🔄 Struktur Komunikasi

### Serial Protocol (Arduino ↔ ESP32)

#### Commands dari ESP32 → Arduino
```
UNLOCK          - Remote unlock
LOCK            - Remote lock
SETPIN:123456   - Set new PIN
STATUS          - Request status
PING            - Connection check
```

#### Response dari Arduino → ESP32
```
STATUS:LOCKED           - Current status
STATUS:UNLOCKED
STATUS:CHANGING_PIN

EVENT:RFID_OK:31E3F216        - RFID success
EVENT:RFID_DENIED:12345678    - RFID denied
EVENT:KEYPAD_OK               - PIN success
EVENT:WRONG_PIN               - PIN wrong

OK:PIN_CHANGED                - Success message
ERROR:PIN must be 4-8 digits  - Error message

PONG                          - Response to PING
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
║   Arduino Mega Ready - FIXED     ║
╚═══════════════════════════════════╝

MFRC522 Version: 0x92

═══════════════════════════════════
Card detected: 31E3F216
✅ ACCESS GRANTED
═══════════════════════════════════

📥 From ESP32: STATUS
```

#### ESP32-CAM
```
╔════════════════════════════════════╗
║  ESP32-CAM SMARTLOCK - NOTIF ONLY ║
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

---

## 📝 Notes

### Default Values
- **Default PIN**: `112233`
- **RFID UID Terdaftar**: `31E3F216`
- **Auto-lock Timer**: 10 detik
- **WiFi AP Password**: `smartlock123`

### Keamanan
- Ganti PIN default setelah instalasi
- Tambahkan RFID card di array `allowedUIDs[]`
- Simpan Auth Token dengan aman
- Gunakan WiFi dengan password kuat

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
