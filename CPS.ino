#include "SPI.h"//serial periphery interface library
#include "MFRC522.h"//rfid library
#include <Servo.h> //Library for Servo motor

#define SS_PIN 10//SS pin of rfid connected to Arduino pin 10 
#define RST_PIN 9//RST pin of rfid connected to Arduino pin 9 

Servo myservo1;//servo variable defined

int IR2 = 4;//IR sensor connected to pin 4 of Arduino

int Slot = 4;//Total number of parking Slots

int val;//Stores value of IR for checking if obstacle present or not

MFRC522 rfid(SS_PIN, RST_PIN);//RFID variable defined

MFRC522::MIFARE_Key key;//RFID key variable defined

void setup()//Void setup is technically a function that you create at the top of each program. Inside the curly brackets is the code that you want to run one time as soon as the program starts running. You set things like pinMode in this section.
{
  Serial.begin(9600);//Serial. begin(9600); passes the value 9600 to the speed parameter. This tells the Arduino to get ready to exchange messages with the Serial Monitor at a data rate of 9600 bits per second. That's 9600 binary ones or zeros per second, and is commonly called a baud rate.
  SPI.begin();//Calling SPI. begin() sets the MOSI, MISO, and SS/CS pin modes to OUTPUT . 
  rfid.PCD_Init();//PCD_Init (void) Initializes the MFRC522 chip. void. PCD_Reset (void) Performs a soft reset on the MFRC522 chip and waits for it to be ready again.
  myservo1.attach(6);//servo motor attached to pin 6 of arduino
  myservo1.write(0);//intially servo motor(gate) is closed
  pinMode(IR2, INPUT);//sets pin to input 
  
  Serial.println("waiting for card...");//message which asks user to use ID card(here:rfid tag) for entry into parking lot
}

void loop()//This is where the bulk of your Arduino sketch is executed. The program starts directly after the opening curly bracket ( } ), runs until it sees the closing curly bracket ( } ), and jumps back up to the first line in loop() and starts all over.
{ 
val = digitalRead(IR2);//val=0->object detected,val=1->no obj
//Loop for increasing available slots in parking lot since a parked car has left the parking lot.
if (val==0&&Slot<=3)
{
  Slot+=1;//slot increased by 1 
  myservo1.write(90);//Gate opens for car exit
  delay(3000);//door open for 3000ms or 3sec
  myservo1.write(0);//Gate closes after car leaves parking lot
  Serial.println(" ");
  Serial.print("No of spots available:");
  Serial.print(Slot);//displays available slot
}
if(digitalRead(IR2)==0&&Slot==4)
{
      Serial.println(" ");
      Serial.println("Parking empty");//displays parking lot is now empty and no car is inside so gate wont open if something is sensed by IR sensor which is inside the parking lot.
}
if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  return;//checks if ID card is present on rfid reader

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);//gets uid of the ID card on rfid reader
 
  // Checks the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K)
      {
        Serial.println(F("Your tag is not of type MIFARE Classic."));//It checks the type of PICC, the :: operator is used to access a const/static member of class or namespace MFRC522.
        return;
      }
  String strID = "";//Blank the string,next line
  for (byte i = 0; i < 4; i++)
  {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");//The code formats UUID as a string of 4 hex bytes (each bytes a pair of hex digits) separated by colons.strid+ used to iterate over 4 bytes of uid
  }
  strID.toUpperCase();//converts lower case letters to upper case for proper checking of UID
  
  delay(500);//waits for 500ms

  if (strID.indexOf("F7:D1:DC:46") >= 0) 
  {  //put your own tap card key;   put your own rfid code of the tag in " C6:05:DA:2B "
   
    Serial.println(" ");
    Serial.println("Authorised access");//The ID card reaveals it is of USER and access to parking lot is granted
    Serial.println("USER");
  
//Below is loop for decreasing parking slots since a new car entered parking lot  

    if(Slot>0)
      {
  
        myservo1.write(90);//gate opens for entry of authorized car
        delay(3000); //gate remains open for 3000ms or 3sec
        myservo1.write(0);//gate closes after 3sec
        Slot = Slot-1;//slot reduced by 1 since new car entered parking lot
        Serial.print("No of spots available:");
        Serial.print(Slot);//displays available slot
      }

else if(Slot<=0)
{
  Serial.print("  Parking Full  "); //Displays parking full and no more cars are allowed even if they are authorized 
}

}
  
else if(strID.indexOf("F7:D1:DC:46") < 0)
{
    Serial.println(" ");
    Serial.println("Access denied");//Displays that wrong ID card is used and entry is prohibited
    myservo1.write(0);//gate remains closed due to usage of wrong id
    digitalWrite(8, HIGH);//sets digital pin 8 ON
    delay(2000);
    digitalWrite(8, LOW);//sets digitalpin 8 OFF
}
}
  
  
