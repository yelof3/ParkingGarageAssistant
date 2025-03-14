/*
 * Parking Assist Stoplight
 * Created by Ty Palowski
 * https://youtube.com/c/typalowski
 */

//For Rotary Encoder
int inputCLK = 4; //Encoder Pin CLK (Clock)
int inputDT = 2; //Encoder Pin DT (Data)
int inputSW = 8; //Encoder Pin SW (Switch)

int counter = 0; 
int currentStateCLK = LOW;
int previousStateCLK = currentStateCLK; 
int switchState = HIGH;

String encdir ="";

//For Ultrasonic Sensor
#define trigPin 9
#define echoPin 10

#define led1 12
#define led2 11
#define led3 13

int light1;
int light2;
int light3;
int distance1;

int const light1s = 12;
int const light2s = 24;
int const light3s = 48;

//For OLED Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//#define LOGO_HEIGHT   16
//#define LOGO_WIDTH    16
const unsigned char myBitmap [] PROGMEM = 
{ // 'stoplight and jeep, 128x32px
  0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x01, 0x80, 0x06, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x01, 0x80, 0x06, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xfc, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xe0, 0x07, 0x80, 0x1c, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xf0, 0x3f, 0xfc, 0x00, 0x07, 0xc1, 0xe0, 0x07, 0x80, 0x0e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xf0, 0x1f, 0xfc, 0x00, 0x07, 0xc1, 0xe0, 0x07, 0x80, 0x1e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xf0, 0x1f, 0xf8, 0x00, 0x06, 0xc1, 0xe0, 0x07, 0x80, 0x3e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xf0, 0x1f, 0xf0, 0x00, 0x07, 0xc1, 0xb0, 0x06, 0xc0, 0x3f, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xf0, 0x3f, 0xe0, 0x00, 0x07, 0xcf, 0xf8, 0x07, 0xc0, 0x3f, 0xff, 0xff, 0x80, 0x00, 
  0x00, 0x0f, 0xf8, 0x7f, 0xc0, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
  0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x00, 
  0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
  0x00, 0xff, 0xfe, 0xff, 0xfc, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
  0x00, 0xff, 0xf0, 0x3f, 0xfc, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
  0x00, 0xff, 0xf0, 0x1f, 0xfc, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x7f, 0xf0, 0x1f, 0xf8, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x3f, 0xf0, 0x1f, 0xf8, 0x00, 0x07, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x1f, 0xf0, 0x3f, 0xe0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x0f, 0xf8, 0x3f, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
  0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
  0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 
  0x00, 0xff, 0xf8, 0x7f, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xf6, 0x00, 0x00, 0x6f, 0xff, 0xc0, 0x00, 
  0x00, 0xff, 0xf0, 0x3f, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 
  0x00, 0xff, 0xf0, 0x1f, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 
  0x00, 0x7f, 0xe0, 0x1f, 0xf8, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 
  0x00, 0x3f, 0xf0, 0x1f, 0xf0, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 
  0x00, 0x0f, 0xf0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 
  0x00, 0x0f, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 
  0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
  0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{ Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

attachInterrupt(digitalPinToInterrupt(inputDT), update, CHANGE);

   // Set encoder pins as inputs  
   pinMode (inputCLK, INPUT);
   pinMode (inputDT, INPUT);
   pinMode(inputSW, INPUT_PULLUP);

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.setRotation (2);

  display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void loop(){
// BUTTON
  switchState = digitalRead(inputSW); // Read the digital value of the switch (LOW/HIGH)
  // If the switch is pressed (LOW), print message
if (switchState == LOW) {
   Serial.println("Switch pressed");
   counter = distance1 - light1s;
  }

 long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); //2
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 73.914; //29.1(cm)
  light1 = counter + light1s;
  light2 = light1 + light2s;
  light3 = light2 + light3s;
  distance1 = distance;


if (distance < light1)
 { //RED LIGHT
  digitalWrite(led1,HIGH);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
 }
else if (distance > light1 && distance < light2)
 { //YELLOW LIGHT
  digitalWrite(led2,HIGH);
  digitalWrite(led1,LOW);
  digitalWrite(led3,LOW);
 }
else if (distance > light2 && distance < light3)
 { //GREEN LIGHT
  digitalWrite(led3,HIGH);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
 }
else
 {
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
 }
  displayStopDist();
  display.display();
}

void update() {
    // Read the current state of inputCLK
   currentStateCLK = digitalRead(inputCLK);
   
   if ((previousStateCLK == LOW) && (currentStateCLK == HIGH)) {  
   if (digitalRead(inputDT) == HIGH) { 
       counter --;
        }
   else {
       counter ++;
        }
      } 
   previousStateCLK = currentStateCLK; 
}

void displayStopDist(){

  // Define d as Stop Distance
  float d = light1;
  float c = distance1;
  // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  //Set the cursor coordinates
  display.setCursor(0,0);
  display.print("Parking Stop Assist");
  display.setCursor(0,11); 
  display.print("Stop Dist: "); 
  display.setTextSize(2);
  display.print(d,0);
  display.print("in");
  display.setTextSize(1);
  display.setCursor(0,25);
  //display.print("Temperature: "); 
  display.print(c,0);
  display.print("in");
}