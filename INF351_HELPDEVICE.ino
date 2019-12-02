#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

// integer variable to hold current counter value
int count = 0;
int state = 0; 

//config for bluetooth
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
#define FACTORYRESET_ENABLE           1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


#include <Adafruit_FeatherOLED.h>
Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

//lights
#define PIN 11
#define NUMPIXELS 2
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// constants won't change. They're used here to set pin numbers:
const int buttonPin1 = 12;     // red button
const int buttonPin2 = 5; // green button

// variables will change:
int buttonState1 = 0;
int buttonState2 = 1; // variable for reading the pushbutton status
int counter1 = 0;
int counter2 = 0; // the saved variable of counter1
int sadboi1 = 0; // red button is pressed (i don't get it)
int sadboi2 = 0; // the saved variable
int happyboi1 = 0; // green button is pressed (i get it)
int happyboi2 = 0; // the saved variable
int turnoff1 = 0; //turns off lights after the boi becomes happy or sad and 'resets'
int turnoff2 = 0; // saved variable

void setup() {
    
  Serial.begin(115200);

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      Serial.println("Couldn't factory reset");
    }
  }
    /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!
  ble.sendCommandCheckOK("AT+GAPDEVNAME=mydevice");


  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);
  
  // put your setup code here, to run once:
  pixels.begin(); // INITIALIZE Neopixel strip object (REQUIRED)
  Serial.begin(115200);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  

}

void loop() {
  // put your main code here, to run repeatedly:
  // read the state of the pushbutton value:
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  // check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (buttonState1 == LOW && (buttonState2 != LOW)) {   // This is for LED 1
    // turn LED on:
    pixels.setPixelColor (0, pixels.Color(10, 0, 0));
    pixels.show();
    counter1++;
    sadboi1++;
    if (counter1 > 1 && (sadboi1 > 1)) {
      Serial.print(" There are ");
      Serial.print(sadboi1);
      Serial.println(" sad boys.");
      ble.print(" There are ");
      ble.print(sadboi1);
      ble.println(" sad boys.");
    } else {
    Serial.print(" There is ");
    Serial.print(sadboi1);
    Serial.println(" sad boy.");
    ble.print(" There is ");
    ble.print(sadboi1);
    ble.println(" sad boy.");
    delay(3000);
  } } else {
    // turn LED off:
    pixels.setPixelColor (0, pixels.Color(0, 0, 0));
    pixels.show();
 
  }
  delay(50);
  counter2 = counter1;
  sadboi2 = sadboi1;
  
  
    if (buttonState2 == LOW && (buttonState1 != LOW)) {   // LED 2 
    // turn LED on:
    pixels.setPixelColor (1, pixels.Color(0, 10, 0));
    pixels.show();
    counter1++;
    happyboi1++;
    if (counter1 > 1 && (happyboi1 > 1)) {
      Serial.print(" There are ");
      Serial.print(happyboi1);
      Serial.println(" happy boys.");
      ble.print(" There are ");
      ble.print(happyboi1);
      ble.println(" happy boys.");
    } else {
    Serial.print(" There is ");
    Serial.print(happyboi1);
    Serial.println(" happy boy.");
    ble.print(" There is ");
    ble.print(happyboi1);
    ble.println(" happy boy.");
    delay(3000);
  } } else {
    // turn LED off:
    pixels.setPixelColor (1, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(50);
  counter2 = counter1;
  happyboi2 = happyboi1;
  
  if (buttonState1 == LOW && (buttonState2 == LOW)) { // If both buttons are pressed
    Serial.println("Voting Done!");
    if (sadboi1 > happyboi1) {
      Serial.println("Oh no! There are more sad boys!");
      ble.println("Oh no! There are more sad boys!");
      for(int i = 0; i <= 40; i++) {
        pixels.setPixelColor (0, pixels.Color(200, 0, 0)); // flashing lights
        pixels.setPixelColor (1, pixels.Color(200, 0, 0));
        pixels.show();
        delay(50);
        pixels.setPixelColor (0, pixels.Color(0, 0, 0));
        pixels.setPixelColor (1, pixels.Color(0, 0, 0));
        pixels.show();
        delay(50);
      }
      turnoff1++;
    } else {
    Serial.println("Yay! There are more happy boys!");
    ble.println("Yay! There are more happy boys!");
    pixels.setPixelColor (0, pixels.Color(0, 100, 0)); 
    pixels.setPixelColor (1, pixels.Color(0, 100, 0));
    pixels.show();
    turnoff1++;
    delay(1000);
  }
  delay(50);
  

  turnoff2 = turnoff1;
  
  if (buttonState2 == LOW && (turnoff2 == 1)) {
    pixels.setPixelColor (0, pixels.Color(0, 0, 0));
    pixels.setPixelColor (1, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(50);
  counter1 = 0; // reset so voting can go on again
  counter2 = 0;
  sadboi1 = 0;
  happyboi1 = 0;
  sadboi2 = 0;
  happyboi2 = 0;
  turnoff2 = 0;
  turnoff1 = 0;
 }

    

}
