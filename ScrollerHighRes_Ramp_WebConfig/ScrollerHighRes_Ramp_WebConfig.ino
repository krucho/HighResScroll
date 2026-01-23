#include "Adafruit_TinyUSB.h"
#include <EEPROM.h>

const int pinButton = 0; 
const bool activeState = LOW; 

#define REPORT_ID_MOUSE      1
#define REPORT_ID_MULTIPLIER 2

// --- CONFIGURACIÓN DE ACELERACIÓN ---
// Valores configurables (cargados desde EEPROM o valores por defecto)

// Valores por defecto
#define DEFAULT_TIEMPO_PARA_ACELERAR 800
#define DEFAULT_SCROLL_MIN -1
#define DEFAULT_SCROLL_MAX -12
#define DEFAULT_PASO_ACELERACION 150
#define DEFAULT_DIRECCION_SCROLL -1  // -1 = abajo, 1 = arriba

// Variables configurables (en RAM, cargadas desde EEPROM en setup)
uint32_t tiempoParaAcelerar = DEFAULT_TIEMPO_PARA_ACELERAR;
int8_t   scrollMin = DEFAULT_SCROLL_MIN;
int8_t   scrollMax = DEFAULT_SCROLL_MAX;
uint32_t pasoAceleracion = DEFAULT_PASO_ACELERACION;
int8_t   direccionScroll = DEFAULT_DIRECCION_SCROLL;

// --- EEPROM Structure ---
#define EEPROM_SIZE 256
#define EEPROM_MAGIC 0x5343524C  // "SCRL"
#define EEPROM_VERSION 1
#define EEPROM_OFFSET_MAGIC 0
#define EEPROM_OFFSET_VERSION 4
#define EEPROM_OFFSET_TIEMPO_PARA_ACELERAR 5
#define EEPROM_OFFSET_SCROLL_MIN 9
#define EEPROM_OFFSET_SCROLL_MAX 10
#define EEPROM_OFFSET_PASO_ACELERACION 11
#define EEPROM_OFFSET_DIRECCION_SCROLL 15
#define EEPROM_OFFSET_CHECKSUM 16

bool configDirty = false;  // Flag para saber si hay cambios sin guardar

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

// Función para calcular checksum simple
uint32_t calculateChecksum() {
  return (uint32_t)tiempoParaAcelerar + (uint32_t)scrollMin + (uint32_t)scrollMax + 
         (uint32_t)pasoAceleracion + (uint32_t)direccionScroll;
}

// Cargar configuración desde EEPROM
void loadConfigFromEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Leer magic number
  uint32_t magic = 0;
  EEPROM.get(EEPROM_OFFSET_MAGIC, magic);
  
  // Leer version
  uint8_t version = 0;
  EEPROM.get(EEPROM_OFFSET_VERSION, version);
  
  // Validar magic number y version
  if (magic != EEPROM_MAGIC || version != EEPROM_VERSION) {
    // EEPROM vacío o corrupto, usar valores por defecto
    tiempoParaAcelerar = DEFAULT_TIEMPO_PARA_ACELERAR;
    scrollMin = DEFAULT_SCROLL_MIN;
    scrollMax = DEFAULT_SCROLL_MAX;
    pasoAceleracion = DEFAULT_PASO_ACELERACION;
    direccionScroll = DEFAULT_DIRECCION_SCROLL;
    return;
  }
  
  // Leer valores
  EEPROM.get(EEPROM_OFFSET_TIEMPO_PARA_ACELERAR, tiempoParaAcelerar);
  EEPROM.get(EEPROM_OFFSET_SCROLL_MIN, scrollMin);
  EEPROM.get(EEPROM_OFFSET_SCROLL_MAX, scrollMax);
  EEPROM.get(EEPROM_OFFSET_PASO_ACELERACION, pasoAceleracion);
  EEPROM.get(EEPROM_OFFSET_DIRECCION_SCROLL, direccionScroll);
  
  // Validar checksum
  uint32_t storedChecksum = 0;
  EEPROM.get(EEPROM_OFFSET_CHECKSUM, storedChecksum);
  uint32_t calculatedChecksum = calculateChecksum();
  
  if (storedChecksum != calculatedChecksum) {
    // Checksum inválido, usar valores por defecto
    tiempoParaAcelerar = DEFAULT_TIEMPO_PARA_ACELERAR;
    scrollMin = DEFAULT_SCROLL_MIN;
    scrollMax = DEFAULT_SCROLL_MAX;
    pasoAceleracion = DEFAULT_PASO_ACELERACION;
    direccionScroll = DEFAULT_DIRECCION_SCROLL;
  }
  
  // Validar rangos
  if (tiempoParaAcelerar < 100 || tiempoParaAcelerar > 10000) tiempoParaAcelerar = DEFAULT_TIEMPO_PARA_ACELERAR;
  if (scrollMin < -127 || scrollMin > 127) scrollMin = DEFAULT_SCROLL_MIN;
  if (scrollMax < -127 || scrollMax > 127) scrollMax = DEFAULT_SCROLL_MAX;
  if (pasoAceleracion < 10 || pasoAceleracion > 1000) pasoAceleracion = DEFAULT_PASO_ACELERACION;
  if (direccionScroll != -1 && direccionScroll != 1) direccionScroll = DEFAULT_DIRECCION_SCROLL;
  
  // Asegurar que scrollMax sea más negativo que scrollMin
  if (scrollMax > scrollMin) scrollMax = scrollMin - 1;
}

// Guardar configuración en EEPROM
void saveConfigToEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Escribir magic number
  EEPROM.put(EEPROM_OFFSET_MAGIC, (uint32_t)EEPROM_MAGIC);
  EEPROM.put(EEPROM_OFFSET_VERSION, (uint8_t)EEPROM_VERSION);
  
  // Escribir valores
  EEPROM.put(EEPROM_OFFSET_TIEMPO_PARA_ACELERAR, tiempoParaAcelerar);
  EEPROM.put(EEPROM_OFFSET_SCROLL_MIN, scrollMin);
  EEPROM.put(EEPROM_OFFSET_SCROLL_MAX, scrollMax);
  EEPROM.put(EEPROM_OFFSET_PASO_ACELERACION, pasoAceleracion);
  EEPROM.put(EEPROM_OFFSET_DIRECCION_SCROLL, direccionScroll);
  
  // Escribir checksum
  uint32_t checksum = calculateChecksum();
  EEPROM.put(EEPROM_OFFSET_CHECKSUM, checksum);
  
  EEPROM.commit();
  configDirty = false;
}

// Restaurar valores por defecto
void resetToDefaults() {
  tiempoParaAcelerar = DEFAULT_TIEMPO_PARA_ACELERAR;
  scrollMin = DEFAULT_SCROLL_MIN;
  scrollMax = DEFAULT_SCROLL_MAX;
  pasoAceleracion = DEFAULT_PASO_ACELERACION;
  direccionScroll = DEFAULT_DIRECCION_SCROLL;
  configDirty = true;
}

// Procesar comandos serial
void processSerialCommand() {
  if (!Serial.available()) return;
  
  String command = Serial.readStringUntil('\n');
  command.trim();
  command.toUpperCase();
  
  if (command.length() == 0) return;
  
  // GET ALL
  if (command == "GET ALL") {
    Serial.print("OK TIEMPO_PARA_ACELERAR=");
    Serial.print(tiempoParaAcelerar);
    Serial.print(" SCROLL_MIN=");
    Serial.print(scrollMin);
    Serial.print(" SCROLL_MAX=");
    Serial.print(scrollMax);
    Serial.print(" PASO_ACELERACION=");
    Serial.print(pasoAceleracion);
    Serial.print(" DIRECCION_SCROLL=");
    Serial.println(direccionScroll);
    return;
  }
  
  // GET <param>
  if (command.startsWith("GET ")) {
    String param = command.substring(4);
    param.trim();
    
    if (param == "TIEMPO_PARA_ACELERAR") {
      Serial.print("OK TIEMPO_PARA_ACELERAR=");
      Serial.println(tiempoParaAcelerar);
    } else if (param == "SCROLL_MIN") {
      Serial.print("OK SCROLL_MIN=");
      Serial.println(scrollMin);
    } else if (param == "SCROLL_MAX") {
      Serial.print("OK SCROLL_MAX=");
      Serial.println(scrollMax);
    } else if (param == "PASO_ACELERACION") {
      Serial.print("OK PASO_ACELERACION=");
      Serial.println(pasoAceleracion);
    } else if (param == "DIRECCION_SCROLL") {
      Serial.print("OK DIRECCION_SCROLL=");
      Serial.println(direccionScroll);
    } else {
      Serial.print("ERROR Unknown parameter: ");
      Serial.println(param);
    }
    return;
  }
  
  // SET <param> <value>
  if (command.startsWith("SET ")) {
    int spaceIndex = command.indexOf(' ', 4);
    if (spaceIndex == -1) {
      Serial.println("ERROR Invalid SET command format");
      return;
    }
    
    String param = command.substring(4, spaceIndex);
    param.trim();
    String valueStr = command.substring(spaceIndex + 1);
    valueStr.trim();
    
    long value = valueStr.toInt();
    bool valid = true;
    
    if (param == "TIEMPO_PARA_ACELERAR") {
      if (value >= 100 && value <= 10000) {
        tiempoParaAcelerar = (uint32_t)value;
        configDirty = true;
        Serial.print("OK TIEMPO_PARA_ACELERAR=");
        Serial.println(tiempoParaAcelerar);
      } else {
        Serial.println("ERROR Value out of range (100-10000)");
      }
    } else if (param == "SCROLL_MIN") {
      if (value >= -127 && value <= 127) {
        scrollMin = (int8_t)value;
        configDirty = true;
        // Validar que scrollMax sea más negativo
        if (scrollMax > scrollMin) scrollMax = scrollMin - 1;
        Serial.print("OK SCROLL_MIN=");
        Serial.println(scrollMin);
      } else {
        Serial.println("ERROR Value out of range (-127 to 127)");
      }
    } else if (param == "SCROLL_MAX") {
      if (value >= -127 && value <= 127) {
        scrollMax = (int8_t)value;
        configDirty = true;
        // Validar que scrollMax sea más negativo que scrollMin
        if (scrollMax > scrollMin) {
          scrollMax = scrollMin - 1;
        }
        Serial.print("OK SCROLL_MAX=");
        Serial.println(scrollMax);
      } else {
        Serial.println("ERROR Value out of range (-127 to 127)");
      }
    } else if (param == "PASO_ACELERACION") {
      if (value >= 10 && value <= 1000) {
        pasoAceleracion = (uint32_t)value;
        configDirty = true;
        Serial.print("OK PASO_ACELERACION=");
        Serial.println(pasoAceleracion);
      } else {
        Serial.println("ERROR Value out of range (10-1000)");
      }
    } else if (param == "DIRECCION_SCROLL") {
      if (value == -1 || value == 1) {
        direccionScroll = (int8_t)value;
        configDirty = true;
        Serial.print("OK DIRECCION_SCROLL=");
        Serial.println(direccionScroll);
      } else {
        Serial.println("ERROR Value must be -1 (abajo) or 1 (arriba)");
      }
    } else {
      Serial.print("ERROR Unknown parameter: ");
      Serial.println(param);
    }
    return;
  }
  
  // SAVE
  if (command == "SAVE") {
    saveConfigToEEPROM();
    Serial.println("OK SAVED");
    return;
  }
  
  // RESET
  if (command == "RESET") {
    resetToDefaults();
    Serial.println("OK RESET");
    return;
  }
  
  // Comando desconocido
  Serial.print("ERROR Unknown command: ");
  Serial.println(command);
}

void setup() {
  pinMode(pinButton, INPUT);
  
  // Inicializar Serial
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);  // Esperar hasta 3 segundos para conexión serial
  }
  
  // Cargar configuración desde EEPROM
  loadConfigFromEEPROM();
  
  // Inicializar HID
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
int8_t   velocidadActual = DEFAULT_SCROLL_MIN;

void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
  #endif

  // Procesar comandos serial (sin bloquear)
  if (Serial.available()) {
    processSerialCommand();
  }

  if (!usb_hid.ready()) return;

  bool estaPresionado = (digitalRead(pinButton) == activeState);

  if (estaPresionado) {
    uint32_t ahora = millis();

    // Si es la primera vez que detectamos la presión
    if (tPresionado == 0) {
      tPresionado = ahora;
      tUltimoReporte = ahora;
      velocidadActual = scrollMin;
    }

    // Calcular cuánto tiempo lleva presionado
    uint32_t duracion = ahora - tPresionado;

    // Lógica de Aceleración:
    // Si pasó el tiempo de espera inicial, empezamos a aumentar la velocidad
    if (duracion > tiempoParaAcelerar) {
        // Cada X milisegundos, aumentamos la potencia del scroll (más negativo)
        int8_t incremento = (duracion - tiempoParaAcelerar) / pasoAceleracion;
        velocidadActual = scrollMin - incremento;
        
        // Limitar a la velocidad máxima
        if (velocidadActual < scrollMax) velocidadActual = scrollMax;
    }

    // Enviar el reporte cada 20ms para mantener fluidez (50Hz de reportes)
    if (ahora - tUltimoReporte >= 20) {
      tUltimoReporte = ahora;
      hi_res_mouse_report_t report = {0};
      // Aplicar dirección del scroll
      report.wheel = velocidadActual * direccionScroll;
      usb_hid.sendReport(REPORT_ID_MOUSE, &report, sizeof(report));
    }
  } 
  else {
    // Botón soltado: resetear todo
    tPresionado = 0;
  }
}