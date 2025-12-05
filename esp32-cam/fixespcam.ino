/*
 * File: SmartLock_ESP32CAM.ino
 * ESP32-CAM Smartlock - NOTIFICATION ONLY VERSION
 * 
 * PERUBAHAN:
 * ✅ Hapus semua fungsi capture foto
 * ✅ Fokus ke notifikasi warning saja
 * ✅ Event names yang benar: "info" dan "warning"
 */

#define BLYNK_TEMPLATE_ID "YOUR TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE"
#define BLYNK_AUTH_TOKEN "YOUR TOKEN"

#define BLYNK_PRINT Serial

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include "board_config.h"

// ============= Camera & Server =============
void startCameraServer();
void setupLedFlash();

// ============= Serial Communication =============
#define MEGA_SERIAL Serial

// ============= Variables =============
String serialBuffer = "";
String currentStatus = "LOCKED";
unsigned long lastPing = 0;
unsigned long lastStatusRequest = 0;
bool megaConnected = false;
bool blynkConnected = false;
bool cameraInitialized = false;

String streamURL = "";
String captureURL = "";

BlynkTimer timer;

// ============= BLYNK CALLBACKS =============

// V0 - Button Lock/Unlock
BLYNK_WRITE(V0) {
  int buttonState = param.asInt();
  
  if (buttonState == 1) {
    MEGA_SERIAL.println("UNLOCK");
    Serial.println("📱 Blynk: UNLOCK command sent");
    Blynk.virtualWrite(V2, "[Blynk] Unlock command sent\n");
  } else {
    MEGA_SERIAL.println("LOCK");
    Serial.println("📱 Blynk: LOCK command sent");
    Blynk.virtualWrite(V2, "[Blynk] Lock command sent\n");
  }
}

// V3 - Button Set PIN
BLYNK_WRITE(V3) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    Blynk.virtualWrite(V2, "[Info] Enter new PIN (4-8 digits) in text field below\n");
  }
}

// V4 - Text Input untuk PIN baru
BLYNK_WRITE(V4) {
  String newPin = param.asStr();
  newPin.trim();
  
  Serial.println("📱 Blynk: New PIN received: " + newPin);
  
  if (newPin.length() < 4 || newPin.length() > 8) {
    Blynk.virtualWrite(V2, "[❌] PIN must be 4-8 digits\n");
    Serial.println("❌ Invalid length");
    return;
  }
  
  for (int i = 0; i < newPin.length(); i++) {
    if (!isDigit(newPin[i])) {
      Blynk.virtualWrite(V2, "[❌] PIN must contain only numbers\n");
      Serial.println("❌ Contains non-digit");
      return;
    }
  }
  
  MEGA_SERIAL.println("SETPIN:" + newPin);
  Serial.println("✅ Sending SETPIN to Arduino: " + newPin);
  Blynk.virtualWrite(V2, "[⏳] Setting new PIN: " + newPin + "...\n");
}

BLYNK_CONNECTED() {
  blynkConnected = true;
  Blynk.syncVirtual(V0, V4);
  
  if (cameraInitialized) {
    Blynk.virtualWrite(V2, "\n╔═══ ESP32-CAM CONNECTED ═══╗\n");
    Blynk.virtualWrite(V2, "Stream: " + streamURL + "\n");
    Blynk.virtualWrite(V2, "╚════════════════════════════╝\n\n");
  } else {
    Blynk.virtualWrite(V2, "\n[System] ESP32-CAM connected\n\n");
  }
  
  MEGA_SERIAL.println("STATUS");
  Serial.println("✅ Blynk connected");
}

// ============= SETUP =============
void setup() {
  Serial.begin(9600);
  delay(100);
  
  Serial.println("\n\n╔════════════════════════════════════╗");
  Serial.println("║  ESP32-CAM SMARTLOCK - NOTIF ONLY ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // ===== CAMERA INITIALIZATION (untuk stream saja) =====
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed: 0x%x\n", err);
    cameraInitialized = false;
  } else {
    cameraInitialized = true;
    Serial.println("✅ Camera initialized (stream only)");
    
    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1);
      s->set_brightness(s, 1);
      s->set_saturation(s, -2);
    }
    
    if (config.pixel_format == PIXFORMAT_JPEG) {
      s->set_framesize(s, FRAMESIZE_SVGA);
    }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
#endif
  }

#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  // ===== WIFI MANAGER SETUP =====
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  
  WiFiManagerParameter custom_blynk_token("blynk", "Blynk Auth Token", BLYNK_AUTH_TOKEN, 34);
  wifiManager.addParameter(&custom_blynk_token);
  
  String apName = "SmartLock-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  
  Serial.println("📡 WiFiManager starting...");
  Serial.println("   AP: " + apName);
  Serial.println("   Pass: smartlock123");
  
  if (!wifiManager.autoConnect(apName.c_str(), "smartlock123")) {
    Serial.println("❌ WiFi failed");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println("✅ WiFi connected");
  Serial.print("📍 IP: ");
  Serial.println(WiFi.localIP());
  
  WiFi.setSleep(false);

  // ===== BUILD URLs =====
  String localIP = WiFi.localIP().toString();
  streamURL = "http://" + localIP + ":81/stream";

  // ===== START CAMERA SERVER (untuk stream) =====
  if (cameraInitialized) {
    startCameraServer();
    Serial.println("✅ Camera server started");
    Serial.println("   Stream: " + streamURL);
  }

  // ===== BLYNK SETUP =====
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
  
  timer.setInterval(10000L, pingMega);
  timer.setInterval(3000L, requestStatus);
  timer.setInterval(1000L, checkBlynkConnection);
  
  delay(1000);
  MEGA_SERIAL.println("STATUS");
  
  Serial.println("\n✅ System ready!\n");
}

// ============= LOOP =============
void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
  readMegaSerial();
  delay(10);
}

// ============= SERIAL COMMUNICATION =============
void readMegaSerial() {
  while (MEGA_SERIAL.available()) {
    char c = MEGA_SERIAL.read();
    
    if (c == '\n') {
      if (serialBuffer.length() > 0) {
        processMegaMessage(serialBuffer);
      }
      serialBuffer = "";
    } else if (c != '\r') {
      serialBuffer += c;
    }
  }
}

void processMegaMessage(String msg) {
  msg.trim();
  if (msg.length() == 0) return;
  
  Serial.println("📥 Arduino: " + msg);
  
  int colonPos = msg.indexOf(':');
  String command = msg;
  String param = "";
  
  if (colonPos > 0) {
    command = msg.substring(0, colonPos);
    param = msg.substring(colonPos + 1);
  }
  
  if (command == "STATUS") {
    currentStatus = param;
    updateBlynkStatus(param);
    megaConnected = true;
  }
  else if (command == "EVENT") {
    handleEvent(param);
  }
  else if (command == "OK") {
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "[✅] " + param + "\n");
    }
    Serial.println("✅ OK: " + param);
  }
  else if (command == "ERROR") {
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "[❌] " + param + "\n");
    }
    Serial.println("❌ Error: " + param);
  }
  else if (command == "PONG") {
    megaConnected = true;
  }
}

// ============= EVENT HANDLER - NOTIFICATION ONLY =============
void handleEvent(String eventData) {
  int colonPos = eventData.indexOf(':');
  String eventType = eventData;
  String eventParam = "";
  
  if (colonPos > 0) {
    eventType = eventData.substring(0, colonPos);
    eventParam = eventData.substring(colonPos + 1);
  }
  
  Serial.println("🔔 Event: " + eventType);
  if (eventParam.length() > 0) {
    Serial.println("   Data: " + eventParam);
  }
  
  // ===== RFID SUCCESS =====
  if (eventType == "RFID_OK") {
    Serial.println("✅ RFID Access Granted");
    
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "═══════════════════════════\n");
      Blynk.virtualWrite(V2, "✅ RFID ACCESS GRANTED\n");
      Blynk.virtualWrite(V2, "Card: " + eventParam + "\n");
      Blynk.virtualWrite(V2, "═══════════════════════════\n\n");
      
      // Event name di Blynk Console: "info"
      Blynk.logEvent("info", "Access granted - RFID: " + eventParam);
    }
    
    updateBlynkStatus("UNLOCKED");
  }
  
  // ===== RFID DENIED - WARNING NOTIFICATION ONLY =====
  else if (eventType == "RFID_DENIED") {
    Serial.println("⚠️ RFID DENIED - Sending notification...");
    
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "═══════════════════════════\n");
      Blynk.virtualWrite(V2, "⚠️ SECURITY ALERT!\n");
      Blynk.virtualWrite(V2, "Unknown RFID: " + eventParam + "\n");
      Blynk.virtualWrite(V2, "═══════════════════════════\n\n");
      
      // ✅ Event name di Blynk Console: "warning"
      Blynk.logEvent("warning", "⚠️ Unknown RFID: " + eventParam);
    }
  }
  
  // ===== KEYPAD SUCCESS =====
  else if (eventType == "KEYPAD_OK") {
    Serial.println("✅ Keypad Access Granted");
    
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "═══════════════════════════\n");
      Blynk.virtualWrite(V2, "✅ KEYPAD ACCESS GRANTED\n");
      Blynk.virtualWrite(V2, "Correct PIN entered\n");
      Blynk.virtualWrite(V2, "═══════════════════════════\n\n");
      
      // Event name di Blynk Console: "info"
      Blynk.logEvent("info", "Access granted - Keypad PIN");
    }
    
    updateBlynkStatus("UNLOCKED");
  }
  
  // ===== WRONG PIN - WARNING NOTIFICATION ONLY =====
  else if (eventType == "WRONG_PIN") {
    Serial.println("⚠️ WRONG PIN - Sending notification...");
    
    if (blynkConnected) {
      Blynk.virtualWrite(V2, "═══════════════════════════\n");
      Blynk.virtualWrite(V2, "⚠️ SECURITY ALERT!\n");
      Blynk.virtualWrite(V2, "Wrong PIN attempted\n");
      Blynk.virtualWrite(V2, "═══════════════════════════\n\n");
      
      // ✅ Event name di Blynk Console: "warning"
      Blynk.logEvent("warning", "⚠️ Wrong PIN attempted!");
    }
  }
}

// ============= BLYNK STATUS UPDATE =============
void updateBlynkStatus(String status) {
  if (!blynkConnected) return;
  
  currentStatus = status;
  
  if (status == "LOCKED") {
    Blynk.virtualWrite(V1, "🔒 LOCKED");
    Blynk.setProperty(V1, "color", "#D3435C");
    Blynk.virtualWrite(V0, 0);
  }
  else if (status == "UNLOCKED") {
    Blynk.virtualWrite(V1, "🔓 UNLOCKED");
    Blynk.setProperty(V1, "color", "#23C48E");
    Blynk.virtualWrite(V0, 1);
  }
  else {
    Blynk.virtualWrite(V1, "⚙️ " + status);
    Blynk.setProperty(V1, "color", "#FFA500");
  }
}

// ============= TIMER FUNCTIONS =============
void pingMega() {
  MEGA_SERIAL.println("PING");
  
  if (!megaConnected && blynkConnected) {
    Blynk.virtualWrite(V2, "[⚠️ Warning] Arduino connection lost\n\n");
  }
  megaConnected = false;
}

void requestStatus() {
  MEGA_SERIAL.println("STATUS");
}

void checkBlynkConnection() {
  if (Blynk.connected() && !blynkConnected) {
    blynkConnected = true;
    Serial.println("✅ Blynk reconnected");
  } else if (!Blynk.connected() && blynkConnected) {
    blynkConnected = false;
    Serial.println("❌ Blynk disconnected");
  }
}