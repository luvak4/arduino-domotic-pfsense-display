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
//  radio rx (4) >---!         !---> display LCM E   (11)
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
const int MSG_LEN = 13;
const int POSIZIONE_CARATT = 11;
const int pinLED =13;
#include <LiquidCrystal.h>
// 
// for my timing
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
String stringaRX;
//
  // initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);  
//================================
// setup
//================================
void setup() {
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
	//




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
    if (stringaRX=="statPFSE000"){
      //Serial.println(buf[POSIZIONE_CARATT]);
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
          ///////////12345678901234567890
          lcd.setCursor(18,0);
          lcd.print("OK");           
          break;
        case '6':
          // internet ko
          ///////////12345678901234567890
          lcd.setCursor(18,0);
          lcd.print("--");      
          break;
        case '7':
          // pfsense received command
          ///////////12345678901234567890
          lcd.setCursor(17,0);
          lcd.print("*");
          delay(1000);
          lcd.setCursor(17,0);
          lcd.print(" ");
         break;
      }
    }

  
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
