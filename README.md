# Documentación Sistema de Control de Servomotores

### Control Remoto de Servomotores con ESP32 y Firebase
### Descripción General
Este sistema permite controlar remotamente dos servomotores (uno estándar de 180° y otro de rotación continua de 360°) utilizando un microcontrolador ESP32 conectado a una base de datos en tiempo real Firebase. El control de activación/desactivación de los servomotores se realiza a través de una variable booleana almacenada en Firebase.

### Componentes de Hardware
- **ESP32:** Microcontrolador principal.
- **Servomotor estándar (180°):** Conectado al pin 13 del ESP32.
- **Servomotor de rotación continua (360°):** Conectado al pin 12 del ESP32.
- **Fuente de alimentación:** Adecuada para el ESP32 y los servomotores.

### Librerías Utilizadas

1. **Arduino.h:** Biblioteca estándar para programación de Arduino/ESP32.
2. **WiFiManager.h:** Facilita la configuración WiFi con portal cautivo.
3. **Firebase_ESP_Client.h:** Cliente Firebase para ESP32.
4. **TokenHelper.h:** Asistente para manejo de tokens de autenticación Firebase.
5. **RTDBHelper.h:** Funciones auxiliares para la base de datos en tiempo real.
6. **ESP32Servo.h:** Control de servomotores con ESP32.

## Configuración de Pines
```
#define SERVO_PIN 13       // Pin para servomotor de 180°
#define SERVO360_PIN 12    // Pin para servomotor de 360°
```

## Configuración de Firebase
```
#define API_KEY "AIzaSyDcqO9D9FCRSI0CBzEW87FbN4GV8zZpxeI"
#define DATABASE_URL "https://servocontrol2-c5a8f-default-rtdb.firebaseio.com/"
```

## Ruta en la Base de Datos
```
const char* SERVO_PATH = "/SERVO/servo1";   // Ruta donde se almacena el estado booleano
```

## Variables Globales

- miServo: Objeto para controlar el servomotor de 180°.
- servo360: Objeto para controlar el servomotor de rotación continua.
- fbdo_s: Objeto para manejar los datos de Firebase.
- auth: Objeto para autenticación en Firebase.
- config: Objeto para configuración de Firebase.
- signupOK: Estado de registro en Firebase.
- servoON: Estado actual de activación de los servomotores (valor leído desde Firebase).

## Función Setup
La función setup() se ejecuta una sola vez al iniciar el dispositivo y configura:

1. **Modo WiFi:** Configura el ESP32 como estación (cliente).
2. **Comunicación Serial:** Inicializa la comunicación a 115200 baudios.
3. **Configuración WiFi:**
   1. Utiliza WiFiManager para crear un portal cautivo ("AutoConnectAP_esp32" con contraseña "12345678").
   2. Si no se puede conectar a una red, el usuario puede configurar las credenciales mediante este portal.


## Configuración de Servomotores:

- Asigna timers para los servomotores.
- Establece la frecuencia a 50Hz (común para servomotores).
- Configura el rango de pulso de 500 a 2400 microsegundos.
- Inicializa los servomotores en posición reposo:
  - Servo 180° a posición 0°
  - Servo 360° a posición 94 (punto muerto para detenerlo)




## Configuración de Firebase:

- Establece la clave API y URL de la base de datos.
- Realiza el registro en Firebase.
- Configura el callback para el estado del token.
- Inicia la conexión con Firebase.
- Activa la reconexión automática.


## Iniciación de Stream:

- Configura un stream para monitorear en tiempo real la ruta SERVO_PATH.


## Comportamiento del Sistema

1. Al iniciar:

- El sistema crea un punto de acceso WiFi para configuración si no hay redes guardadas.
- Conecta a Firebase y establece los servomotores en posición inicial.


2. Durante la operación:

- Monitorea continuamente el valor booleano en la ruta SERVO_PATH de Firebase.
- Cuando el valor es true:
  - El servomotor de 180° realiza un barrido oscilante entre 0° y 180°.
  - El servomotor de 360° gira continuamente.

- Cuando el valor es false:
  - El servomotor de 180° se mantiene en posición 0°.
  - El servomotor de 360° se detiene.



## Notas de Implementación

- El sistema utiliza WiFiManager para facilitar la conexión WiFi sin necesidad de codificar credenciales.
- La biblioteca ESP32Servo está optimizada para el uso de servomotores con ESP32.
- El control de tiempo para el movimiento del servo de 180° evita movimientos bruscos y sobrecargas.
- El valor 94 para el servo de 360° es el punto muerto típico, pero puede requerir ajustes según el modelo específico del servomotor.
