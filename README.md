# 🖱️ Raspberry Pi Pico - High Resolution Smooth Scroll

Este repositorio contiene un ejemplo avanzado para transformar una **Raspberry Pi Pico (RP2040)** en un periférico HID capaz de realizar un **scroll suave (pixel a pixel)** utilizando la librería Adafruit TinyUSB.

A diferencia del scroll estándar, este proyecto implementa el **Resolution Multiplier**, permitiendo una experiencia de navegación fluida y profesional.

## ✨ Características Técnicas

- **High-Res Scroll:** Utiliza el multiplicador de resolución HID (Usage 0x48) para romper la barrera de las "3 líneas por muesca" del mouse tradicional.
- **Aceleración Dinámica:**
  - **Fase de Precisión:** Los primeros 800ms el scroll es ultra-lento (ideal para diseño o selección de texto).
  - **Fase de Vuelo:** La velocidad aumenta progresivamente después del umbral de tiempo.
- **Polling Rate de 1000Hz:** Respuesta inmediata gracias al intervalo de consulta de 1ms.
- **Robustez de Descriptor:** Implementación de 8-bits para la rueda, garantizando compatibilidad total de signo (+/-) en Windows y macOS.

## 🔧 Conexiones (Hardware)

| Componente | Pin Pico (GP) | Pin Físico | Nota |
| :--- | :--- | :--- | :--- |
| **Pulsador** | GP0 | Pin 1 | Conectar a GND al presionar |
| **Resistencia** | GP0 a 3.3V | - | Pull-up externo (10k recomendado) |

## 🛠️ Requisitos de Software

1.  **Arduino IDE** con el core de [Raspberry Pi Pico (Earle Philhower)](https://github.com/earlephilhower/arduino-pico).
2.  **Librería:** Adafruit TinyUSB (disponible en el Library Manager).
3.  **Configuración en IDE:** - Tools > USB Stack: "Adafruit TinyUSB".

## 📖 ¿Cómo funciona el Scroll Suave?

Normalmente, el sistema operativo espera que un mouse envíe un valor de `120` para mover una "línea" de texto. Al enviar valores pequeños como `-1` o `-2` sin el descriptor adecuado, el SO simplemente los ignora hasta que suman 120.

Este proyecto define un **HID Feature Report** llamado `Resolution Multiplier`. Al iniciar, la Pico le dice al PC: *"Soy capaz de manejar resoluciones mayores"*. Gracias a esto, cada unidad enviada se traduce en un movimiento de píxeles, logrando la suavidad que ves en este código.

📜 Licencia
Este proyecto se distribuye bajo la licencia MIT. Siéntete libre de usarlo en tus propios proyectos de periféricos personalizados.
