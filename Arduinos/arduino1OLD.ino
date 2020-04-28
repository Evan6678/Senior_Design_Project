#include <SPI.h>
#include <MFRC522.h>

//pins
//RFID 13,12,11 are also in use
//Wire 0 -> 3.3V
//Wire 1 -> Pin9
//Wire 2 -> GND
//Wire 3 -> Pin 10
//Wire 4 -> Pin 12
//Wire 5 -> Pin 11
//Wire 6 -> Pin 13
#define SS_PIN 10
#define RST_PIN 9
#define ERR_PIN 4 //connect to red light for RFID
#define RFD_PIN 3 //connect to green light and to Rpi as input for RFID state

//Motion Sensor
#define TRG_PIN 8
#define ECH_PIN 7
#define MTN_PIN 6

//Pressure Plate
#define FSR_PIN A0 // Pin connected to FSR/resistor divider
#define ALM_PIN 2

//UID of the card(s) that gets accessn
#define UID "59 13 25 B3" 
#define UID2 "C2 FE AD 2C"

//Input pin to reset
#define SYSRESET_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 3252.0; // Measured resistance of 3.3k resistor

/*** need to set for specific diamond weights or box dimensions***/
const float BASEWEIGHT = 670; //weight of diamond's base
const int TOLERANCE = 200; //tolerance +- g's
const int MIN_DISTANCE = 32; // If distance is below, it will output on MTN_PIN

int fsrADC;
float force;
bool unlocked = false;
long duration;
int distance;

void setup() {
  //setup for RFID
  //Serial.begin(9600);   // Initiate a //Serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  
  //setup for FSR
  pinMode(FSR_PIN, INPUT); // Pressure plate analog input

  //setup for Motion Sensor
  pinMode(ECH_PIN, INPUT); // Echo pin for motion sensor
  pinMode(TRG_PIN,OUTPUT); // Trig pin for motion sensor

  //setup for RFID
  pinMode(ERR_PIN,OUTPUT); //Output for wrong card entered
  
  //Output signals
  pinMode(ALM_PIN,OUTPUT); //Output for Pressure plate
  pinMode(RFD_PIN,OUTPUT); //Output for RFID unlocked
  pinMode(MTN_PIN,OUTPUT); //Output for motion sensor
  
  //Input signals
  pinMode(SYSRESET_PIN,INPUT); //Input to put system back in default state
}//end of setup

void loop() 
{
  /***SYSRESET_PIN***
  if(digitalRead(SYSRESET_PIN)){
    unlocked=false;
    digitalWrite(ALM_PIN,LOW);
    digitalWrite(RFD_PIN,LOW);
    digitalWrite(MTN_PIN,LOW);
    while(digitalRead(SYSRESET_PIN));
  }***/
  
  /****RFID****/
  
  bool cardFound=true;//true if a valid RFID card is put on sensor
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) cardFound=false; 
  if ( ! mfrc522.PICC_ReadCardSerial()) cardFound=false; 

  //If a card is found
  if (cardFound){
    
    //Show UID on //Serial monitor
    //Serial.print("UID tag :");
    String content= "";
    byte letter;
    
    for (byte i = 0; i < mfrc522.uid.size; i++) {
       //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       //Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }//end of for
    
    //Serial.println();
    //Serial.print("Message : ");
    content.toUpperCase();

    //Check if correct card
    if (content.substring(1) == UID || content.substring(1) == UID2){ //change here the UID of the card/cards that you want to give access
      //Serial.println("Authorized access");
      //Serial.println();
      
      //Toggle state
      unlocked=!unlocked;
      
      //Update output pins
      if (unlocked){
        //digitalWrite(ALM_PIN,LOW);
        digitalWrite(RFD_PIN,HIGH);
      }/*else if (force<BASEWEIGHT-TOLERANCE || fsrADC==0){
        //digitalWrite(ALM_PIN,HIGH);
        digitalWrite(RFD_PIN,LOW);*/
      else
        digitalWrite(RFD_PIN,LOW);  
      //delay for 1 second      
      delay(1000);
      
    }else{
      Serial.println("Access denied");
      digitalWrite(ERR_PIN,HIGH);
      delay(500);
      digitalWrite(ERR_PIN,LOW);
      Serial.println();
      //delay for .5 seconds
      delay(500);
    }//end of if (content.substring(1) == UID)
    
  }//end of if(cardFound)


 /****FSR****/

  //read from analog pin
  fsrADC = analogRead(FSR_PIN);
  
  // If the FSR has no pressure, the resistance will be
  // near infinite. So the voltage should be near 0.
  if (fsrADC != 0){ // If the analog reading is non-zero
    
    //calculate voltage:
    float fsrV = fsrADC * VCC / 1023.0;
    
    // calculate FSR resistance:
    float fsrR = R_DIV * (VCC / fsrV - 1.0);
    //Serial.println("Resistance: " + String(fsrR) + " ohms");
    
    // Approximate force based on slopes in FSR datasheet:
    float fsrG = 1.0 / fsrR; // Calculate conductance
    
    // Break parabolic curve down into two linear slopes:
    if (fsrR <= 600) 
      force = (fsrG - 0.00075) / 0.00000032639;
    else
      force =  fsrG / 0.000000642857;
    Serial.println("Force: " + String(force) + " g");
    Serial.println();

    //Update Alarm output
    if(force<=(BASEWEIGHT+TOLERANCE)){
      //Serial.println("Diamond removed");
      /*if (unlocked)
         digitalWrite(ALM_PIN,LOW);
      else*/
        digitalWrite(ALM_PIN,HIGH);
    }else{
      //Serial.println("Diamond in place");
      digitalWrite(ALM_PIN,LOW);
    }//end of if(force<=(BASEWEIGHT+TOLERANCE))
    
  }else{
    
      //If nothing is on the plate, update Alarm accordingly
      if (unlocked)
        digitalWrite(ALM_PIN,LOW);
      else
        digitalWrite(ALM_PIN,HIGH);
        
  }//end of if (fsrADC != 0)

  /***Ultrasonic Sensor***/
    // Clears the trigPin
  digitalWrite(TRG_PIN, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECH_PIN, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  // Prints the distance on the //Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  
  if (distance<MIN_DISTANCE)
    digitalWrite(MTN_PIN,HIGH);
  else
    digitalWrite(MTN_PIN,LOW);
  //half second delay
  delay(500);

}//end of loop
