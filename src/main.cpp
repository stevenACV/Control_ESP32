#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <ESP32Servo.h>

Servo miServo;      // Servo de 180°
Servo servo360;     // Servo de rotación continua (360°)

#define SERVO_PIN 13
#define SERVO360_PIN 12

#define API_KEY "AIzaSyDbiKAyB-qugi2JITiKIoSGGTgUyysQRd4"
#define DATABASE_URL "https://solsicontrol-default-rtdb.firebaseio.com/"

FirebaseData fbdo_s;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
bool servoON = false;

const char* SERVO_PATH = "/SERVO/servo1";

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFiManager wm;

  // Configurar los servos
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  miServo.setPeriodHertz(50);
  servo360.setPeriodHertz(50);

  miServo.attach(SERVO_PIN, 500, 2400);
  servo360.attach(SERVO360_PIN, 500, 2400);

  miServo.write(0);
  servo360.write(94);  // 90-94 suele ser el "detenido" para servos 360

  // Conexión Wi-Fi
  wm.resetSettings();
  bool res;
  res = wm.autoConnect("AutoConnectAP_esp32","12345678"); 
  if(!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Registro en Firebase exitoso");
    signupOK = true;
  } else {
    Serial.printf("Error de registro: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Iniciar stream para la ruta servo1
  if (!Firebase.RTDB.beginStream(&fbdo_s, SERVO_PATH)) {
    Serial.printf("Error de stream (servo1): %s\n", fbdo_s.errorReason().c_str());
  }

  Serial.println("Sistema listo");
}

void loop() {
  // Leer estado desde Firebase
  if (!Firebase.RTDB.readStream(&fbdo_s)) {
    Serial.println("Error de stream servo1: " + fbdo_s.errorReason());
  } else if (fbdo_s.streamAvailable() && fbdo_s.dataType() == "boolean") {
    servoON = fbdo_s.boolData();
    Serial.println("servo1 (180° y 360°): " + String(servoON));
  }

  // ---------------- Movimiento servo 180° ----------------
  static unsigned long lastMoveTime = 0;
  static int servo1Pos = 0;
  static bool subiendo = true;

  if (servoON) {
    if (millis() - lastMoveTime > 10) {
      miServo.write(servo1Pos);
      lastMoveTime = millis();
      if (subiendo) {
        servo1Pos++;
        if (servo1Pos >= 180) subiendo = false;
      } else {
        servo1Pos--;
        if (servo1Pos <= 0) subiendo = true;
      }
    }
  } else {
    miServo.write(0);  // posición fija
  }

  // ---------------- Movimiento servo 360° ----------------
  if (servoON) {
    servo360.write(120);  // gira
  } else {
    servo360.write(94);   // detener
  }
}