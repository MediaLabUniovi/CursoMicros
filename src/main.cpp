/*
 * ==============================================================================
 * PROYECTO: SENSOR DE APARCAMIENTO CON ESP32
 * ==============================================================================
 * Este proyecto lee la distancia a un obstáculo con un sensor ultrasónico HC-SR04,
 * muestra la distancia y una barra gráfica en una pantalla OLED (I2C) y modula
 * la frecuencia de pitido de un buzzer según la proximidad (estilo sensor de coche).
 * ==============================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==========================================
// 1. CONFIGURACIÓN DE LA PANTALLA OLED
// ==========================================
// Dimensiones físicas de la pantalla en píxeles:
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA  64

// Dirección I2C del módulo SSD1306 (habitualmente 0x3C o 0x3D):
#define DIRECCION_I2C  0x3C

// Creamos la instancia para la pantalla usando el bus I2C estándar (Wire):
Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

// ==========================================
// 2. ASIGNACIÓN DE PINES (GPIOs DEL ESP32)
// ==========================================
// Sensor ultrasónico HC-SR04:
const int PIN_TRIG = 5;   // SALIDA: Envía el tren de pulsos ultrasónicos
const int PIN_ECHO = 18;  // ENTRADA: Mide el rebote (PASA POR DIVISOR DE TENSIÓN)

// Avisador acústico:
const int PIN_BUZZER = 19; // SALIDA: Activa el sonido

// NOTA SOBRE I2C:
// En el ESP32, los pines por defecto para el bus I2C son:
// - SDA = GPIO 21 (Datos)
// - SCL = GPIO 22 (Reloj)
// No es necesario definirlos manualmente, la librería Wire los usa automáticamente.

// ==========================================
// 3. DECLARACIÓN DE FUNCIONES AUXILIARES
// ==========================================
float medirDistancia();
void actualizarPantalla(float dist);
void gestionarBuzzer(float dist);

// ==========================================
// 4. CONFIGURACIÓN INICIAL (SETUP)
// ==========================================
void setup() {
  // Inicializamos la consola serie a 115200 baudios para depuración
  Serial.begin(115200);

  // Definimos el rol de cada pin en el silicio:
  // - OUTPUT: El microcontrolador impone 0V o 3.3V
  // - INPUT: El microcontrolador lee alta impedancia
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Dejamos los actuadores en estado bajo inicial
  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  // Inicialización de la pantalla OLED mediante I2C
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, DIRECCION_I2C)) {
    Serial.println(F("[ERROR] No se pudo encontrar la pantalla OLED SSD1306"));
    while (true) {
      // Bloqueamos la ejecución si no hay comunicación I2C
      delay(100);
    }
  }

  // Pantalla de bienvenida
  pantalla.clearDisplay();
  pantalla.setTextColor(SSD1306_WHITE);
  pantalla.setTextSize(1);
  pantalla.setCursor(10, 25);
  pantalla.print("SISTEMA PARKING OK");
  pantalla.display();
  delay(1200);
}

// ==========================================
// 5. BUCLE PRINCIPAL (LOOP)
// ==========================================
void loop() {
  // 1. Tomamos la medición física
  float distancia = medirDistancia();

  // 2. Mostramos los datos en la pantalla OLED
  actualizarPantalla(distancia);

  // 3. Emitimos el sonido correspondiente según la cercanía
  gestionarBuzzer(distancia);
}

// ==========================================
// 6. IMPLEMENTACIÓN DE FUNCIONES
// ==========================================

// Mide el tiempo de vuelo del ultrasonido y calcula la distancia en centímetros
float medirDistancia() {
  // Generamos un pulso limpio en HIGH de 10 microsegundos en el pin TRIG
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Medimos cuántos microsegundos permanece en nivel alto el pin ECHO
  // Ponemos un timeout de 30000 us (aprox. 5 metros de límite)
  long duracion_us = pulseIn(PIN_ECHO, HIGH, 30000);

  // Si no hay eco (fuera de rango o sin obstáculo), fijamos un valor por defecto
  if (duracion_us == 0) {
    return 200.0;
  }

  // Conversión física:
  // Velocidad del sonido = 343 m/s = 0.0343 cm/microsegundo
  // Distancia = (tiempo * velocidad) / 2 (el sonido va y vuelve)
  float distancia_cm = (duracion_us * 0.0343) / 2.0;

  if (distancia_cm > 200.0) {
    distancia_cm = 200.0;
  }

  return distancia_cm;
}

// Dibuja en la pantalla OLED la distancia y una barra de proximidad
void actualizarPantalla(float dist) {
  pantalla.clearDisplay();

  // Encabezado
  pantalla.setTextSize(1);
  pantalla.setTextColor(SSD1306_WHITE);
  pantalla.setCursor(0, 0);
  pantalla.print("SENSOR DE PARKING");

  // Lectura numérica
  pantalla.setTextSize(2);
  pantalla.setCursor(0, 18);
  pantalla.print(dist, 1);
  pantalla.print(" cm");

  // Alerta si estamos a menos de 10 cm
  if (dist <= 10.0) {
    pantalla.setTextSize(1);
    pantalla.setCursor(85, 22);
    pantalla.print("STOP!");
  }

  // Barra gráfica inferior proporcional a la proximidad:
  // Si dist = 100 cm -> ancho 0 px
  // Si dist = 5 cm   -> ancho 120 px (barra llena)
  int distAcotada = constrain((int)dist, 5, 100);
  int anchoBarra = map(distAcotada, 5, 100, 120, 0);

  pantalla.drawRect(0, 45, 124, 14, SSD1306_WHITE);           // Contorno exterior
  pantalla.fillRect(2, 47, anchoBarra, 10, SSD1306_WHITE);     // Relleno proporcional

  pantalla.display();
}

// Controla el buzzer variando el retardo entre pitidos según la distancia
void gestionarBuzzer(float dist) {
  // Caso 1: Lejos (> 100 cm) -> Silencio total
  if (dist > 100.0) {
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
  }
  // Caso 2: Muy cerca (<= 10 cm) -> Pitido continuo (peligro inminente)
  else if (dist <= 10.0) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
  }
  // Caso 3: Rango medio (10 a 100 cm) -> Pitidos intermitentes progresivos
  else {
    // Calculamos el intervalo:
    // A 100 cm -> pausa larga (450 ms)
    // A 10 cm  -> pausa muy corta (40 ms)
    int pausa_ms = map((int)dist, 10, 100, 40, 450);

    digitalWrite(PIN_BUZZER, HIGH); // Activa sonido
    delay(40);                      // Duración del pulso sonoro
    digitalWrite(PIN_BUZZER, LOW);  // Corta sonido
    delay(pausa_ms);                // Intervalo calculado
  }
}
