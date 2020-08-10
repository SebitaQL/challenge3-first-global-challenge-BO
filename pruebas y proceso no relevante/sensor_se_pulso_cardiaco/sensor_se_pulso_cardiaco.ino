// Pulse Monitor Test Script
int sensorPin = 0;
void setup() {
   Serial.begin(9600);
}
void loop ()
{
   while(1)
   {
     Serial.println(analogRead(sensorPin));
   }
} 
