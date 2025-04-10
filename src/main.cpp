#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "utilidades.h"

#define punto 25
#define linea 26 
#define led 27

// Dirección I2C del LCD y tamaño de la pantalla
LiquidCrystal_I2C lcd(0x27, 16, 2); 
// Bluetooth
BluetoothSerial SerialBT;

void setup() {
	
	Serial.begin(115200);
	Wire.begin(21, 22);

	lcd.init(); //inicializa la lcd
	lcd.backlight(); // Enciende la luz de fondo
	lcd.setCursor(0, 0); // columna 0, fila 0
	lcd.clear();

	SerialBT.begin("ESP32_Receptor"); // Nombre Bluetooth del dispositivo
	Serial.println("Esperando señales Morse por Bluetooth...");

	// botones para puntos y líneas
	pinMode(punto, INPUT_PULLDOWN);
	pinMode(linea, INPUT_PULLDOWN);
	pinMode(led, OUTPUT);
}

String codigoActual = "";
String codigoEnvio = "";
unsigned long ultimoTiempo = 0;
unsigned long tiempoActual = 0;
unsigned long ultimoTiempoEnv = 0;
unsigned long tiempoActualEnv = 0;

const unsigned long pausaLetra = 500;    // 1500 milisegundos
const unsigned long pausaPalabra = 1000;  // 2500 milisegundos

u_int8_t col = 0;
u_int8_t col2 = 0;

unsigned long tiempo1 = 0; //debounce de la línea 
unsigned long tiempo2 = 0; //debounce del punto

/// Variales para recibir
bool flag_imprimir = false;
bool flag = true; //para evitar que se imprima más de un espacio seguido
char letra = ' ';

/// Variables para enviar
bool flag_imprimirEnv = false;
bool flagEnv = true; //para evitar que se imprima más de un espacio seguido
char letraEnv = ' ';

// Variables para detectar flanco de subida
bool estadoAnteriorPunto = LOW;
bool estadoAnteriorLinea = LOW;

// Comprobación de conexión
bool conectado = false;

void loop() {

	// Led de conexión
	while (!conectado) {
		delay(500);
		digitalWrite(led, HIGH);
		delay(500);
		digitalWrite(led, LOW);
		String mesConectado = "";
		mesConectado = SerialBT.read();
		if (mesConectado == "Conectado!") {
			digitalWrite(led, HIGH);
			conectado = true;
		}
	}

	tiempoActual = millis();
	tiempoActualEnv = millis();
	//################### RECIBIR DATOS ################### 
	// Ver si hay datos disponibles por Bluetooth
	while (SerialBT.available()) {
		char recibido = SerialBT.read();
		if (recibido == '.' || recibido == '-') {
			codigoActual += recibido;
			ultimoTiempo = millis();
			pitido(recibido);
		}
	}

	//detectar fin de letra esperar 1 segundo
	if (codigoActual.length() > 0 && (tiempoActual - ultimoTiempo > pausaLetra)) {
		letra = buscarLetra(codigoActual);
		codigoActual = "";
		flag = false; //se baja la flag
		flag_imprimir = true;
		Serial.print(letra);
		ultimoTiempo = millis();
	}

	//detectar fin de palabra esperar 2 segundos 
	if(!flag && letra == ' '){ 
		if (tiempoActual - ultimoTiempo > pausaPalabra) {
			flag = true; //se sube la flag
			Serial.print("_");
			flag_imprimir = true;
			ultimoTiempo = millis();
		}
	}

	//imprimit en la LCD mensaje recibido
	if(flag_imprimir){
		lcd.setCursor(col, 0);
		lcd.print(letra);
		letra = ' ';
		col++;
		if (col >= 16) {
			col = 0;
			lcd.setCursor(col, 0);
			delay(1000);
			lcd.print("                ");
		}
		flag_imprimir = false;
	}
	
	//################### ENVIAR DATOS ###################
	while (Serial.available()) {
		char enviado = Serial.read();
		if (enviado == '.' || enviado == '-') {
		  	codigoEnvio += enviado;
		  	ultimoTiempoEnv = millis();
		  	pitido(enviado);
			Serial.println(codigoEnvio);
		}
	}
	// Leer estados actuales de botones
  bool estadoActualPunto = digitalRead(punto);
  bool estadoActualLinea = digitalRead(linea);

	// Botón punto con debounce
	if (estadoActualPunto == HIGH && estadoAnteriorPunto == LOW &&
    (millis() - tiempo2) > 100) {
    codigoEnvio += '.';
    SerialBT.write('.');
    Serial.println(codigoEnvio);
    ultimoTiempoEnv = millis();
    pitido('.');
    tiempo2 = millis();
}

// Botón línea con debounce y flanco de subida
if (estadoActualLinea == HIGH && estadoAnteriorLinea == LOW &&
    (millis() - tiempo1) > 100) {
    codigoEnvio += '-';
    SerialBT.write('-');
    Serial.println(codigoEnvio);
    ultimoTiempoEnv = millis();
    pitido('-');
    tiempo1 = millis();
}

	// Actualizar estados anteriores
	estadoAnteriorPunto = estadoActualPunto;
	estadoAnteriorLinea = estadoActualLinea;
	

	//detectar fin de letra esperar 1 segundo
	if (codigoEnvio.length() > 0 && (tiempoActualEnv - ultimoTiempoEnv > pausaLetra) && digitalRead(punto) == LOW) {
		letraEnv = buscarLetra(codigoEnvio);
		codigoEnvio = "";
		flagEnv = false; //se baja la flag
		flag_imprimirEnv = true;
		ultimoTiempoEnv = millis();
	}
	
	//detectar fin de palabra esperar 2 segundos 
	if(!flagEnv && letraEnv == '_'){ 
		if (tiempoActualEnv - ultimoTiempoEnv > pausaPalabra) {
		  	flagEnv = true; //se sube la flag
		  	flag_imprimirEnv = true;
		  	ultimoTiempoEnv = millis();
		}
	}
	
	// Imprimir en el LCD el mensaje a enviar
	if(flag_imprimirEnv){
		lcd.setCursor(col2, 1);
		lcd.print(letraEnv);
		letraEnv = '_';
		col2++;
		if (col2 >= 16) {
		  	col2 = 0;
		  	lcd.setCursor(col2, 1);
		  	delay(1000);
		  	lcd.print("                ");
		}
		flag_imprimirEnv = false;
	}
}
