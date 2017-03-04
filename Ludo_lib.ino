/*********************************************************
This is Ludo's code

Designed specifically to work with the MPR121 Breakout from Adafruit
**********************************************************/
#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <math.h>

//******************************
// SECTION THERMISTANCE 
//******************************
#define PIN_NTC 0

double Rref = 10000.0; //Résistance de référence à 25°C
double V_IN = 5.0; //Alimentation électrique

//Information de la thermistance
double A_1 = 3.354016E-3;
double B_1 = 2.569850E-4;
double C_1 = 2.620131E-6;
double D_1 = 6.383091E-8;

double SteinhartHart(double R)
{
 //Division de l'équation en 4 parties. La premiere est 
 //uniquement A1
 double equationB1 = B_1 * log(R/Rref);
 double equationC1 = C_1 * pow(log(R/Rref), 2);
 double equationD1 = D_1 * pow(log(R/Rref), 3);
 double equation = A_1 + equationB1 + equationC1 + equationD1;
 return pow(equation, -1);

}

//******************************
// SECTION MRP121
//******************************
// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

//******************************
// SECTION RELAIS
//******************************
#define RELAY_1 2
#define RELAY_2 3
#define RELAY_3 4
#define RELAY_4 5

void pulseRelay(int pin) {
   // pendant 3s on allume la led du montage
   digitalWrite(pin, LOW);
   delay(3000);
   // et on l'éteind
   digitalWrite(pin, HIGH);
}

//******************************
// SETUP
//******************************
void setup() {
  Serial.begin(9600);

  //** SETUP DU MPR121 **
  //*********************
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
 
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
 
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  //** SETUP DU RELAIS **
  //*********************
  // on initialise les sorties et on éteind les leds connectées
  pinMode(RELAY_1, OUTPUT);  
  digitalWrite(RELAY_1, HIGH);
  pinMode(RELAY_2, OUTPUT);  
  digitalWrite(RELAY_2, HIGH);
  pinMode(RELAY_3, OUTPUT);  
  digitalWrite(RELAY_3, HIGH);
  pinMode(RELAY_4, OUTPUT);    
  digitalWrite(RELAY_4, HIGH);
}

//******************************
// LOOP
//******************************
void loop() {
  //** LOOP THERMISTANCE **
  //*********************

  //Calcul de la tension sur la borne analogique
  double valeurAnalog = analogRead(PIN_NTC);
  double V =  valeurAnalog / 1024 * V_IN;

  //Calcul de la résistance de la thermistance
  double Rth = (Rref * V ) / (V_IN - V);
  Serial.print("Rth = ");
  Serial.print(Rth);

  //Calcul de la température en kelvin( Steinhart and Hart)
  double kelvin = SteinhartHart(Rth);
  double celsius = kelvin - 273.15; //Conversion en celsius
  Serial.print("Ohm  -  T = ");
  Serial.print(celsius);
  Serial.print("C\n");

  //** LOOP MPR121 **
  //*******************
  // Get the currently touched pads
  currtouched = cap.touched();
 
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }
  // reset our state
  lasttouched = currtouched;

  // enlever le commentaire pour les infos de debug
  /*
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  */

  //** LOOP RELAIS **
  //*******************
   // on allume la led de chaque relai chacune à son tour
   pulseRelay(RELAY_1);
   pulseRelay(RELAY_2);
   pulseRelay(RELAY_3);
   pulseRelay(RELAY_4);
}


