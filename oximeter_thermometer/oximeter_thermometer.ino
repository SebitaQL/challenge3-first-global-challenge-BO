//DISPLAY 16X2
#include <LiquidCrystal.h>

// a liquid crystal displays data
LiquidCrystal lcd (7, 6, 5, 4, 3, 2);


//BEAT SENSOR
#define maxperiod_siz 80 // max number of samples in a period
#define measures 10      // number of periods stored
#define samp_siz 4       // number of samples for average
#define rise_threshold 3 // number of rising measures to determine a peak

int T = 20;              // slot milliseconds to read a value from the sensor
const int sensorPin = A0; 
const int REDLed = 9;
const int IRLed = 8;
const int WHITELed = 13;

byte sym[3][8] =
{
 {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
},{
  B00000,
  B00000,
  B00000,
  B11000,
  B00100,
  B01000,
  B10000,
  B11100
},{
  B00000,
  B00100,
  B01010,
  B00010,
  B00100,
  B00100,
  B00000,
  B00100
}
};

//TEMP SENSOR
#include <OneWire.h>

int HighByte, LowByte, TReading, SignBit, Tc_100;  //needed values
float Whole = 0.0, Fract = 0.0;
 
// DS18S20 Temperature chip i/o
OneWire ds(10);  // on pin 10

void setup()
{
  Serial.begin(9600);
  Serial.flush();
  pinMode(sensorPin,INPUT);
  pinMode(REDLed,OUTPUT);
  pinMode(IRLed,OUTPUT);
  pinMode(WHITELed,OUTPUT);

  lcd.begin (16,2);   // set up the LCD's number of columns and rows:

  //Serial.println("SensandoSignosVitales"); // prints in monitor

  //lcd.setCursor(0,0);
  //lcd.print("Inic. Medicion");  //prints in lsc screen

  // turn off leds
  digitalWrite(REDLed,LOW);
  digitalWrite(IRLed,LOW);

  for(int i=0;i<8;i++) lcd.createChar(i, sym[i]);
  
}

void loop()
{
  bool finger_status = true;
  
  float readsIR[samp_siz], sumIR,lastIR, reader, start;
  float readsRED[samp_siz], sumRED,lastRED;


  int period, samples;
  period=0; samples=0;
  int samplesCounter = 0;
  float readsIRMM[maxperiod_siz],readsREDMM[maxperiod_siz];
  int ptrMM =0;
  for (int i = 0; i < maxperiod_siz; i++) { readsIRMM[i] = 0;readsREDMM[i]=0;}
  float IRmax=0;
  float IRmin=0;
  float REDmax=0;
  float REDmin=0;
  double R=0;

  float measuresR[measures];
  int measuresPeriods[measures];
  int m = 0;
  for (int i = 0; i < measures; i++) { measuresPeriods[i]=0; measuresR[i]=0; }
   
  int ptr;

  float beforeIR;

  bool rising;
  int rise_count;
  int n;
  long int last_beat;
  for (int i = 0; i < samp_siz; i++) { readsIR[i] = 0; readsRED[i]=0; }
  sumIR = 0; sumRED=0; 
  ptr = 0; 
    
  digitalWrite(WHITELed,LOW);

  while(1)
  {
     
    //
    // turn on IR LED
    digitalWrite(REDLed,LOW);
    digitalWrite(IRLed,HIGH);
    
    // calculate an average of the sensor
    // during a 20 ms (T) period (this will eliminate
    // the 50 Hz noise caused by electric light
    n = 0;
    start = millis();
    reader = 0.;
    
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);
    
    reader /= n;  // we got an average
    // Add the newest measurement to an array
    // and subtract the oldest measurement from the array
    // to maintain a sum of last measurements
    sumIR -= readsIR[ptr];
    sumIR += reader;
    readsIR[ptr] = reader;
    lastIR = sumIR / samp_siz;


    
    //
    // TURN ON RED LED and do the same
    
    digitalWrite(REDLed,HIGH);
    digitalWrite(IRLed,LOW);

    n = 0;
    start = millis();
    reader = 0.;
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);  
    
    reader /= n;  // we got an average
    // Add the newest measurement to an array
    // and subtract the oldest measurement from the array
    // to maintain a sum of last measurements
    sumRED -= readsRED[ptr];
    sumRED += reader;
    readsRED[ptr] = reader;
    lastRED = sumRED / samp_siz;


    //                                  
    // R CALCULATION
    // save all the samples of a period both for IR and for RED
    readsIRMM[ptrMM]=lastIR;
    readsREDMM[ptrMM]=lastRED;
    ptrMM++;
    ptrMM %= maxperiod_siz;
    samplesCounter++;
    //
    // if I've saved all the samples of a period, look to find
    // max and min values and calculate R parameter
    if(samplesCounter>=samples)
    {
      samplesCounter =0;
      IRmax = 0; IRmin=1023; REDmax = 0; REDmin=1023;
      for(int i=0;i<maxperiod_siz;i++)
      {
        if( readsIRMM[i]> IRmax) IRmax = readsIRMM[i];
        if( readsIRMM[i]>0 && readsIRMM[i]< IRmin ) IRmin = readsIRMM[i];
        readsIRMM[i] =0;
        if( readsREDMM[i]> REDmax) REDmax = readsREDMM[i];
        if( readsREDMM[i]>0 && readsREDMM[i]< REDmin ) REDmin = readsREDMM[i];
        readsREDMM[i] =0;
      }
      R =  ( (REDmax-REDmin) / REDmin) / ( (IRmax-IRmin) / IRmin ) ;
    }


    float avR = 0;
    int avBPM=0;

    if (finger_status==true)
    {
          
      // lastIR holds the average of the values in the array
      // check for a rising curve (= a heart beat)
      if (lastIR > beforeIR)
      {
        rise_count++;  // count the number of samples that are rising
        if (!rising && rise_count > rise_threshold)
        {
          // Ok, we have detected a rising curve, which implies a heartbeat.
          // Record the time since last beat, keep track of the 10 previous
          // peaks to get an average value.
          // The rising flag prevents us from detecting the same rise 
          // more than once. 
          rising = true;
      
          measuresR[m] = R;
          measuresPeriods[m] = millis() - last_beat;
          last_beat = millis();
          int period = 0;
          for(int i =0; i<measures; i++) period += measuresPeriods[i];
          
          // calculate average period and number of samples
          // to store to find min and max values
          period = period / measures;
          samples = period / (2*T);
            
          int avPeriod = 0;
          
          int c = 0;
          // c stores the number of good measures (not floating more than 10%),
          // in the last 10 peaks
          for(int i =1; i<measures; i++)
          {
            if ((measuresPeriods[i] <  measuresPeriods[i-1] * 1.1) && (measuresPeriods[i] >  measuresPeriods[i-1] / 1.1)) 
            {
              c++;
              avPeriod += measuresPeriods[i];
              avR += measuresR[i];
            }
          }
            
          m++;
          m %= measures;
                        
          //lcd.setCursor(9,0);
          //lcd.print("c= "+String(c)+"  ");
          
          
          // bpm and R shown are calculated as the
          // average of at least 5 good peaks
          avBPM = 60000 / ( avPeriod / c) ;
          avR = avR / c ;
          
          // if there are at last 5 measures 
          lcd.setCursor(9,1);
          if(c==0)
          {
            lcd.print("R=?.??");
            lcd.setCursor(0,0);
            lcd.print("bpm= - ");
            lcd.setCursor(0,1);
            lcd.print("SpO2= - ");
          }
          else
          {
            lcd.print("R=" + String(avR) + " ");
          }
            
          // if there are at least 5 good measures...
          if(c > 2)
          {
            digitalWrite(WHITELed,HIGH);
            // SATURTION IS A FUNCTION OF R (calibration)
            // Y = k*x + m
            // k and m are calculated with another oximeter
            int SpO2 = -19 * R + 102;
              
            lcd.setCursor(0,0);
            if(avBPM > 40 && avBPM <220)
            {
              lcd.print("         ");
              lcd.setCursor(0,0);
              lcd.print("bpm=" + String(avBPM));
            }
            else
            {
              lcd.setCursor(0,0);
              lcd.print("         ");
              lcd.setCursor(0,0);
              lcd.print("bpm= - ");
            }
    
            lcd.setCursor(0,1); 
            if(SpO2 > 70 && SpO2 <150) 
            {
              lcd.print("         ");
              lcd.setCursor(0,1); 
              lcd.print("SpO2=" + String(SpO2) + "%"); //else lcd.print("--% ");
            }
            else
            {
              lcd.setCursor(0,1);
              lcd.print("         ");
              lcd.setCursor(0,1);
              lcd.print("SpO2= - ");
            }
            digitalWrite(WHITELed,LOW);
            avR = (String(avR) == " NAN") ? 0.0 : avR;

            //sends the infomation to the python program
            //Serial.print(String(avBPM)+","+String(SpO2)+","+String(avR)+","+String(c)+"\n"); 
            Serial.print(float(avBPM));
            Serial.print(","); 
            Serial.print(float(SpO2));
            Serial.print(","); 
            Serial.print(float(avR));
            Serial.print(","); 
            Serial.println(float(Whole));                         
          }
        }
      }
      else
      {
        // Ok, the curve is falling
        rising = false;
        rise_count = 0;
        //lcd.setCursor(3,0);lcd.print(" ");
      }
      
      // to compare it with the new value and find peaks
      beforeIR = lastIR;
      
      termometro();//gets the temperature
      
    } // finger is inside
   
    
    // PLOT everything
    //Serial.print(lastIR);
    //Serial.print(",");
    //Serial.print(lastRED);
    //Serial.println();

    // handle the arrays      
    ptr++;
    ptr %= samp_siz;    
  } // loop while 1
}


void termometro()
{
  lcd.setCursor(8,0);
  lcd.print("tmp=");
     
  //For conversion of raw data to C
 
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
 
  if (!ds.search(addr))
  {
    //Serial.print("No more addresses.\n");
    ds.reset_search();
    return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);
 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {           // 9 bytes are needed
    data[i] = ds.read();
  }
  
  //Conversion of raw data to C
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
 
  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 10000;

  Whole += Fract;  //gets a single float value 
  Whole /= 100;
 
  //prints on Serial Monitor
  //Serial.print(Whole);
  //Serial.print("\n");

  //prints on lcd
  lcd.setCursor(12,0);
  lcd.print("    ");
  lcd.setCursor(12,0);
  lcd.print(Whole);
  //End conversion to C
}
