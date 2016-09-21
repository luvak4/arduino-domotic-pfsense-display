//================================================================
// Halt, reboot, check status of a pfSense server through Arduino 
// and radio modules. 
// Project is divided in three parts: "keyboard", "servente" and "display".
//
// author: luvak4@gmail.com
//================================================================
//
/////////////////////////////////////
//// this is the "DISPLAY" code ////
/////////////////////////////////////
#include <VirtualWire.h>
#include <LiquidCrystal.h>
// max lenght of my message
const int MSG_LEN = 13;
// position of character to change
const int POSIZIONE_CARATT = 11;
//
const int pinLED =13;
// timing loop
int dutyCycle = 0;
unsigned long int Pa;
unsigned long int Pb;
// radio modules
const int receive_pin = 2; 
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
// display pins
const int RS = 9;
const int Enable = 10;
const int D4 = 5;
const int D5 = 6;
const int D6 = 7;
const int D7 = 8;
// 
String stringaRX;
//
//================================
// initialize the LCD library
//================================
//
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);  
//================================
// setup
//================================
void setup() {
  // led
  pinMode(pinLED, OUTPUT);
  //radio rx
  vw_set_rx_pin(receive_pin);  
  vw_setup(2000);      
  vw_rx_start();
  // display 20x4
  lcd.begin(20, 4);
  ///////////12345678901234567890
  lcd.setCursor(0,0);
  lcd.print("Valk domotica 2016  ");
}
//================================
// loop
//================================
void loop() {
  //--------------------------------
  // time subdivision
  //--------------------------------
  unsigned long int Qa;
  unsigned long int Qb;
  int DIFFa;
  int DIFFb;
  int Xa;
  int Xb;
  //
  dutyCycle += 1;
  if (dutyCycle > 9){
    dutyCycle = 0;
  }
  if (dutyCycle > 0){
    Qa=millis();
    if (Qa >= Pa){
      DIFFa=Qa-Pa;
      Xa = DIFFa - 25;
      if (Xa >= 0){
	Pa = Qa;
	//--------------------------------
	// every 0.025 Sec
	//--------------------------------
      }
    } else {
      Pa = Qa - Xa;
    }
  } else {  
    Qb=millis();
    if (Qb >= Pb){
      DIFFb=Qb-Pb;
      Xb = DIFFb - 1000;
      if (Xb >= 0){
	Pb = Qb - Xb;
	//--------------------------------
	// BEGIN every second
	//--------------------------------
	//--------------------------------
	// BEGIN message received
	//--------------------------------
	if (vw_get_message(buf, &buflen)){
	  //
	  stringaRX="";
	  //
	  // retriving message
	  for (int i = 1; i <= POSIZIONE_CARATT; i++){
	    stringaRX += char(buf[i-1]);
	  }
	  // detect base keyword
	  if (stringaRX=="statPFSE000"){
	    switch (buf[POSIZIONE_CARATT]){
	    case '0':
	      // attempt sync
	      ///////////12345678901234567890
	      lcd.setCursor(0,0);
	      lcd.print("                    ");   
	      break;
	    case '1':
	      // pfSense is in ignition state
	      /////////////12345678901234567890
	      lcd.setCursor(0,0);
	      lcd.print("pfSense: in avvio   ");       
	      break;
	    case '2':
	      // pfSense ON
	      ///////////12345678901234567890
	      lcd.setCursor(0,0);   
	      lcd.print("pfSense: ACCESO     ");          
	      break;
	    case '3':
	      // pfSense in shutdown
	      ///////////12345678901234567890
	      lcd.setCursor(0,0);
	      lcd.print("pfSense: spegnendo  ");              
	      break;
	    case '4':
	      // pfSense is OFF
	      ///////////12345678901234567890
	      lcd.setCursor(0,0);
	      lcd.print("pfSense: SPENTO     ");              
	      break;
	    case '5':
	      // internet ok
	      lcd.setCursor(18,0);
	      lcd.print("  ");       
	      delay(2000);
	      ///////////12345678901234567890
	      lcd.setCursor(18,0);
	      lcd.print("OK");           
	      break;
	    case '6':
	      // internet ko
	      lcd.setCursor(18,0);
	      lcd.print("  ");       
	      delay(2000);          
	      ///////////12345678901234567890
	      lcd.setCursor(18,0);
	      lcd.print("--");      
	      break;
	    case '7':
	      // pfsense received command
	      ///////////12345678901234567890
	      digitalWrite(pinLED, HIGH);
	      lcd.setCursor(17,0);
	      lcd.print("*");
	      delay(1000);
	      lcd.setCursor(17,0);
	      lcd.print(" ");
	      digitalWrite(pinLED, LOW);
	      break;
	    }
	  }
	}
	//
	//--------------------------------
	// END message received
	//--------------------------------
	//--------------------------------
	// END every second
	//--------------------------------	
      }
    } else {
      Pb = Qb;
    }      
  }
}
