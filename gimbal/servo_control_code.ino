#include <Servo.h>

Servo pitch;  
Servo roll;

boolean pitch_direct = 1; // direction true=forward, false=backward
boolean pitch_stops = true;

boolean roll_direct = true;
boolean roll_stops = true;

void read_value(){
  if(Serial.available()){
    char inChar = (char)Serial.read();
    if(inChar == '='){
      pitch_stops = false;
      pitch_direct = true;
      Serial.println("Pitch Forwards");
    }
    if(inChar == '-'){
      pitch_stops = false;
      pitch_direct = false;
      Serial.println("Pitch Reverse");
    }
    if(inChar == '['){
      pitch_stops = true;
      Serial.println("Pitch Stop");
    }
    if(inChar == 'q'){
      roll_stops = false;
      roll_direct = false;
      Serial.println("Roll Forwards");
    }
    if(inChar == 'w'){
      roll_stops = false;
      roll_direct = true;
      Serial.println("Roll Reverse");
    }
    if(inChar == 'e'){
      roll_stops = true;
      Serial.println("Roll Stop");
    }
  }  
}

int pitch_pos = 0;    // variable to store the servo position
int roll_pos = 0;    // variable to store the servo position


void setup() {
  Serial.begin(115200);
  pitch.attach(9); 
  roll.attach(6);
}

void loop() {

  read_value();

  if(!pitch_stops){
    pitch.attach(9);
    if(pitch_direct){
      pitch_pos = 95;
    }
    else if(!pitch_direct){
      pitch_pos =90;
    }
    pitch.write(pitch_pos);
  }
  else if(pitch_stops){
    pitch.detach();
  }
  if(!roll_stops){
    roll.attach(6);
    if(roll_direct){
      roll_pos = 95;
    }
    else if(!roll_direct){
      roll_pos =90;
    }
    roll.write(roll_pos);
  }
  else if(roll_stops){
    roll.detach();
  }
}
