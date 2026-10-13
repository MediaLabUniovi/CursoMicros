# Taller ESP32 - Sensor de Parking (MediaLab UniOvi)

Bienvenido al repositorio oficial del taller práctico de sistemas embebidos de **MediaLab UniOvi** (Universidad de Oviedo). En esta sesión construirás un **sensor de aparcamiento inteligente** para coche utilizando un microcontrolador **ESP32**, un sensor de distancia ultrasónico **HC-SR04**, un avisador acústico (**Buzzer**) y una pantalla **OLED I2C SSD1306**.

---

<p align="center">
  <img src="https://api.qrserver.com/v1/create-qr-code/?size=400x400&data=https://github.com/MediaLabUniovi/CursoMicros" alt="QR Repositorio" width="380" height="380" />
  <br>
  <sub><b>Escanea este código QR con tu móvil para acceder al repositorio en GitHub</b></sub><br>
  <code>https://github.com/MediaLabUniovi/CursoMicros</code>
</p>

---

## ⚡ Carga Ultrarrápida para Alumnos (3 Pasos)

No necesitas instalar PlatformIO, drivers complejos ni compilar código si solo quieres cargar el proyecto en tu ESP32:

1. **Paso 1: Conectar el ESP32 por USB**  
   Conecta la placa ESP32 a cualquier puerto USB de tu ordenador con un cable de datos.

2. **Paso 2: Entrar en la carpeta `CargaCodigo` y ejecutar el cargador**  
   - En **Windows**: Haz doble clic en `Iniciar_Cargador.bat`.  
   - En **Linux / macOS**: Abre una terminal en la carpeta y ejecuta `./Iniciar_Cargador.sh`.  
   *(Se abrirá automáticamente tu navegador en `http://localhost:8000`)*.

3. **Paso 3: Pulsar "Conectar y Cargar Código" en el navegador**  
   - Haz clic en el botón principal.
   - Selecciona el puerto serie de tu ESP32 (COMx en Windows o `/dev/ttyUSB*` en Linux/Mac).
   - ¡Listo! En unos segundos el firmware se grabará y el ESP32 se reiniciará automáticamente.

> [!TIP]
> Si el cargador web indica que no puede conectar con la placa, mantén presionado el botón **BOOT** del ESP32 justo al pulsar el botón "Conectar" hasta que empiece la detección.

---

## 🔌 Tabla Resumen de Pines y Conexiones

Conecta los componentes en la protoboard siguiendo esta asignación de pines:

| Componente | Pin del Componente | Pin ESP32 / Conexión | Descripción y Notas |
| :--- | :--- | :--- | :--- |
| **HC-SR04** (Ultrasónico) | `VCC` | **VIN (5V)** | Alimentación a 5V desde el bus USB |
| | `GND` | **GND** | Masa común |
| | `TRIG` | **GPIO 5** | Salida digital de pulsos ultrasónicos |
| | `ECHO` | **GPIO 18** | **Pasa por divisor resistivo (1kΩ / 2kΩ)** |
| **Divisor Resistivo** | Resistencia R1 (1kΩ) | Entre `ECHO` y `GPIO 18` | Protege el GPIO del ESP32 (máx. 3.3V) |
| *(Protección Echo)* | Resistencia R2 (2kΩ) | Entre `GPIO 18` y `GND` | Reduce la señal de 5V a ~3.3V |
| **Buzzer** (Zumbador) | `Ánodo (+)` | **GPIO 19** | Salida PWM/digital para tonos de proximidad |
| | `Cátodo (-)` | **GND** | Masa común |
| **Pantalla OLED I2C** | `VCC` | **3.3V** o **VIN** | Alimentación del módulo SSD1306 |
| *(128x64 píxeles)* | `GND` | **GND** | Masa común |
| | `SDA` | **GPIO 21** | Línea de datos I2C por defecto |
| | `SCL` | **GPIO 22** | Línea de reloj I2C por defecto |

> [!IMPORTANT]
> **Protección del pin ECHO (5V -> 3.3V):**  
> El sensor HC-SR04 emite la señal ECHO a 5V. Las entradas digitales del ESP32 toleran un máximo de 3.3V. El divisor formado por **R1 = 1 kΩ** en serie y **R2 = 2 kΩ** a masa rebaja el voltaje a un nivel seguro:  
> $$V_{GPIO} = 5\text{V} \times \frac{2\text{k}\Omega}{1\text{k}\Omega + 2\text{k}\Omega} \approx 3.33\text{V}$$

---

## 💻 Desarrollo y Modificación del Código (PlatformIO + VS Code)

Para los alumnos que deseen explorar el código fuente en C++, modificar los límites de distancia, personalizar los gráficos de la pantalla OLED o añadir nuevas funciones:

1. Abre **Visual Studio Code** con la extensión **PlatformIO IDE** instalada.
2. Abre la carpeta raíz del proyecto (`CursoMicros`).
3. El archivo de código principal está en [`src/main.cpp`](src/main.cpp), completamente documentado y comentado paso a paso.
4. Pulsa el botón **✓ (Build)** en la barra inferior para compilar o **➜ (Upload)** para subir tus modificaciones.

---

## 📁 Estructura del Repositorio

```text
CursoMicros/
├── CargaCodigo/                 <- Herramienta autónoma web para flashear
│   ├── index.html               <- Interfaz Web Serial en español (esptool-js)
│   ├── esptool-bundle.js        <- Motor Web Serial empaquetado (offline)
│   ├── Iniciar_Cargador.bat     <- Lanzador 1-clic para Windows
│   ├── Iniciar_Cargador.sh      <- Lanzador 1-clic para Linux / macOS
│   ├── server.ps1               <- Servidor HTTP nativo en PowerShell
│   ├── manifest.json            <- Manifiesto ESP Web Tools
│   ├── firmware.bin             <- Offset 0x10000 (Código del sensor de parking)
│   ├── bootloader.bin           <- Offset 0x1000 (Gestor de arranque ESP32)
│   ├── partitions.bin           <- Offset 0x8000 (Tabla de particiones)
│   └── boot_app0.bin            <- Offset 0xe000 (Configuración de arranque OTA)
├── src/
│   └── main.cpp                 <- Código fuente en C++ para Arduino/PlatformIO
├── platformio.ini               <- Configuración de compilador y librerías
└── README.md                    <- Guía del taller y conexiones
```

---

<p align="center">
  <b>MediaLab UniOvi</b> — Laboratorio de Fabricación y Prototipado<br>
  Universidad de Oviedo
</p>
