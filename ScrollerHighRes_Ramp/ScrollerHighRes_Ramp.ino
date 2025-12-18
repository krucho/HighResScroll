#include "Adafruit_TinyUSB.h"

const int pinButton = 0; 
const bool activeState = LOW; 

#define REPORT_ID_MOUSE      1
#define REPORT_ID_MULTIPLIER 2

// // --- CONFIGURACIÓN DE ACELERACIÓN ---

// Valores para tunearlo a tu gusto:

// Si quieres que tarde más en empezar a acelerar: aumenta TIEMPO_PARA_ACELERAR.
// Si quieres que la aceleración sea más agresiva: disminuye PASO_ACELERACION.
// Si quieres más velocidad punta: aumenta (hacia lo negativo) SCROLL_MAX.

const uint32_t TIEMPO_PARA_ACELERAR = 800;  // ms antes de empezar a subir la velocidad
const int8_t   SCROLL_MIN = -1;             // Velocidad inicial (suave)
const int8_t   SCROLL_MAX = -12;            // Velocidad máxima (rápida)
const uint32_t PASO_ACELERACION = 150;      // ms entre cada incremento de velocidad

uint8_t const desc_hid_report[] = {
  0x05, 0x01, 0x09, 0x02, 0xA1, 0x01,
    0x85, REPORT_ID_MOUSE,
    0x09, 0x01, 0xA1, 0x00,
      0x05, 0x09, 0x19, 0x01, 0x29, 0x03, 0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01, 0x81, 0x02,
      0x95, 0x01, 0x75, 0x05, 0x81, 0x03,
      0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x16, 0x01, 0x80, 0x26, 0xFF, 0x7F, 0x75, 0x10, 0x95, 0x02, 0x81, 0x06,
      0x05, 0x01, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x01, 0x81, 0x06,
      0x05, 0x01, 0x09, 0x48, 0x85, REPORT_ID_MULTIPLIER, 0x15, 0x00, 0x25, 0x01, 0x35, 0x01, 0x45, 0x0C, 0x75, 0x02, 0x95, 0x01, 0xB1, 0x02,
      0x75, 0x06, 0xB1, 0x03,
    0xC0,
  0xC0
};

typedef struct __attribute__((packed)) {
  uint8_t buttons;
  int16_t x;
  int16_t y;
  int8_t  wheel;
} hi_res_mouse_report_t;

Adafruit_USBD_HID usb_hid;

uint16_t my_get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
  if (report_type == HID_REPORT_TYPE_FEATURE && report_id == REPORT_ID_MULTIPLIER) {
    if (reqlen >= 1) { buffer[0] = 1; return 1; }
  }
  return 0;
}

void setup() {
  pinMode(pinButton, INPUT);
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setReportCallback(my_get_report_callback, NULL);
  usb_hid.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(500);
    TinyUSBDevice.attach();
  }
}

// Variables de estado para la aceleración
uint32_t tPresionado = 0;   // Cuándo se presionó el botón
uint32_t tUltimoReporte = 0; // Para controlar el flujo sin usar delay() duro
int8_t   velocidadActual = SCROLL_MIN;

void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
  #endif

  if (!usb_hid.ready()) return;

  bool estaPresionado = (digitalRead(pinButton) == activeState);

  if (estaPresionado) {
    uint32_t ahora = millis();

    // Si es la primera vez que detectamos la presión
    if (tPresionado == 0) {
      tPresionado = ahora;
      tUltimoReporte = ahora;
      velocidadActual = SCROLL_MIN;
    }

    // Calcular cuánto tiempo lleva presionado
    uint32_t duracion = ahora - tPresionado;

    // Lógica de Aceleración:
    // Si pasó el tiempo de espera inicial, empezamos a aumentar la velocidad
    if (duracion > TIEMPO_PARA_ACELERAR) {
        // Cada X milisegundos, aumentamos la potencia del scroll (más negativo)
        int8_t incremento = (duracion - TIEMPO_PARA_ACELERAR) / PASO_ACELERACION;
        velocidadActual = SCROLL_MIN - incremento;
        
        // Limitar a la velocidad máxima
        if (velocidadActual < SCROLL_MAX) velocidadActual = SCROLL_MAX;
    }

    // Enviar el reporte cada 20ms para mantener fluidez (50Hz de reportes)
    if (ahora - tUltimoReporte >= 20) {
      tUltimoReporte = ahora;
      hi_res_mouse_report_t report = {0};
      report.wheel = velocidadActual;
      usb_hid.sendReport(REPORT_ID_MOUSE, &report, sizeof(report));
    }
  } 
  else {
    // Botón soltado: resetear todo
    tPresionado = 0;
  }
}