#include <Wire.h>
#include "i2c.h"
#include "i2c_SI7021.h"
#include "SparkFunMPL3115A2.h"
#define ledPin 13

SI7021 myHumidity;
MPL3115A2 myPressure;
float current_pressure;
float temperature;
float altitude;
float humi;
float temp;
//----------------------
float sea_level = 995; //Barometric pressure at Sea-level in Regina, SK
//----------------------
float ht = -1000; //High Temperaure
float current_temp = 0; //Current Temperature
float lt = 1000; //Low Temperature
//----------------------
float la = 1000; //Low Altitude
float current_alti = 0; //Current Altitude
float ha = 0; //High Altitude
//----------------------
float hb = 0; //High Barometric Pressure
float current_baro = 0; //Current Barometric Pressure
float lb = 100000; //Low Barometric Pressure
//----------------------
float lh = 100000; //Low Humidity
float current_humi = 0; //Current Humidity
float hh = 0; //High Humidity
//----------------------
char retrieve;
int timer1_counter;
int Geiger= 0;
int count = 0;
int latch = 0;
int CPM = 0;
int n = 0;

void get_Geiger();
void get_altitude();
void get_Humidity();
void print_stuff();
void get_values();

void setup()
{
  Wire.begin();
  pinMode(ledPin, OUTPUT);
    pinMode(2, INPUT); 
  myPressure.begin(); // Get pressure online
  myHumidity.initialize(); // Get humidity online
  Serial.begin(9600);

  myPressure.setModeBarometer(); //Measure pressure in Pascals from 20-110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags     
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  n++;
}

void loop()
{
  get_Geiger();
  if(n > 109){ //109 OVF being set means a minute has been served in purgotory...
    get_values();
    print_stuff();
    n = 0;
  }
}

void get_Geiger(){
  if(digitalRead(2)){
    Geiger++;
    while(digitalRead(2)){}
  }   
}

void get_values(){
    //GET PRESSURE
    current_pressure = myPressure.readPressure();
    current_pressure /= 100;
    
    //GET TEMPERATURE
    temperature = myPressure.readTemp();

    //GET ALTITUDE
    get_altitude();

    //GET HUMIDITY
    get_Humidity();

    //STORE BAROMETRIC PRESSURE VALUES
    current_baro = current_pressure;
    if(current_pressure < lb){
      lb = current_pressure;  
      latch = 1;
    }
    if(current_pressure > hb){
      hb = current_pressure;  
      latch = 1;
    }    
    
    //STORE ALTITUDE VALUES
    current_alti = altitude;
    
    if(altitude < la){
      la = altitude;  
      latch = 1;
    }
    if(altitude > ha){
      ha = altitude;  
      latch = 1;
    }

    //STORE HUMIDITY VALUES
    current_humi = humi;
    
    if(humi < lh){
      lh = humi;  
      latch = 1;
    }
    if(humi > hh){
      hh = humi;  
      latch = 1;
    }

    //STORE TEMPERATURE VALUES
    current_temp = temperature;
    
    if(temperature < lt){
      lt = temperature;  
      latch = 1;
    }
    if(temperature > ht){
      ht = temperature; 
      latch = 1;
    }  

    //STORE CPM RADIATION
    CPM = Geiger;
    Geiger = 0;
    if(CPM > 0){
      latch = 1;
    }
}

void get_Humidity(){
    myHumidity.getHumidity(humi);
    myHumidity.getTemperature(temp);
    myHumidity.triggerMeasurement();  
}

void get_altitude(){
  float temp_kpa = sea_level / current_pressure;
  float temp_expo = 1 / 5.257;
  temp_kpa = pow(temp_kpa, temp_expo);
  temp_kpa -= 1;
  float temp_temp = temperature + 273.15;
  temp_kpa *= temp_temp;
  temp_kpa /= 0.0065;
  altitude = temp_kpa;
}

void print_stuff(){
  if(latch){
    Serial.println();     
    delay(50);
    Serial.println("Current Values:");
    delay(50);
    Serial.println("---------------"); 
    delay(50);
    Serial.println(); 
    //------------------------
    Serial.print("Current Humidity:\t");
    delay(50);
    Serial.print(current_humi, 2); 
    delay(50);
    Serial.println();  
    //------------------------
    Serial.print("Current Temperature:\t");
    delay(50);
    Serial.print(current_temp, 2); 
    delay(50);
    Serial.println();  
    //------------------------
    Serial.print("Current Pressure:\t");
    delay(50);
    Serial.print(current_baro, 2); 
    delay(50);
    Serial.println();  
    //------------------------       
    Serial.print("Current Altitude:\t");
    delay(50);
    Serial.print(current_alti, 2);
    delay(50);
    Serial.println();  
    //------------------------  
    Serial.print("Radiation Emission:\t");
    delay(50);
    Serial.println(CPM);
    delay(50);  
    latch = 0;               
  }
}

