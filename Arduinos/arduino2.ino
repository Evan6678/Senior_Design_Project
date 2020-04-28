#include "Arduino.h"
#include "Keypad.h"

//pins that go to pi to define the current defense state
#define ST0_PIN 0
#define ST1_PIN 1
//other pins
#define KPD_PIN 11 //Output to pi and green LED
#define ERR_PIN 12 //Hook up buzzer and Red LED
const byte CODESIZE = 4; 
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6,7,8,9}; //connect to the row pinouts of the keypad - four in a row
byte colPins[COLS] = {2,3,4,5}; //connect to the column pinouts of the keypad - four in a row
char code[CODESIZE] = {'2','4','8','7'};

//create key mapping
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Array to hold up to CODESIZE numbers
char enteredCode[CODESIZE]={};
int i=0;
bool same=false;
bool active=true;//true if keypad has not been disarmed

void setup() {
  pinMode(ST0_PIN,OUTPUT);//Bit0 of state
  pinMode(ST1_PIN,OUTPUT);//Bit1 of state
  pinMode(KPD_PIN,OUTPUT);//Green LED
  pinMode(ERR_PIN,OUTPUT);//Buzzer and red LED
  Serial.begin(9600);
}

void loop() {
    //wait for mode select
    char key=keypad.waitForKey();
    beep();
    
    if (key=='#'){
      bool wrong = false;
      int mode;
      key=keypad.waitForKey();
      beep();
      if (key=='A'){
        mode=0;
      }else if (key=='B'){
        mode=1;
      }else if (key=='C'){
        mode=2;
      }else if (key=='D'){
        mode=3;
      }else{
        mode = -1;
      }
      
      key=keypad.waitForKey();
      beep();
      if (codeCorrect(key)){
        if (mode==0){
          //Serial.println("Mode 0");
          digitalWrite(ST0_PIN,LOW);
          digitalWrite(ST1_PIN,LOW);
        }else if (mode==1){
          //Serial.println("Mode 1");
          digitalWrite(ST0_PIN,LOW);
          digitalWrite(ST1_PIN,HIGH);
        }else if (mode==2){
          //Serial.println("Mode 2");
          digitalWrite(ST0_PIN,HIGH);
          digitalWrite(ST1_PIN,LOW);
        }else if (mode==3){
          //Serial.println("Mode 3");
          digitalWrite(ST0_PIN,HIGH);
          digitalWrite(ST1_PIN,HIGH);
        }else{
          //if mode selected incorrectly, treat as wrong code
          digitalWrite(ERR_PIN,HIGH);
          delay(1000);
          digitalWrite(ERR_PIN,LOW);
          //Serial.println("Wrong");
        }
      }else{
        digitalWrite(ERR_PIN,HIGH);
        delay(1000);
        digitalWrite(ERR_PIN,LOW);
        //Serial.println("Wrong");
      }
    }else if (codeCorrect(key)){
      if (active){ //if deactivating, just turn green light on
        digitalWrite(KPD_PIN,HIGH);
        active=false;
      }else{ //if reactivating, wait for them to enter a # followed by A, B, C, or D and 
        digitalWrite(KPD_PIN,LOW);
        active=true;  //set state pins accordingly. Then turn off green light
      }//end of else
    }else{
      digitalWrite(ERR_PIN,HIGH);
      delay(1000);
      digitalWrite(ERR_PIN,LOW);
      //Serial.println("Wrong");
    }
}


//function to print out code
void output(char x[4]){
  for (int i=0;i<4;i++){
    Serial.print(x[i]);
  }
  Serial.println();

}

void beep(){
    //Buzzer
    digitalWrite(ERR_PIN,HIGH);
    delay(80);
    digitalWrite(ERR_PIN,LOW);
}

bool codeCorrect(char first){
  char key;
  enteredCode[0]=first;
  for (int i=1;i<CODESIZE;i++){
    key=keypad.waitForKey();
    beep();
    //Add to key array
    enteredCode[i]=key;
  }
  //output(enteredCode);
  //output(code);
  //if you have entered the number of keys equal to length of code

  //check for equality
  for (int i=0;i<CODESIZE;i++){
    if (code[i]!=enteredCode[i]){
      //Serial.println("False");
      return false;
    }
  }
  
  return true;
}
