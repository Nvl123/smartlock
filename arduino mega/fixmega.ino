/*
 * Arduino Mega - SmartLock (LOGIC FIXED)
 * 
 * PERBAIKAN:
 * ✅ Fixed: LCD tidak salah tampil saat RFID denied
 * ✅ Fixed: Terminal tidak nampil PIN baru saat RFID denied
 * ✅ Simplified: Event notification only (no photo capture)
 */

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

// ============= RFID RC522 =======================
#define SS_PIN 53
#define RST_PIN 8
MFRC522 rfid(SS_PIN, RST_PIN);

String allowedUIDs[] = {
  "31E3F216",
};
int totalAllowedCards = 1;

// ============= LCD 16x2 PARALLEL ==============
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ============= BUZZER ==========================
#define BUZZER_PIN 7

// ============= RELAY SOLENOID ==================
#define RELAY_PIN 6

// ============= KEYPAD ==========================
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ============= SERIAL COMMUNICATION =============
#define ESP32_SERIAL Serial1

// ============= VARIABLES ========================
String defaultPin = "112233";
String enteredPin = "";
String newPin = "";

unsigned long lastUnlockTime = 0;
unsigned long lastRFIDReadTime = 0;
String lastCardUID = "";
unsigned long lastStatusSend = 0;

enum State { LOCK, UNLOCK, CHANGE_PIN };
State currentState = LOCK;

String serialBuffer = "";

void setup() {
  Serial.begin(115200);
  ESP32_SERIAL.begin(9600);
  
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  SPI.begin();
  rfid.PCD_Init();
  
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print("MFRC522 Version: 0x");
  Serial.println(v, HEX);
  
  if (v == 0x00 || v == 0xFF) {
    Serial.println("ERROR: RC522 tidak terdeteksi!");
    lcd.clear();
    lcd.print("RFID ERROR!");
    delay(2000);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(30, OUTPUT);  // LED Merah
  pinMode(31, OUTPUT);  // LED Hijau

  digitalWrite(RELAY_PIN, HIGH);

  delay(1000);
  showState();
  sendStatusToESP32();
  
  Serial.println("\n╔═══════════════════════════════════╗");
  Serial.println("║   Arduino Mega Ready - FIXED     ║");
  Serial.println("╚═══════════════════════════════════╝\n");
}

void loop() {
  char key = keypad.getKey();

  updateIndicator();

  // ✅ RFID hanya dicek saat LOCKED
  if (currentState == LOCK) {
    checkRFID();
  }

  if (key) handleKey(key);

  readESP32Command();

  // Auto-lock setelah 10 detik
  if (currentState == UNLOCK && (millis() - lastUnlockTime > 10000)) {
    currentState = LOCK;
    updateSolenoid();
    
    lastCardUID = "";
    lastRFIDReadTime = 0;
    
    Serial.println("=== AUTO-LOCKED ===");
    showState();
    sendStatusToESP32();
  }

  // Kirim status setiap 5 detik
  if (millis() - lastStatusSend > 5000) {
    sendStatusToESP32();
    lastStatusSend = millis();
  }
}

// ============= KOMUNIKASI ESP32 =============
void readESP32Command() {
  while (ESP32_SERIAL.available()) {
    char c = ESP32_SERIAL.read();
    
    if (c == '\n') {
      if (serialBuffer.length() > 0) {
        processESP32Command(serialBuffer);
      }
      serialBuffer = "";
    } else if (c != '\r') {
      serialBuffer += c;
    }
  }
}

void processESP32Command(String cmd) {
  cmd.trim();
  Serial.print("📥 From ESP32: ");
  Serial.println(cmd);

  int colonPos = cmd.indexOf(':');
  String command = cmd;
  String param = "";
  
  if (colonPos > 0) {
    command = cmd.substring(0, colonPos);
    param = cmd.substring(colonPos + 1);
  }

  // ===== UNLOCK dari Blynk =====
  if (command == "UNLOCK") {
    if (currentState == LOCK) {
      currentState = UNLOCK;
      lastUnlockTime = millis();
      updateSolenoid();
      
      lcd.clear();
      lcd.print("UNLOCKED!");
      lcd.setCursor(0, 1);
      lcd.print("Via Blynk");
      
      tone(BUZZER_PIN, 1000, 300);
      delay(300);
      delay(1500);
      showState();
      
      sendStatusToESP32();
      Serial.println("✅ Unlocked via Blynk");
    }
  }
  
  // ===== LOCK dari Blynk =====
  else if (command == "LOCK") {
    if (currentState == UNLOCK || currentState == CHANGE_PIN) {
      currentState = LOCK;
      updateSolenoid();
      
      lastCardUID = "";
      lastRFIDReadTime = 0;
      
      lcd.clear();
      lcd.print("LOCKED!");
      lcd.setCursor(0, 1);
      lcd.print("Via Blynk");
      
      tone(BUZZER_PIN, 800, 300);
      delay(300);
      delay(1500);
      showState();
      
      sendStatusToESP32();
      Serial.println("✅ Locked via Blynk");
    }
  }
  
  // ===== SET PIN dari Blynk =====
  else if (command == "SETPIN") {
    Serial.println("🔑 Processing PIN change: " + param);
    
    if (param.length() >= 4 && param.length() <= 8) {
      bool valid = true;
      for (int i = 0; i < param.length(); i++) {
        if (!isDigit(param[i])) {
          valid = false;
          break;
        }
      }
      
      if (valid) {
        defaultPin = param;
        
        lcd.clear();
        lcd.print("PIN Changed!");
        lcd.setCursor(0, 1);
        lcd.print("New: ");
        lcd.print(param);
        
        tone(BUZZER_PIN, 1200, 300);
        delay(300);
        delay(2000);
        showState();
        
        ESP32_SERIAL.println("OK:PIN_CHANGED");
        Serial.println("✅ PIN changed to: " + param);
      } else {
        ESP32_SERIAL.println("ERROR:PIN must be numbers only");
        Serial.println("❌ Invalid PIN format");
      }
    } else {
      ESP32_SERIAL.println("ERROR:PIN must be 4-8 digits");
      Serial.println("❌ Invalid PIN length");
    }
  }
  
  // ===== STATUS Request =====
  else if (command == "STATUS") {
    sendStatusToESP32();
  }
  
  // ===== PING =====
  else if (command == "PING") {
    ESP32_SERIAL.println("PONG");
  }
}

void sendStatusToESP32() {
  String status = "STATUS:";
  
  if (currentState == LOCK) {
    status += "LOCKED";
  } else if (currentState == UNLOCK) {
    status += "UNLOCKED";
  } else {
    status += "CHANGING_PIN";
  }
  
  ESP32_SERIAL.println(status);
}

// ============= RFID (FIXED LOGIC) =============
void checkRFID() {
  rfid.PCD_Init();
  
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String cardUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) cardUID += "0";
    cardUID += String(rfid.uid.uidByte[i], HEX);
  }
  cardUID.toUpperCase();

  Serial.println("\n═══════════════════════════════════");
  Serial.print("Card detected: ");
  Serial.println(cardUID);

  // Debounce - cegah baca kartu yang sama berulang kali
  if (cardUID == lastCardUID && (millis() - lastRFIDReadTime < 1500)) {
    Serial.println("Same card - ignoring (debounce)");
    Serial.println("═══════════════════════════════════\n");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  lastRFIDReadTime = millis();
  lastCardUID = cardUID;

  // Cek izin akses
  bool isAllowed = false;
  for (int i = 0; i < totalAllowedCards; i++) {
    if (cardUID == allowedUIDs[i]) {
      isAllowed = true;
      break;
    }
  }

  if (isAllowed) {
    // ✅ KARTU TERDAFTAR - UNLOCK
    Serial.println("✅ ACCESS GRANTED");
    Serial.println("═══════════════════════════════════\n");
    
    currentState = UNLOCK;
    lastUnlockTime = millis();
    updateSolenoid();
    
    // LCD tampil RFID ACCEPTED
    lcd.clear();
    lcd.print("RFID ACCEPTED!");
    lcd.setCursor(0, 1);
    lcd.print(cardUID.substring(0, 8));
    
    // Buzzer sukses (2 beep)
    tone(BUZZER_PIN, 1200, 150);
    delay(150);
    tone(BUZZER_PIN, 1500, 150);
    delay(150);
    
    // Kirim event ke ESP32
    ESP32_SERIAL.println("EVENT:RFID_OK:" + cardUID);
    sendStatusToESP32();
    
    delay(1500);
    showState();
  }
  else {
    // ❌ KARTU TIDAK TERDAFTAR - DENIED
    Serial.println("❌ ACCESS DENIED - Unknown card");
    Serial.println("═══════════════════════════════════\n");
    
    // LCD tampil ACCESS DENIED (BUKAN PIN!)
    lcd.clear();
    lcd.print("ACCESS DENIED!");
    lcd.setCursor(0, 1);
    lcd.print("Unknown Card");
    
    // Buzzer error (1 long beep)
    tone(BUZZER_PIN, 400, 600);
    delay(600);
    
    // Kirim event ke ESP32 (untuk notifikasi warning)
    ESP32_SERIAL.println("EVENT:RFID_DENIED:" + cardUID);
    
    delay(1500);
    showState(); // Kembali ke tampilan normal
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ============= KEYPAD =============
void handleKey(char key) {

  if (currentState == LOCK) {

    if (key == '*') {
      if (enteredPin == defaultPin) {
        // ✅ PIN BENAR - UNLOCK
        Serial.println("\n═══════════════════════════════════");
        Serial.println("✅ CORRECT PIN - Access granted");
        Serial.println("═══════════════════════════════════\n");
        
        currentState = UNLOCK;
        lastUnlockTime = millis();
        updateSolenoid();
        
        lastCardUID = "";
        lastRFIDReadTime = 0;
        
        lcd.clear(); 
        lcd.print("UNLOCKED!");
        lcd.setCursor(0, 1);
        lcd.print("Via Keypad");
        
        tone(BUZZER_PIN, 1000, 300);
        delay(300);
        
        ESP32_SERIAL.println("EVENT:KEYPAD_OK");
        sendStatusToESP32();
        
        delay(1500);
        showState();
      } else {
        // ❌ PIN SALAH
        Serial.println("\n═══════════════════════════════════");
        Serial.println("❌ WRONG PIN - Access denied");
        Serial.println("Entered: " + enteredPin);
        Serial.println("Expected: " + defaultPin);
        Serial.println("═══════════════════════════════════\n");
        
        lcd.clear(); 
        lcd.print("Wrong PIN!");
        
        tone(BUZZER_PIN, 500, 400);
        delay(400);
        
        // Kirim event untuk notifikasi warning
        ESP32_SERIAL.println("EVENT:WRONG_PIN");
        
        delay(1000);
        showState();
      }
      enteredPin = "";
    }

    else if (key == '#') {
      enteredPin = "";
      lcd.clear();
      lcd.print("Enter PIN:");
    }

    else {
      enteredPin += key;
      lcd.clear();
      lcd.print("PIN:");
      lcd.setCursor(0,1);
      lcd.print(enteredPin);
    }
  }

  else if (currentState == UNLOCK) {
    if (key == '#') {
      currentState = CHANGE_PIN;
      newPin = "";
      lcd.clear();
      lcd.print("New PIN:");
      Serial.println("\n>>> Entering CHANGE_PIN mode");
    }
  }

  else if (currentState == CHANGE_PIN) {

    if (key == '*') {
      defaultPin = newPin;

      Serial.println("\n═══════════════════════════════════");
      Serial.println("✅ PIN CHANGED via Keypad");
      Serial.println("New PIN: " + defaultPin);
      Serial.println("═══════════════════════════════════\n");

      lcd.clear(); 
      lcd.print("PIN SAVED!");
      tone(BUZZER_PIN, 1200, 300);
      delay(300);
      delay(1000);

      currentState = LOCK;
      updateSolenoid();
      
      lastCardUID = "";
      lastRFIDReadTime = 0;
      
      sendStatusToESP32();
      showState();
    }

    else if (key == '#') {
      newPin = "";
      lcd.clear();
      lcd.print("New PIN:");
    }

    else {
      newPin += key;
      lcd.clear();
      lcd.print("New PIN:");
      lcd.setCursor(0,1);
      lcd.print(newPin);
    }
  }
}

void updateIndicator() {
  if (currentState == LOCK) {
    digitalWrite(30, HIGH);  // LED Merah ON
    digitalWrite(31, LOW);   // LED Hijau OFF
  }
  else if (currentState == UNLOCK) {
    digitalWrite(30, LOW);   // LED Merah OFF
    digitalWrite(31, HIGH);  // LED Hijau ON
  }
  else {
    digitalWrite(30, HIGH);  // LED Merah ON (changing PIN)
    digitalWrite(31, LOW);
  }
}

void updateSolenoid() {
  if (currentState == UNLOCK) {
    digitalWrite(RELAY_PIN, LOW);  // Solenoid aktif (unlock)
  }
  else {
    digitalWrite(RELAY_PIN, HIGH); // Solenoid OFF (lock)
  }
}

void showState() {
  lcd.clear();
  if (currentState == LOCK) {
    lcd.print("Enter PIN/Card");
    lcd.setCursor(0, 1);
    lcd.print("to unlock");
  }
  else if (currentState == UNLOCK) {
    lcd.print("UNLOCKED");
    lcd.setCursor(0, 1);
    lcd.print("Press # to edit");
  }
  else {
    lcd.print("New PIN:");
  }
}