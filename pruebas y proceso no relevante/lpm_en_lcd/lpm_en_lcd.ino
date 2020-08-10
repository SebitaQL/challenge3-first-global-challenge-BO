#include <LiquidCrystal.h> //lcd library

// initializes the library with the numbers of the interface pins
LiquidCrystal lcd (7, 6, 5, 4, 3, 2);

float factor = 0.75;    // coeficiente para filtro pasa bajos
float maximo = 0.0;   // para almacenar valor maximo 
int minimoEntreLatidos = 300; // 300 mseg. de tiempo minimo entre latidos
float valorAnterior = 500;  // in order to keep the previus value
int latidos = 0;    // contador de cantidad de latidos y ciclos de medicion
float ciclo = 1.0;

void setup()
{
  Serial.begin(9600);   // initializes the serie monitor at 9600 bps

  lcd.begin (16,2);   // set up the LCD's number of columns and rows
  
  pinMode(13, OUTPUT);    // LED incorporado en pin 13 como salida
  
  Serial.println("Iniciando mediciones"); // prints in monitor

  lcd.setCursor(0,0);
  lcd.print("IniciandoMedicion");  //prints in lsc screen

}

void loop()
{
  static unsigned long tiempoLPM = millis();  // tiempo Latidos Por Minuto con
            // valor actual devuelto por millis()
  static unsigned long entreLatidos = millis(); // tiempo entre Latidos con
            // valor actual devuelto por millis()

  int valorLeido = analogRead(A0);    // lectura de entrada analogica A0

  float valorFiltrado = factor * valorAnterior + (1 - factor) * valorLeido; // filtro pasa bajos
  float cambio = valorFiltrado - valorAnterior;   // diferencia entre valor filtrado y
              // valor anterior
  valorAnterior = valorFiltrado;    // actualiza valor anterior con valor filtrado

  if ((cambio >= maximo) && (millis() > entreLatidos + minimoEntreLatidos)) // si cambio es
  {
          // es mayor o igual a maximo y pasaron al menos 300 mseg.
    maximo = cambio;      // actualiza maximo con valor de cambo
    digitalWrite(13, HIGH);   // enciende LED incorporado
    entreLatidos = millis();    // actualiza variable entreLatidos con millis()
    latidos++;        // incrementa latidos en uno
  }
  else        // condicion falsa del condicional
  {
    digitalWrite(13, LOW);    // apaga LED incorporado
  }
  
  maximo = maximo * 0.97;   // carga maximo como el 97 por ciento de su propio
          // valor para dejar decaer y no perder pulsos

  if (millis() >= tiempoLPM + 15000)    // si transcurrieron al menos 15 segundos
  {
    Serial.print("Latidos por minuto:\t"); // muestra texto
    Serial.println(latidos * 4);    // muestra variable latidos multiplicado por 4

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print(latidos * 4);
    lcd.print("..........lpm");

    ciclo+=1.0;
    latidos = 0;        // coloca contador de latidos en cero
    tiempoLPM = millis();     // actualiza variable con valor de millis()
  }
  
  lcd.setCursor(0,1);
  lcd.print("actualiza en:");
  lcd.print((ciclo * 15) - (millis()/1000));

  delay(50);        // demora entre lecturas de entrada analogica
}
