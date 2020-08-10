#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd (7, 6, 5, 4, 3, 2);

void setup() 
{
  // set up the LCD's number of columns and rows:
  lcd.begin (16,2);
}

void loop() 
{
  lcd.setCursor (0,0);
  lcd.print ("I'm the chosen");
  lcd.setCursor (0,1);
  lcd.print ("Je suis le escogé");
  //lcd.scrollDisplayLeft ();        // string moves 1 char every 700ms
  delay (700);

  /*lcd.print (millis( ) / 1000);          // muestra los segudos después de haberse enviado el codigo
  lcd.print (" seg.");*/
