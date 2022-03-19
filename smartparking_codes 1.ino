#include <ESP8266WiFi.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "parking-17dfa-default-rtdb.firebaseio.com"                 // the project name address from firebase id
#define FIREBASE_AUTH "M6ztbuy2pvkmWHGopuihIpFYhOfrmACBiyfVbEPh"            // the secret key generated from firebase

#define WIFI_SSID "MU-STUDENT-1"                                             // input your home or public wifi name 
#define WIFI_PASSWORD "mahedubai"                                    //password for Wifi
String Available = "";                                                  //availability string
String fireAvailable = "";

LiquidCrystal_I2C lcd(0x3F, 16, 2);         //i2c display address 27 and 16x2 lcd display
Servo myservo;                          //servo as gate
Servo myservos;                               //servo as gate
int Empty=0;                            //available space integer
int allSpace = 10;
int countYes = 0;
int carEnter = D0;                      // entry sensor 
int carExited = D4;                   //exi sensor

int pos;
int pos1;


void setup() {
  delay(1000);
  Serial.begin (9600);     // serial debugging
  Wire.begin(D2, D1);       // i2c start
  myservo.attach(D6);      // servo pin to D6
  myservos.attach(D5);       // servo pin to D5

  pinMode(carExited, INPUT);    // ir as input
  pinMode(carEnter, INPUT);     // ir as input

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);                          // display ssid
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");                          // if not connected print this
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);        // begin firebase authentication

  lcd.begin(0,0,0);                           //begin lcd
 lcd.home();                            
  lcd.setCursor(0, 0);                      // 0th row and 0thh column
lcd.print("Smart Parking");    
}

void loop() {
  int carEntry=1;
  int carExit=1;

  carEntry = digitalRead(carEnter);      // read ir input
   Serial.print(carEntry);
  if (carEntry == 0 && countYes <= 10) {                     // if high then count and send data
    countYes++;                             //increment count
    Serial.print("Car Entered = " ); 
    Serial.println(countYes);
   lcd.setCursor(0, 1);
   lcd.print("Car Entered");
  
   for (pos = 45; pos <= 140; pos += 1){        // change servo position
      myservos.write(pos);                       
      delay(5);
    }
    delay(2000);
    
  for (pos = 140; pos >= 50 ; pos -= 1) 
    {       
      myservos.write(pos);
      delay(5);
    }
     Empty = allSpace - countYes;         //calculate available data
  
  if(Empty == 0)
  { 
  Available = String("Parking is Full");
  }
  else{
  Available = String("Available= ") + String(Empty) + String("/") + String(allSpace);       // convert the int to string
  fireAvailable = String("Available=") + String(Empty) + String("/") + String(allSpace);
  }
  
      Firebase.pushString("/Parking Status/", fireAvailable );    // send string to firebase
   lcd.clear();
  }

  
   carExit = digitalRead(carExited);              //read exit ir sensor
    Serial.println(carExit);
 
  if (carExit == 0) {                            //if high then count and send
    countYes--;                                    //decrement count
    Serial.print("Car Exited = " ); 
    Serial.println(countYes);
    lcd.setCursor(0, 1);
    lcd.print("Car Exited");
    for (pos1 = 140; pos1 >= 45; pos1 -= 1) {         // change servo position
      myservo.write(pos1);
      delay(5);
    }
    delay(3000);

    for (pos1 = 45; pos1 <= 140; pos1 += 1) {            // change servo position
      // in steps of 1 degree
      myservo.write(pos1);
      delay(5);
    }
 Empty = allSpace - countYes;         //calculate available data
  
  if(Empty == 0)
  { 
  Available = String("Parking is Full");
  }
  else{
  Available = String("Available= ") + String(Empty) + String("/") + String(allSpace);       // convert the int to string
  fireAvailable = String("Available=") + String(Empty) + String("/") + String(allSpace);
  }
  
   Firebase.pushString("/Parking Status/", fireAvailable );  // send string to firebase
  lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print(Available);                 //print available data to lcd
  
}
