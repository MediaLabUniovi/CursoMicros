/*
 * ==============================================================================
 * PROYECTO: SENSOR DE APARCAMIENTO CON ESP32
 * MediaLab UniOvi - Taller de Microcontroladores
 * ==============================================================================
 * Lee distancia con sensor HC-SR04, muestra datos en OLED (I2C), modula
 * pitidos en buzzer según proximidad y reporta telemetría por puerto serie.
 * ==============================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==========================================
// 1. CONFIGURACIÓN DE LA PANTALLA OLED
// ==========================================
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA  64
#define DIRECCION_I2C  0x3C

Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
bool oledConectada = false;

// ==========================================
// 2. ASIGNACIÓN DE PINES (GPIOs DEL ESP32)
// ==========================================
const int PIN_TRIG   = 5;   // SALIDA: Disparo de pulso ultrasónico
const int PIN_ECHO   = 18;  // ENTRADA: Retorno del eco (con divisor resistivo)
const int PIN_BUZZER = 19;  // SALIDA: Avisador acústico

// ==========================================
// 3. DECLARACIÓN DE FUNCIONES
// ==========================================
float medirDistancia();
void actualizarPantalla(float dist);
void gestionarBuzzer(float dist);

// ==========================================
// 4. SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(500); // Pequeña pausa para estabilizar alimentación y USB

  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F("   MediaLab UniOvi - Sensor de Parking ESP32      "));
  Serial.println(F("=================================================="));

  // Configuración de pines físicos
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  // Inicialización de I2C con timeout para evitar bloqueos
  Wire.begin(21, 22);
  Wire.setTimeOut(50); // Timeout en ms para evitar cuelgues I2C

  // Prueba de pantalla OLED
  Serial.print(F("[INFO] Buscando pantalla OLED I2C en 0x3C... "));
  if (pantalla.begin(SSD1306_SWITCHCAPVCC, DIRECCION_I2C)) {
    oledConectada = true;
    Serial.println(F("OK (Pantalla detectada)"));

    pantalla.clearDisplay();
    pantalla.setTextColor(SSD1306_WHITE);
    pantalla.setTextSize(1);
    pantalla.setCursor(10, 20);
    pantalla.println("MediaLab UniOvi");
    pantalla.setCursor(10, 35);
    pantalla.println("Sensor Parking OK");
    pantalla.display();
    delay(800);
  } else {
    oledConectada = false;
    Serial.println(F("NO DETECTADA"));
    Serial.println(F("[AVISO] El sistema continuará funcionando sin OLED (usando Buzzer y Serial)."));
  }

  // Prueba breve del Buzzer al arrancar (1 pitido corto de test)
  Serial.println(F("[INFO] Test de arranque del Buzzer..."));
  digitalWrite(PIN_BUZZER, HIGH);
  delay(80);
  digitalWrite(PIN_BUZZER, LOW);

  Serial.println(F("[INFO] Sistema listo. Iniciando lecturas del sensor HC-SR04..."));
  Serial.println();
}

// ==========================================
// 5. LOOP
// ==========================================
void loop() {
  // 1. Medir distancia
  float distancia = medirDistancia();

  // 2. Reportar siempre por puerto serie
  if (distancia >= 200.0) {
    Serial.println(F("[HC-SR04] Sin obstaculo detectado o fuera de rango (> 200 cm / timeout)"));
  } else {
    Serial.print(F("[HC-SR04] Distancia: "));
    Serial.print(distancia, 1);
    Serial.print(F(" cm"));
    if (distancia <= 10.0) {
      Serial.print(F("  --> ALERTA STOP!"));
    }
    Serial.println();
  }

  // 3. Actualizar pantalla si está conectada
  if (oledConectada) {
    actualizarPantalla(distancia);
  }

  // 4. Modulación del buzzer
  gestionarBuzzer(distancia);
}

// ==========================================
// 6. FUNCIONES AUXILIARES
// ==========================================

float medirDistancia() {
  // Pulso limpio de disparo de 10 microsegundos
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Leer duración del pulso de eco con timeout de 30000 us (aprox. 5 metros)
  long duracion_us = pulseIn(PIN_ECHO, HIGH, 30000);

  if (duracion_us <= 0) {
    return 200.0; // Sin rebote detectado
  }

  // Distancia (cm) = tiempo (us) * 0.0343 / 2
  float distancia_cm = (duracion_us * 0.0343) / 2.0;

  if (distancia_cm > 200.0) {
    distancia_cm = 200.0;
  }
  return distancia_cm;
}

void actualizarPantalla(float dist) {
  pantalla.clearDisplay();

  // Cabecera
  pantalla.setTextSize(1);
  pantalla.setTextColor(SSD1306_WHITE);
  pantalla.setCursor(0, 0);
  pantalla.print("SENSOR DE PARKING");

  // Lectura en cm
  pantalla.setTextSize(2);
  pantalla.setCursor(0, 18);
  if (dist >= 200.0) {
    pantalla.print(">200 cm");
  } else {
    pantalla.print(dist, 1);
    pantalla.print(" cm");
  }

  // Alerta STOP si está muy cerca
  if (dist <= 10.0) {
    pantalla.setTextSize(1);
    pantalla.setCursor(85, 22);
    pantalla.print("STOP!");
  }

  // Barra gráfica
  int distAcotada = constrain((int)dist, 5, 100);
  int anchoBarra = map(distAcotada, 5, 100, 120, 0);

  pantalla.drawRect(0, 45, 124, 14, SSD1306_WHITE);
  pantalla.fillRect(2, 47, anchoBarra, 10, SSD1306_WHITE);

  pantalla.display();
}

void gestionarBuzzer(float dist) {
  if (dist > 100.0) {
    // Fuera de rango o lejos: silencio
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
  } else if (dist <= 10.0) {
    // Muy cerca: tono continuo
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
  } else {
    // Rango medio: cadencia según proximidad
    int pausa_ms = map((int)dist, 10, 100, 40, 450);
    digitalWrite(PIN_BUZZER, HIGH);
    delay(40);
    digitalWrite(PIN_BUZZER, LOW);
    delay(pausa_ms);
  }
}
