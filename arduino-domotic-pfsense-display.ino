//================================================================
// Halt, reboot, check status of a pfSense server through Arduino 
// and radio modules. 
// Project is divided in three parts: "keyboard", "servente" and "display".
//
// author: luvak4@gmail.com
//================================================================
// System hardware is divided into 3 parts:
// "servente": communicates via serial RS-232 with pfSense
// "keyboard": performs a remote control. It has 4 buttons. Give commands.
// "display": receive "servente" signals (and other devices, eventually) 
// and display its status on LCM or light.
//
// "servente": receive radio-commands from "keyboard" and transmit response to "display"
// "keyboard": transmit radio-commands to "servente"
// "display": receive radio-commands from "servente"
//
// !----------!               !----------!               !---------!
// ! keyboard !--> tx   rx >--! servente !--> tx   rx >--! display !
// !----------!               !          !               !---------!
//                            !          !
//                            !          !               !---------!
//                            !          !--<> rs232 <>--!         !
//                            !          !               ! pfSense !
//                            !----------!               !---------!
//////////////////////////////////////////
//// this is the "display" schematic ////
//////////////////////////////////////////
//
//                   !---------!
//  radio rx (2) >---!         !---> display LCM E   (11)
//                   ! display !---> display LCM R/S (12)
//                   !         !
//                   !         !---> display LCM D4   (5)
//                   !         !---> display LCM D5   (4)
//                   !         !---> display LCM D6   (3)
//                   !         !---> display LCM D7   (2)
//                   !---------!
//
//
/////////////////////////////////////
//// this is the "servente" code ////
/////////////////////////////////////
#include <VirtualWire.h>
#include <LiquidCrystal.h>
// 
// for my timing
int dutyCycle = 0;
unsigned long int Pa;
unsigned long int Pb;
// radio modules
const int receive_pin = 4; 
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
// commands that can be receive
//////////////////////////////123456789012
const String msg00  ="statPFSE0000";
const String msg01  ="statPFSE0001";
const String msg02  ="statPFSE0002";
const String msg03  ="statPFSE0003";
const String msg04  ="statPFSE0004";
const String msg05  ="statPFSE0005";
const String msg06  ="statPFSE0006";
const String msg07  ="statPFSE0011";
// display pins
const int RS = 12;
const int Enable = 11;
const int D4 = 5;
const int D5 = 4;
const int D6 = 3;
const int D7 = 2;
//================================
// setup
//================================
void setup() {
  // initialize the library with the numbers of the interface pins
  LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);  
  //radio rx
  vw_set_rx_pin(receive_pin);  
  vw_setup(2000);      
  vw_rx_start();
  // display 20x4
  lcd.begin(20, 4);
  ///////////12345678901234567890
  lcd.setCursor(0,0);
  lcd.print("Luvak Ar-domotica ;-)";
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("                    ");
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
	//
	//--------------------------------
	// message received
	//--------------------------------
	if (vw_get_message(buf, &buflen)){
	  String stringaRX="";
	  // retriving message
	  for (int i = 0; i < buflen; i++){
	    stringaRX += char(buf[i]);
	  }
	}
	if(stringaRX==msg00){
	  // all 'leds' are off
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("                    ";	  
	}
	if(stringaRX==msg01){
	  // ignition state
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("pfSense: in avvio   ";	  
	}
	if(stringaRX==msg02){
	  // on
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);	  
	  lcd.print("pfSense: ACCESO     ";	  	  
	}
	if(stringaRX==msg03){
	  // shutdown
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("pfSense: spegnendo  ";	  
	}
	if(stringaRX==msg04){
	  // halted
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("pfSense: SPENTO     ";	  	  
	}
	if(stringaRX==msg05){
	  // internet ok
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("internet OK         ";	  	  	  
	}
	if(stringaRX==msg06){
	  // internet ko
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("no conness.internet ";	  	  	  	  
	}
	if(stringaRX==msg07){
	  // internet ko
	  ///////////12345678901234567890
	  lcd.setCursor(0,0);
	  lcd.print("< comando ricevuto >";	  	  	  	  
	}	    
	//
	//--------------------------------
	// END every second
	//--------------------------------	
      }
    } else {
      Pb = Qb;
    }      
  }
}

