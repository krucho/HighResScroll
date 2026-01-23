# 🖱️ Raspberry Pi Pico - High Resolution Smooth Scroll with Web Configuration

[English](#english) | [Español](#español)

---

## English

This repository contains an advanced example to transform a **Raspberry Pi Pico (RP2040)** into an HID peripheral capable of performing **smooth scrolling (pixel by pixel)** using the Adafruit TinyUSB library.

Unlike standard scrolling, this project implements the **Resolution Multiplier**, enabling a smooth and professional navigation experience. Additionally, it includes **EEPROM configuration persistence**, **serial communication**, and a **web interface** to configure and test the device.

### ✨ Technical Features

* **High-Res Scroll:** Uses the HID resolution multiplier (Usage 0x48) to break the traditional mouse "3 lines per notch" barrier.
* **Dynamic Acceleration:**  
   * **Precision Phase:** The first milliseconds the scroll is ultra-slow (ideal for design or text selection).  
   * **Flight Phase:** Speed increases progressively after the configurable time threshold.
* **1000Hz Polling Rate:** Immediate response thanks to the 1ms polling interval.
* **Robust Descriptor:** 8-bit implementation for the wheel, ensuring full sign (+/-) compatibility on Windows and macOS.
* **EEPROM Persistence:** Configuration parameters are saved in non-volatile memory and persist after restart.
* **Serial Communication:** Configuration via simple commands through serial port (115200 baud).
* **Web Interface:** Visual configuration via Web Serial API, no additional software required.

### 🔧 Hardware Connections

| Component      | Pico Pin (GP) | Physical Pin | Note                              |
| -------------- | ------------- | ----------- | --------------------------------- |
| **Button**     | GP0           | Pin 1       | Connect to GND when pressed       |
| **Resistor**   | GP0 to 3.3V   | \-          | External pull-up (10k recommended) |

### 🛠️ Software Requirements

1. **Arduino IDE** with Raspberry Pi Pico core (Earle Philhower).
2. **Library:** Adafruit TinyUSB (available in Library Manager).
3. **IDE Configuration:** Tools > USB Stack: "Adafruit TinyUSB".

### 📦 Project Structure

```
Scroller Pedal/
├── ScrollerGeminiHighRes/
│   └── ScrollerGeminiHighRes.ino          # Basic version (simple scroll)
├── ScrollerHighRes_Ramp_WebConfig/
│   └── ScrollerHighRes_Ramp_WebConfig.ino # Full version with acceleration, EEPROM and serial
└── web/
    └── index.html                          # Web configuration interface
```

### ⚙️ Configurable Parameters

The following parameters can be configured and saved to EEPROM:

| Parameter | Description | Range | Default Value |
|-----------|-------------|-------|---------------|
| `TIEMPO_PARA_ACELERAR` | Time in ms before acceleration starts | 100-10000 | 800 |
| `SCROLL_MIN` | Initial scroll speed (smooth) | -127 to 127 | -1 |
| `SCROLL_MAX` | Maximum scroll speed (fast) | -127 to 127 | -12 |
| `PASO_ACELERACION` | Interval in ms between speed increments | 10-1000 | 150 |
| `DIRECCION_SCROLL` | Scroll direction (-1=down, 1=up) | -1 or 1 | -1 |

### 📡 Serial Commands

The device accepts the following commands via serial port (115200 baud):

#### Get Values

```
GET ALL
→ OK TIEMPO_PARA_ACELERAR=800 SCROLL_MIN=-1 SCROLL_MAX=-12 PASO_ACELERACION=150 DIRECCION_SCROLL=-1

GET SCROLL_MAX
→ OK SCROLL_MAX=-12
```

#### Set Values

```
SET SCROLL_MAX -15
→ OK SCROLL_MAX=-15

SET TIEMPO_PARA_ACELERAR 1000
→ OK TIEMPO_PARA_ACELERAR=1000
```

#### Save and Reset

```
SAVE
→ OK SAVED

RESET
→ OK RESET
```

### 🌐 Web Interface

The web interface (`web/index.html`) allows visual device configuration:

1. **Open** `web/index.html` in Chrome or Edge (support Web Serial API)
2. **Connect** the device from the interface
3. **Configure** parameters according to your preferences
4. **Apply** changes (saved to RAM)
5. **Save to EEPROM** to make changes persistent
6. **Test** scrolling in the integrated test area

**Interface features:**
- Modern dark theme
- Test area with long text to evaluate scrolling
- Real-time range validation
- Visual feedback of connection status
- Automatic scroll positioning to test both directions

### 💾 EEPROM Structure

Data is stored in EEPROM with the following structure:

```
Offset 0-3:   Magic number (0x5343524C = "SCRL")
Offset 4:     Version (1)
Offset 5-8:   tiempoParaAcelerar (uint32_t)
Offset 9:     scrollMin (int8_t)
Offset 10:    scrollMax (int8_t)
Offset 11-14: pasoAceleracion (uint32_t)
Offset 15:    direccionScroll (int8_t)
Offset 16-19: Checksum (integrity validation)
```

The system includes magic number, version, and checksum validation to detect corrupted data and automatically restore default values.

### 📖 How Does Smooth Scrolling Work?

Normally, the operating system expects a mouse to send a value of `120` to move one "line" of text. When sending small values like `-1` or `-2` without the proper descriptor, the OS simply ignores them until they sum to 120.

This project defines an **HID Feature Report** called `Resolution Multiplier`. On startup, the Pico tells the PC: _"I'm capable of handling higher resolutions"_. Thanks to this, each unit sent translates into pixel movement, achieving the smoothness you see in this code.

### 🚀 Quick Start

1. **Compile and upload** `ScrollerHighRes_Ramp_WebConfig.ino` to the Pico
2. **Open** `web/index.html` in Chrome/Edge
3. **Connect** the device from the web interface
4. **Configure** parameters according to your needs
5. **Save** to EEPROM to make changes persistent
6. **Test** scrolling with the pedal

### 🔍 Troubleshooting

**Device doesn't appear in web interface:**
- Make sure you're using Chrome or Edge (support Web Serial API)
- Verify the serial port isn't in use by another application
- Check that the baudrate is 115200

**Values don't persist after restart:**
- Make sure to use the `SAVE` command or "Save to EEPROM" button after making changes
- Verify EEPROM isn't corrupted by using `RESET` and then configuring again

**Scrolling doesn't work:**
- Check button connections (GP0 to GND when pressed)
- Verify the HID device is correctly recognized by the operating system
- Try changing scroll direction if movement is inverted

### 📜 License

This project is distributed under the MIT license. Feel free to use it in your own custom peripheral projects.

---

## Español

Este repositorio contiene un ejemplo avanzado para transformar una **Raspberry Pi Pico (RP2040)** en un periférico HID capaz de realizar un **scroll suave (pixel a pixel)** utilizando la librería Adafruit TinyUSB.

A diferencia del scroll estándar, este proyecto implementa el **Resolution Multiplier**, permitiendo una experiencia de navegación fluida y profesional. Además, incluye **persistencia de configuración en EEPROM**, **comunicación serial** y una **interfaz web** para configurar y probar el dispositivo.

### ✨ Características Técnicas

* **High-Res Scroll:** Utiliza el multiplicador de resolución HID (Usage 0x48) para romper la barrera de las "3 líneas por muesca" del mouse tradicional.
* **Aceleración Dinámica:**  
   * **Fase de Precisión:** Los primeros milisegundos el scroll es ultra-lento (ideal para diseño o selección de texto).  
   * **Fase de Vuelo:** La velocidad aumenta progresivamente después del umbral de tiempo configurable.
* **Polling Rate de 1000Hz:** Respuesta inmediata gracias al intervalo de consulta de 1ms.
* **Robustez de Descriptor:** Implementación de 8-bits para la rueda, garantizando compatibilidad total de signo (+/-) en Windows y macOS.
* **Persistencia EEPROM:** Los parámetros de configuración se guardan en memoria no volátil y persisten después de reiniciar.
* **Comunicación Serial:** Configuración mediante comandos simples vía puerto serial (115200 baud).
* **Interfaz Web:** Configuración visual mediante Web Serial API, sin necesidad de software adicional.

### 🔧 Conexiones (Hardware)

| Componente      | Pin Pico (GP) | Pin Físico | Nota                              |
| --------------- | ------------- | ---------- | --------------------------------- |
| **Pulsador**    | GP0           | Pin 1      | Conectar a GND al presionar       |
| **Resistencia** | GP0 a 3.3V    | \-         | Pull-up externo (10k recomendado) |

### 🛠️ Requisitos de Software

1. **Arduino IDE** con el core de Raspberry Pi Pico (Earle Philhower).
2. **Librería:** Adafruit TinyUSB (disponible en el Library Manager).
3. **Configuración en IDE:** Tools > USB Stack: "Adafruit TinyUSB".

### 📦 Estructura del Proyecto

```
Scroller Pedal/
├── ScrollerGeminiHighRes/
│   └── ScrollerGeminiHighRes.ino          # Versión básica (scroll simple)
├── ScrollerHighRes_Ramp_WebConfig/
│   └── ScrollerHighRes_Ramp_WebConfig.ino # Versión completa con aceleración, EEPROM y serial
└── web/
    └── index.html                          # Interfaz web de configuración
```

### ⚙️ Parámetros Configurables

Los siguientes parámetros pueden configurarse y guardarse en EEPROM:

| Parámetro | Descripción | Rango | Valor por Defecto |
|-----------|-------------|-------|-------------------|
| `TIEMPO_PARA_ACELERAR` | Tiempo en ms antes de comenzar la aceleración | 100-10000 | 800 |
| `SCROLL_MIN` | Velocidad inicial del scroll (suave) | -127 a 127 | -1 |
| `SCROLL_MAX` | Velocidad máxima del scroll (rápida) | -127 a 127 | -12 |
| `PASO_ACELERACION` | Intervalo en ms entre incrementos de velocidad | 10-1000 | 150 |
| `DIRECCION_SCROLL` | Dirección del scroll (-1=abajo, 1=arriba) | -1 o 1 | -1 |

### 📡 Comandos Serial

El dispositivo acepta los siguientes comandos vía puerto serial (115200 baud):

#### Obtener Valores

```
GET ALL
→ OK TIEMPO_PARA_ACELERAR=800 SCROLL_MIN=-1 SCROLL_MAX=-12 PASO_ACELERACION=150 DIRECCION_SCROLL=-1

GET SCROLL_MAX
→ OK SCROLL_MAX=-12
```

#### Establecer Valores

```
SET SCROLL_MAX -15
→ OK SCROLL_MAX=-15

SET TIEMPO_PARA_ACELERAR 1000
→ OK TIEMPO_PARA_ACELERAR=1000
```

#### Guardar y Resetear

```
SAVE
→ OK SAVED

RESET
→ OK RESET
```

### 🌐 Interfaz Web

La interfaz web (`web/index.html`) permite configurar el dispositivo de forma visual:

1. **Abrir** `web/index.html` en Chrome o Edge (soportan Web Serial API)
2. **Conectar** el dispositivo desde la interfaz
3. **Configurar** los parámetros según tus preferencias
4. **Aplicar** los cambios (se guardan en RAM)
5. **Guardar en EEPROM** para hacer los cambios persistentes
6. **Probar** el scroll en el área de prueba integrada

**Características de la interfaz:**
- Tema oscuro moderno
- Área de prueba con texto largo para evaluar el scroll
- Validación de rangos en tiempo real
- Feedback visual del estado de conexión
- Posicionamiento automático del scroll para probar ambas direcciones

### 💾 Estructura EEPROM

Los datos se almacenan en EEPROM con la siguiente estructura:

```
Offset 0-3:   Magic number (0x5343524C = "SCRL")
Offset 4:     Version (1)
Offset 5-8:   tiempoParaAcelerar (uint32_t)
Offset 9:     scrollMin (int8_t)
Offset 10:    scrollMax (int8_t)
Offset 11-14: pasoAceleracion (uint32_t)
Offset 15:    direccionScroll (int8_t)
Offset 16-19: Checksum (validación de integridad)
```

El sistema incluye validación de magic number, versión y checksum para detectar datos corruptos y restaurar valores por defecto automáticamente.

### 📖 ¿Cómo funciona el Scroll Suave?

Normalmente, el sistema operativo espera que un mouse envíe un valor de `120` para mover una "línea" de texto. Al enviar valores pequeños como `-1` o `-2` sin el descriptor adecuado, el SO simplemente los ignora hasta que suman 120.

Este proyecto define un **HID Feature Report** llamado `Resolution Multiplier`. Al iniciar, la Pico le dice al PC: _"Soy capaz de manejar resoluciones mayores"_. Gracias a esto, cada unidad enviada se traduce en un movimiento de píxeles, logrando la suavidad que ves en este código.

### 🚀 Uso Rápido

1. **Compilar y subir** `ScrollerHighRes_Ramp_WebConfig.ino` al Pico
2. **Abrir** `web/index.html` en Chrome/Edge
3. **Conectar** el dispositivo desde la interfaz web
4. **Configurar** los parámetros según tus necesidades
5. **Guardar** en EEPROM para hacer los cambios persistentes
6. **Probar** el scroll con el pedal

### 🔍 Solución de Problemas

**El dispositivo no aparece en la interfaz web:**
- Asegúrate de usar Chrome o Edge (soportan Web Serial API)
- Verifica que el puerto serial no esté en uso por otra aplicación
- Revisa que el baudrate sea 115200

**Los valores no persisten después de reiniciar:**
- Asegúrate de usar el comando `SAVE` o el botón "Guardar en EEPROM" después de hacer cambios
- Verifica que el EEPROM no esté corrupto usando `RESET` y luego configurando de nuevo

**El scroll no funciona:**
- Verifica las conexiones del botón (GP0 a GND cuando se presiona)
- Comprueba que el dispositivo HID esté correctamente reconocido por el sistema operativo
- Prueba cambiar la dirección del scroll si el movimiento es inverso

### 📜 Licencia

Este proyecto se distribuye bajo la licencia MIT. Siéntete libre de usarlo en tus propios proyectos de periféricos personalizados.
