
//ARDUINO 1.0+ ONLY
//ARDUINO 1.0+ ONLY
#include <Ethernet.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h> 

// initialize the 4 bit LCD library with the numbers of the interface pins
LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );

////////////////////////////////////////////////////////////////////////
//CONFIGURE
////////////////////////////////////////////////////////////////////////
byte server[] = { 192,168,1,1 }; //ip Address of the server you will connect to
//The location to go to on the server
//make sure to keep HTTP/1.0 at the end, this is telling it what type of file it is
String location = "/arduino/redditapi.php HTTP/1.0";
// if need to change the MAC address (Very Rare)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
////////////////////////////////////////////////////////////////////////
EthernetClient client;
char inString[128]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?
String lastValue = "";

Servo myservo;
int servoPos = 10;    // variable to store the servo position 

//Top leds
int LEDLR = 42;
int LEDLG = 44;
int LEDLB = 40;
int LEDRR = 50;
int LEDRG = 48;
int LEDRB = 46;
int LEDLstate = HIGH;
int LEDRstate = HIGH;

//timers
long tnow, tnextServo, tnextScroll, tnextConnect, tnextLED = 0;

int alert = 0;

void setup(){
  
  pinMode(LEDLR, OUTPUT);
  pinMode(LEDLG, OUTPUT);
  pinMode(LEDLB, OUTPUT);
  pinMode(LEDRR, OUTPUT);
  pinMode(LEDRG, OUTPUT);
  pinMode(LEDRB, OUTPUT);
  
  myservo.attach(28); //11,10,9,6,5,3
  myservo.write(10);
  //myservo.write(80);
  
  // set up the LCD's number of rows and columns:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Hello! Display");
  delay (800);
  lcd.setCursor(0, 1);
  lcd.print("API data example");
  delay (1400);
  
//  Ethernet.begin(mac);
  
  //check IP address
  if (Ethernet.begin(mac) == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to get IP");
    lcd.setCursor(0, 1);
    lcd.print("Check cable");
    clearLeds();
    errorLeds();
    delay(1400);
    clearLeds();  
  }else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("My IP address is");
    lcd.setCursor(0, 1);
    lcd.print(Ethernet.localIP());
    clearLeds();
    successLeds();
    delay(1400);
    clearLeds();    
    
  }
  
  Serial.begin(9600);
}

void loop(){
  tnow = millis();
  if(tnow>=tnextLED){
    if(alert == 1){
      flashLeds(tnow);
      flagUp();
    }else{
      //clearLeds();
      flagDown();
    }
    tnextLED = tnow + 500;
  }
  /*
  if(tnow>=tnextServo){
    testServo();
    tnextServo = tnow + 50;
  }  
  */
  
  if(tnow>=tnextConnect){
    Serial.println("Connect");

    String pageValue = connectAndRead(); //connect to the server and read the output
    Serial.println(pageValue); //print out the findings.
    
    //check if result has changed
    if(pageValue != lastValue){
      lastValue = pageValue;
      alert = 1;
    }else{
      //stop lights
      alert = 0;
    }
    
    if(pageValue == "failedconn"){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Could not");
      lcd.setCursor(0, 1);
      lcd.print("Connect");     
     //check again sooner
     clearLeds();
     errorLeds();
     
     tnextConnect = tnow + 5000;
    }else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(pageValue);
      clearLeds();
      //dont check again so soon
      tnextConnect = tnow + 15000;  
    }
  }
  if(tnow >= tnextScroll){
    lcd.scrollDisplayLeft();
    tnextScroll = tnow + 200;
  }
  

}


void flashLeds(long tnow){
  if(LEDLstate == HIGH){
    LEDLstate = LOW;
    LEDRstate = HIGH;    
  }else{
    LEDLstate = HIGH;
    LEDRstate = LOW;        
  }
  digitalWrite(LEDLR, LEDLstate);
  digitalWrite(LEDRB, LEDRstate);
//  tnextLED = tnow + 500;
}
void clearLeds(){
  LEDLstate = HIGH;        
  LEDRstate = HIGH;        
  digitalWrite(LEDLR, LEDLstate);
  digitalWrite(LEDLG, LEDLstate);  
  digitalWrite(LEDLB, LEDRstate);
  digitalWrite(LEDRR, LEDLstate);
  digitalWrite(LEDRG, LEDLstate);  
  digitalWrite(LEDRB, LEDRstate);  
}
void successLeds(){
    LEDLstate = LOW;
    LEDRstate = LOW;    
  digitalWrite(LEDLG, LEDLstate);
  digitalWrite(LEDRG, LEDRstate);
}
void errorLeds(){
    LEDLstate = LOW;
    LEDRstate = LOW;    
  digitalWrite(LEDLR, LEDLstate);
  digitalWrite(LEDRR, LEDRstate);
}


void flagUp(){
  myservo.write(110);                    
}
void flagDown(){
  myservo.write(10);                    
}

void testServo(){
  if(servoPos < 90){                                  
    servoPos = servoPos + 1;
  } else{
    servoPos = 10;
  }
  Serial.println(servoPos);
  myservo.write(servoPos);                    
}

String connectAndRead(){
//String connectAndRead(){
  //connect to the server

  Serial.println("connecting...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting..");
  //delay(2000);

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    lcd.setCursor(0, 1);
    lcd.print("Connected");
    client.print("GET ");
    client.println(location);
    client.println();

    //Connected - Read the page
    //Serial.println(readPage());
    //alert = 1;
    return readPage(); //go and read the output

  }else{
    /*lcd.setCursor(0, 1);
    lcd.print("Failed");*/
    //delay(2500);
    Serial.println("connection failed");
    //return "connection failed";
    //alert = 0;
    //return a custom failed string so we can display a formal one later
    return "failedconn";
  }
  

}

String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  memset( &inString, 0, 128 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '<' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
      }else if(startRead){

        if(c != '>'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //got what we need here! We can disconnect now
          startRead = false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");

          return inString;

        }

      }
    }

  }

}
