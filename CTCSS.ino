/********************************************************************************
 *  modified for SSD1306 display, two pullup buttons with active LOW, based on  *
 *                      ATMEGA328P HAM RADIO CTCSS ENCODER                      *
 *                                Version: V3.1                                 *
 *                                                                              *
 *                Created by Geert Vanhulle - ON7GF - June 2019                 *
 *                                                                              *
 * Released under GPL3 - GNU GENERAL PUBLIC LICENSE - Version 3, 29 June 2007   *    
 *                                                                              *
 *    Derivative work can only be distributed under these same license terms    *
 *    For the full license text goto: fsf.org - The Free Software Foundation    *
 *    a lowpass filter needs to be added to get a nice sinewave                 *
 ********************************************************************************/
 
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4 

Adafruit_SSD1306 display(OLED_RESET);


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// tabel gegevens
byte ocrL[50]; byte ocrH[50];
// ctcss tonen
float ctcssFreq[] =   {  67.0,    69.3,    71.9,    74.4,    77.0,   79.7,    82.5,    85.4,    88.5,    91.5,
                         94.8,    97.4,   100.0,   103.5,   107.2,  110.9,   114.8,   118.8,   123.0,   127.3, 
                        131.8,   136.5,   141.3,   146.2,   151.4,  156.7,   159.8,   162.2,   165.5,   167.9,  
                        171.3,   173.8,   177.3,   179.9,   183.5,  186.2,   189.9,   192.8,   196.6,   199.5,
                        203.5,   206.5,   210.7,   218.1,   225.7,  229.1,   233.6,   241.8,   250.3,    254.1      
                      };

// Change this variable to get the correct tone from the list above
// To do so you need to count the position in the list starting from zero 
// For example: 67Hz = 0  ---  69.3Hz = 1 --- 71.9Hz = 2 --- and so on... 
// Here the variable is set for 100Hz = 12 

int ctcsstone=12;

void setup() 
{
  pinMode(7,INPUT_PULLUP);// minus  
  pinMode(8,INPUT_PULLUP);// plus
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C address of the display
  display.clearDisplay();                     // clear the display buffer
  display.display();                          // update display
}

void loop() 
{  
  if(digitalRead(7)==LOW){ctcsstone++;if(ctcsstone>49){ctcsstone=49;}}
  if(digitalRead(8)==LOW){ctcsstone--;if(ctcsstone<0){ctcsstone=0;}} 
   
 ctcsscalc();
    /* frequentie-deler */ 
    // configuratie Timer1
 TCCR1A = 0x50;
 TCCR1B = 0x0A;
 TCCR1C = 0x00;

 // zet pin PB1 = OCR1A = "D9" als output
 DDRB |= 1 << PB1; 

 // zet het deeltal in het output compare register
 OCR1AH = ocrH[ctcsstone];
 OCR1AL = ocrL[ctcsstone];
  
  displayfreq(); 
  display.clearDisplay(); 
}

void displayfreq() 
{           
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10,5);
  display.print(ctcssFreq[ctcsstone], 2);  // rounded to 2 decimal places
  display.display();                    // update the OLED display with all the new text
}

// bereken deeltal ocrnx voor elke frequentie
void ctcsscalc() {

  int N = 8;
  int clk = 16; // 16 MHz clock

  for (int n=0; n<50; n++) {
  
    int ocrLH = ( clk * 1000000 ) / (( 2 * N * ctcssFreq[n] ) - 1 );
       
    ocrH[n] = ocrLH >> 8;
    ocrL[n] = ocrLH & 0x00FF;

  }
}