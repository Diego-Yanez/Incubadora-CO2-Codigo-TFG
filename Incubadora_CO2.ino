#include <Wire.h>  // Incluimos la librería para la comunicación I2C
#include <LiquidCrystal_I2C.h>  // Incluimos la librería para controlar el LCD por I2C
#include "DHT11.h"  // Incluimos la librería para el sensor DHT11
#include "MQ135.h"  // Incluimos la librería para el sensor MQ-135

// Declaración de variables para los pines
const int MQ135_PIN = A0; // Definimos el pin de conexión del sensor MQ-135
const int DHT11_PIN = 11; // Definimos el pin de conexión del sensor DHT11

// Pines para los botones
const int button1Pin = 6;
const int button2Pin = 7;
const int button3Pin = 8;
const int button4Pin = 9;

// Pines para los relés
const int relePin1 = 2; // Pin del primer relé (para la bombilla)
const int relePin2 = 3; // Pin del segundo relé (para el ventilador 1)
const int relePin3 = 4; // Pin del tercer relé (para el ventilador 2)
const int relePin4 = 5; // Pin del cuarto relé (para el humidificador)

// Dirección I2C del LCD
const int I2C_ADDR = 0x27;

// Creamos un objeto de la clase LiquidCrystal_I2C
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);  // Inicializamos el LCD de 16x2

// Creamos un objeto de la clase DHT11
DHT11 dht(DHT11_PIN);

// Creamos un objeto de la clase MQ135
MQ135 mq135(MQ135_PIN);

// Valores deseados iniciales
int desiredTemp = 34;
int desiredHumidity = 70;

void setup() {
  // Inicialización de la comunicación serial
  Serial.begin(9600);

  // Inicialización del LCD
  lcd.init();
  lcd.backlight();  // Encendemos la retroiluminación del LCD

  // Inicialización del sensor DHT11
  dht.setDelay(500); // Establecemos un retraso entre lecturas de 0.5 segundos

  // Configuramos los pines de los relés como salida
  pinMode(relePin1, OUTPUT);
  pinMode(relePin2, OUTPUT);
  pinMode(relePin3, OUTPUT);
  pinMode(relePin4, OUTPUT);

  // Configuramos los pines de los botones como entrada con resistencia pull-up interna
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);

  // Leemos los valores iniciales de temperatura, humedad y CO2 y actualizamos el LCD
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();
  int co2 = mq135.getPPM();
  updateLCD(co2, temperature, humidity);
}

void loop() {
  // Lectura de temperatura y humedad del sensor DHT11
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();
  int co2 = mq135.getPPM();

  // Control del relé #1 según la temperatura
  if (temperature < desiredTemp) {
    // Encendemos el relé #1 (activamos la bombilla)
    digitalWrite(relePin1, LOW);
  } else {
    // Apagamos el relé #1 (apagamos la bombilla)
    digitalWrite(relePin1, HIGH);
  }

  // Control de los relés #2 y #3 para los ventiladores
  if (temperature > desiredTemp) {
    // Encendemos los relés #2 y #3
    digitalWrite(relePin2, LOW);
    digitalWrite(relePin3, LOW);
  } else {
    // Apagamos los relés #2 y #3
    digitalWrite(relePin2, HIGH);
    digitalWrite(relePin3, HIGH);
  }

  // Control del relé #4 según la humedad
  if (humidity < desiredHumidity) {
    // Encendemos el relé #4
    digitalWrite(relePin4, LOW);
  } else {
    // Apagamos el relé #4
    digitalWrite(relePin4, HIGH);
  }

  // Leemos y actualizamos los valores de los botones
  if (digitalRead(button1Pin) == LOW) {
    desiredTemp = min(desiredTemp + 1, 38); // Incrementamos la temperatura deseada
    delay(200); // Retardo para evitar rebotes
  }
  if (digitalRead(button2Pin) == LOW) {
    desiredTemp = max(desiredTemp - 1, 20); // Decrementamos la temperatura deseada
    delay(200); // Retardo para evitar rebotes
  }
  if (digitalRead(button3Pin) == LOW) {
    desiredHumidity = min(desiredHumidity + 1, 95); // Incrementamos la humedad deseada
    delay(200); // Retardo para evitar rebotes
  }
  if (digitalRead(button4Pin) == LOW) {
    desiredHumidity = max(desiredHumidity - 1, 50); // Decrementamos la humedad deseada
    delay(200); // Retardo para evitar rebotes
  }

  // Actualizamos el LCD con los valores actuales y deseados
  updateLCD(co2, temperature, humidity);
}

// Función para actualizar el LCD con los valores de CO2, temperatura, humedad y los valores deseados
void updateLCD(int co2, int temperature, int humidity) {
  lcd.clear(); // Limpiamos el LCD
  lcd.setCursor(0, 0); // Establecemos el cursor en la primera línea
  lcd.print(co2); // Mostramos el valor de CO2
  lcd.print("PPM "); // Mostramos las unidades de CO2
  lcd.print(temperature); // Mostramos el valor de temperatura
  lcd.print("C "); // Mostramos las unidades de temperatura
  lcd.setCursor(0, 1); // Establecemos el cursor en la segunda línea
  lcd.print("T_D:"); // Mostramos la etiqueta de temperatura deseada
  lcd.print(desiredTemp); // Mostramos la temperatura deseada
  lcd.print("C "); // Mostramos las unidades de temperatura deseada
  lcd.print("H_D:"); // Mostramos la etiqueta de humedad deseada
  lcd.print(desiredHumidity); // Mostramos la humedad deseada
  lcd.print("%"); // Mostramos las unidades de humedad deseada
  
  // Esperar un breve período antes de actualizar la pantalla nuevamente
  delay(1000);
}