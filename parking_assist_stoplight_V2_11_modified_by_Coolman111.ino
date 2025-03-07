/*
 * Parking Assist Stoplight
 * Created by Ty Palowski
 * https://youtube.com/c/typalowski
 * https://create.arduino.cc/projecthub/typalowski/parking-assist-stoplight-2794d7
 */

/*
 * -------------------------------------------------------------------------------------------------
 * modified and enhanced by Hans-Jürgen Brix (Arduino user name: Coolman111), Nov. 2023 - May 2024
 * -------------------------------------------------------------------------------------------------
 *
 * This C++ sketch is running on a standard Arduino Nano with 30KB flash and 2KB ram memory with an ATmega328P CPU.
 *
 * Sketch uses 29304 bytes (95%) of program storage space. Maximum is 30720 bytes.
 * Global variables use 1077 bytes (52%) of dynamic memory, leaving 971 bytes for local variables. Maximum is 2048 bytes.
 *
 * Modifications:
 * - enhanced comments, for me and all NewBees ;-)
 * - you can use it with cm or inches - you can change it by #define statemant, search for "SETUP OF INTERNATIONAL UNITS"
 * - enhancing the code ( getting more stable ultrasonic results)
 * - enhancing modes of operation:
 *   -- 4th level added (critical after red light), all three LEDs are flashing, you should never pass this line with your car.
 *   -- store on demand (STORE-TO-EEPROM-Button) the desired red light distance value in EEPROM, so after a reboot, your own value is reloaded, not the default.
 *   -- reload on demand (READ-FROM-EEPROM-Button) red light distance value from EEPROM without reboot. 
 *   -- Sleep mode for LEDs: after a configurable Timeout (15 sec, search for "#define LED_TIMEOUT"), LEDs will turn off, if the car does not move.
 *   -- Sleep mode for OLED Display: after the Timeout of the LEDs, and then after another configurable Timeout (15 sec, search for "#define DISPLAY_TIMEOUT"), Display turns off.
 *      In Display Timeout Mode, display dimms down to indicate the timeout.
 *   -- When the car moves with a minimum of 4cm/2in per second, LEDs and display will wake up again. 
 *   -- display CPU Temperature every 5 seconds on OLED. 
 *   -- display program version and build time at long press on READ-FROM-EEPROM-Button,
 *      on a further click, display of System Uptime, Display Sleep Time and Display Wake Time.
 *   -- a beeper confirms the buttons with different numbers of beeps. 
 *
 *
 * Button Functions:
 * - RED button (STORE-TO-EEPROM-Button):     Short click: writes the current stop distance into EEPROM (only if it is different from the stored value). 
 * - YELLOW button (READ-FROM_EEPROM-Button): Short click: reads the stored stop distance from EEPROM and displays it for use.
 *                                            Long Click (3 sec.): displays program information, another short click: displays uptime, another short click: leave.
 *                                            Any time when the display is in sleep mode, you can wake it up by a short click on the YELLOW button.
 * - GREEN button:                            Restarts the Arduino and uses the stop distance stored in EEPROM. The Uptime display will be reset to zero.
 * - Click on Rotary Encoder Button:          Sets the actual distance measured by the ultra sonic into display as new stop distance. If you want to store the value into the EEPROM,
 *                                            you have to use the RED button after this.
 * - Turning Rotary Encoder Button:           you can choose you individual stop distance. If you want to store it forever, you have to click the RED button here after.
 *
 *
 * How to use the Garage Stop Light:
 * ---------------------------------
 * At the first run (when you store the sketch into your Arduino), calibrate the display of the CPU temperature by changing the constant CPUTOFFSET (CPU Temperature Offset).
 * Unfortunately every Arduino NANO chip shows slightly different CPU temperatures, so you have to test.
 * Use a digital Infrared thermometer pointing at the CPU, or if no IR thermometer is available, compare with the room temperature within first 10 minutes after cold CPU.
 *
 * After loading the sketch into your Arduino, you have to mount the Parking Assist Stoplight on the back wall of your garage with the ultrasonic sensor in face of your cars bumpers.
 * When you drive the car into the garage, at a distance of 90cm/36in the green light will turn on to indicate measuring begins.
 * At 60cm/24in the yellow light turns on to indicate pay attention.
 * At 30cm/12in the red light turns on to indicate: stop here.
 * At 15cm/6in, all three LEDs will flash to indicate: WARNING, absolute stop here, danger now
 *   (this WARNING function is very useful when your car occupies nearly all the space in your garage and you have to park very exactly).
 * 
 * At the first run, you have to adjust the minimal distance (for the red light) by turning the encoder knob and store it into EEPROM using the STORE-TO-EEPROM-Button.
 * (the default value, if EEPROM is unused, for US, it is 12 inches, for Europe it is 30 cm)
 *
 * The possible values for the red light setting (turning the Rotary Encoder) are: min 18cm/7in, max 150cm/60in.
 * The maximum range the ultrasonic sensor will check is ~ 400cm/157in.
 *
 * After this, you may test your best minimal distance (red light, turning the rotary encoder button) and store it into EEPROM by pressing the STORE-TO-EEPROM-Button,
 * so this value will be reloaded on every boot of the Arduino.
 * When the new value has been stored, the display will show "Dist=..cm --> EEPROM" or "Dist=..in --> EEPROM" (2 beeps).
 * When the display shows: "Dist=..cm, not stored" or "Dist=..in, not stored" means, the new value is already in EEPROM, storing is not necessary (5 beeps).
 *
 * By pressing the rotary encoder button (1 beep), you can set the actual distance to the object without measuring by hand or turning the encoder button.
 * This new value will not be automatically stored into the EEPROM, if you want to store it, you have to press the STORE-TO-EEPROM-Button.
 * 
 * By pressing the READ-FROM-EEPROM-Button, the EEPROM value will be displayed and overwrite to the current value (1 beep).
 * 
 * Be careful, do not change the program to store the value automatically after each change! The Lifetime of Arduino's EEPROM is only about
 * 100,000 write cycles. Much better is storing into EEPROM on demand only.
 *
 * The Wiring Scheme also has a RESTART Button (attached from GND to RESET Pin), you can reboot the Adruino, for example to reset the uptime display.
 * As well, to enjoy Ty's nice Jeep Logo, the RESTART Button is useful :-)
 *
 * Take care, when wiring, to check all corresponding Pin numbers with the definitions, if not the project will not work.
 * The RESTART Button does not need a pullup resistor, it is simply attached from Gound to the Arduinos RESET PIN.
 * The Beeper is attached with (+) to Pin D3, with (-) to GND. Attention: the beeper is NOT a loud speaker! The beeper makes ONE sound (beep) putting 5 volts on it.
 * The READ-FROM-EEPROM-Button does not need a pullup resistor, the internal pullup is defined (INPUT_PULLUP), attached from Ground to Pin D5.
 * The STORE-TO-EEPROM-Button does not need a pullup resistor, the internal pullup is defined (INPUT_PULLUP), attached from Ground to Pin D7.
 * To display the program version and build time, press the READ-FROM-EEPROM-Button for 3 seconds (3 beeps), then click another time to see the Uptime,
 * the display sleeptime and display waketime values (in day(d)hour:minute:second format).
 * the maximum value which can be displayed is about 49710 days ( ~ 136 years ).
 * ===> the original Arduino's millis() function will overflow to zero after ~ 49 days of uptime !
 *
 * Sleep Timer function for LEDs and OLED display:
 * When the car is stopped or no car is in the garage, the Sleep Timer function is indicated by a "T" in the bottom line of the display followed by a number (e.g. T12).
 * The timer counts until T15, then the LED's are going off and the display will be dimmed.
 * When the timer reaches T30, the display will show "Display --> Sleep" and then shut off.
 * You can wakeup the display and LEDs again by moving the car or clicking the yellow button.
 *
 * Finally, this is the most comfortable enhancement of Ty Palowski's original code. Nearly all of the 30KB of Arduino's program space is used. 
 *
 */


/* Changelog:
 * ------------------------------------------------------------------------------------------------------------------------------------
 * 17.10.2023 V2.05 HJB copied from V2.04, added the documentation of the version of each included library.
 *                      Libraries SPI.h (1.14.5), Adafruit_GFX.h (1.11.9) updated. Seem to work.
 * 28.10.2023 V2.06 HJB display Uptime bug fixed when millis() overflow, now you can see the system uptime for up to 49710 days (~136 years).
 * 30.10.2023 V2.07 HJB bugfix in ConvertSectoDay()
 * 18.11.2023 V2.08 HJB Pin for CLK (rotary encoder) changed back from D6 to D4. Some more comments.
 * 08.03.2024 V2.09 HJB bugfix on millis() / Uptime (seconds) overflow.
 * 08.03.2024 V2.10 HJB another bugfix when multiple overflow of millis().
 * 04.05.2004 V2.11 HJB bugfix, after very long time, sleeptime gets a little big greater then uptime
 *                      because of rounding in update_up_sec_from_millis() and misplaced adding, search for "V2.11"
 */
 

 /* Library documentation
  * -----------------------------------------------------------------------------------------------------------------------------------
  *   Include name      Version installed/updated       Library name                      Description
  * -----------------------------------------------------------------------------------------------------------------------------------
  *  #include <EEPROM.h>                1.8.5           C:\Users\...\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.5\libraries\EEPROM\src\EEPROM.h
  *  #include <SPI.h>                   1.13.2/1.14.5   Adafruit BusIO by Adafruit        SPI & I2C interface  (for oled SSD1306)                                                    
  *  #include <Wire.h>                  1.8.5           C:\Users\...\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.5\libraries\Wire\src\Wire.h (for oled SSD1306)
  *  #include <Adafruit_GFX.h>          1.11.3/1.11.9   Adafruit GFX Library by Adafruit  GFX graphics core library (for oled SSD1306)
  *  #include <Adafruit_SSD1306.h>      2.5.7           Adafruit SSD1306 by Adafruit      SSD1306 oled driver for monochrome 128x64/128x32 display
  *
  */



/* ===== Documentation of Ultrasonic sensor ==========
  // How to convert microseconds to inches or cm 
  // from Arduino.cc documentation of the Ultrasonic Sensor SEN136B5B from Seeedstudio.
  
long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
  // ===== End of Documentation of Ultrasonic sensor ==========
*/


// Set here the program version number
String const PROG_VERS = "2.11";


// *********** SETUP OF INTERNATIONAL UNITS **************************************
// here you can setup if using inches (for US / UK) or cm (for Europe and most other countries)
// Attention! Uncomment ***one*** statement (only one!) , UNIT_US or UNIT_EU or TEST_EU, if not you get compilation errors !
// #define UNIT_US           // uncomment if US (inches and temperature in degrees Fahrenheit)
#define UNIT_EU              // uncomment if EU (cm and temperature in degrees Celsius)
// #define UNIT_TEST_EU      // uncomment if EU (cm), only for testing
// -------------------------------------------------------------------------------

  
  
// ############## Enable / Disable Debug modes ################
// #define DEBUG             // uncomment (enable) if you want some DEBUG prints on serial monitor
// #define DEBUG_U           // uncomment (enable) this debug for the Ultrasonic Sensor only if you need it. It writes a lot of lines to serial monitor


// ##### ONLY FOR TEST ###################################
// *** Setup for Testing millis() function overflow and testing the Uptime Display function on overflow (display_uptime_info) ***
// the millis() function can give a maximum of 4294967295 ms (~49.7 days), after this is begins from 0.
// It starts from 0 at boottime and gives the uptime of the sketch in miili seconds (ms)
// -----> See also definition of variable "unsigned long up_sec =", to simulate a long SYSTEM UPTIME 
// ===> uncomment one of next lines ONLY FOR TEST !
// #define STARTMILLIS 50000      // set a value of your choice for the start value of the millis() function.
// #define STARTMILLIS 4294907295      // max-60 sec  - set a value of your choice for the start value of the millis() function.
// ##### END TEST ####

#if defined STARTMILLIS
extern volatile unsigned long timer0_millis;
#endif
// ##### END ONLY FOR TEST ###############################


// Wireing for Rotary Encoder
int inputCLK = 4;   //Encoder Pin CLK (Clock)
int inputDT = 2;    //Encoder Pin DT (Data)
int inputSW = 8;    //Encoder Pin SW (Switch)

int counter = 0; // difference to light1s when Rotary Encoder has been moved or set.
int currentStateCLK = LOW;
int previousStateCLK = currentStateCLK; 
int switchState = HIGH;

String encdir ="";

// Wireing for Ultrasonic Sensor
#define trigPin 9
#define echoPin 10

// Wireing for RED Button "Store red light distance in EEPROM", also called "STORE-TO-EEPROM-Button"
#define BUTTON_STORE_EEPROM 7       // Button on Pin D7  -> setup as INPUT_PULLUP

// Wireing for YELLOW Button "Read red light distance from EEPROM", also called "READ-FROM-EEPROM-Button"
#define BUTTON_READ_EEPROM 5        // Button on Pin D5  -> setup as INPUT_PULLUP

// Wireing for Beeper (it's not a normal SPEAKER, it is a digital beeper, which sounds a beep tone attaching 5 Volt DC)
#define BEEPER 3                    // Beeper on Pin D3  -> setup as OUTPUT

// Wireing for LEDs
#define led1 12    // red    LED on Pin D12     
#define led2 11    // yellow LED on Pin D11
#define led3 13    // green  LED on Pin D13
// Note: a small blue 3mm LED is used as POWER indicator (+ attached to 3.3V, - attached by a 1 kOhm resistor to GND)


// define Offset for CPU temperature, this depends on CPU and model, you have to calibrate it (--> see doc at begin "How to use it" / second text block)
// when OFFSET higher, then CPU temp lower
// #define CPUTOFFSET    324.31     // adjusted offset, original was 324.31
#define CPUTOFFSET    293.00     // adjusted offset for Arduino Nano V3 clone (#3 -> Test Arduino).
// #define CPUTOFFSET    320.00     // adjusted offset for Arduino Nano V3 clone (#2 -> My garage Arduino).

// Global Constants
const int LED_interval = 200;       // LED blink interval

// Global Variables
int light0;
int light1;
int light2;
int light3;
long duration;
float dist_avg;
float distance;
float distance1;
float dist_m;
float dist_m_prev = 0;
float diff_dist_m_max;
float diff_dist_m;
unsigned long diff_ti = 0;

float last_dist;
float move_ps;
unsigned long last_move;
unsigned long last_outofrange = 0;
int leds_off = 0;
int dimmed = 0;
unsigned long start_timeout = 0;
int timeout = 0;
unsigned long start_for = 0;
unsigned long c_dst = 0;            // display sleeptime in seconds
unsigned long c_dwt = 0;            // display wake time in seconds 
unsigned long c_uptime = 0;         // display field of uptime in seconds (sum of all millis)
char date_buffer[30];
int butEEState = HIGH;
int butSEEState = HIGH;
unsigned long butEETimePressed = 0;
int butmode = 0;
int E_address = 0;
int E_value = -1;

unsigned long last_cput;
double cpu_temp_C; 
double cpu_temp_F; 

int LEDState = LOW;      // ledState used to set the LED
unsigned long LED_prevMillis = 0;

String ts_short = "";
unsigned long currentMillis = 0;


// for the calculation of display sleep time
int displaysleeping = 0;           // state of display sleep (1=yes, 0=no)
unsigned long dst_start = 0;       // start sleep (sec)
unsigned long dst_end = 0;         // end sleep (sec)
unsigned long dst_dur = 0;         // duration of sleep (sec)


// for the calculation of the uptime in sec when millis() overflow
unsigned long up_mi_max = 4294967294;  // limit of unsigned long -> millis()
unsigned long up_mi = 0;
unsigned long up_mi_old = 0;
unsigned long diff_mi = 0;
unsigned long diff_sec = 0;
unsigned long up_sec_old = 0;              // to detect up_sec overflow

unsigned long up_sec = 0;                  // =0 -> normal predefined state of system uptime counter in seconds
// Attention: use one of the following lines only for TEST !!!
// up_sec = 4294967205;      // TEST: simulate nearly maximal runtime (90  sec. until overflow of up_sec -> ~ 136 years)
// up_sec = 4294967115;      // TEST: simulate nearly maximal runtime (180 sec. until overflow of up_sec -> ~ 136 years)



// Definitions of Country dependant units (make changes here only if absolutely necessary)
// ------------------------------------------------------------------------------------------

#if defined  UNIT_US                    // normal area used for USA
  // distances for warning, red, yellow and green lights in inches
  int const light0s = 6;                // 6 inches, e.g. Warning Lights (3 LEDs flashing) turn on 6 in below actual red light setting,          
  //                                       but if actual red light setting is below light1s, warning distance becomes 1/2 of actual red light settimg.  
  int       light1s = 12;               // 12 inches             RED LED, this value can change turning the Rotary Encoder
  int const light2s = 12;               // 12+12=24 inches       YELLOW LED
  int const light3s = 12;               // 12+12+12=36 inches    GREEN LED 
  int const ultrasonic_factor = 73.914; // div. factor 73.914 (for inches)
  int const range_limit = 157;          // measuring limit 157 inches
  int const range_min = 7;              // red light nearest distance 7 inches
  int const range_max = 60;             // red light longest distance 60 inches
  int const range_diff = 2;             // car has to move with a speed of 2 inch per second to awake LEDs and display
  int       warning_limit = light1s;
  #define LED_TIMEOUT 15                // LED timeout in seconds, if the car does not move
  #define DISPLAY_TIMEOUT 15            // Display goes to Sleep Mode in 15 seconds after LED_TIMEOUT
  String const unit = "in";             // Unit for distance
  String const tmode = "";              // if "T", a "T" ist shown in the display io indicate "Test mode" (nothing else)
#endif


#if defined  UNIT_EU                    // normal area used for Europa 
  // distances for warning, red, yellow and green lights in cm
  int const light0s = 15;               // 15cm, e.g. Warning Lights (3 LEDs flashing) turn on 15cm below actual red light setting,
  //                                       but if actual red light setting is below light1s, warning distance becomes 1/2 of actual red light settimg.  
  int       light1s = 30;               // 30cm              RED LED, this value can change turning the Rotary Encoder
  int const light2s = 30;               // 30+30=60cm        YELLOW LED 
  int const light3s = 30;               // 30+30+30=90cm     GREEN LED 
  int const ultrasonic_factor = 29.1;   // div. factor 29.1 (for cm)
  int const range_limit = 400;          // measuring limit 400 cm
  int const range_min = 18;             // red light nearest distance 18 cm
  int const range_max = 150;            // red light longest distance 150 cm
  int const range_diff = 4;             // car has to move with a speed of 4 cm per second to awake LEDs and display
  int       warning_limit = light1s;
  #define LED_TIMEOUT 15                // LED timeout in seconds, if the car does not move
  #define DISPLAY_TIMEOUT 15            // Display goes to Sleep Mode in 15 seconds after LED_TIMEOUT
  String const unit = "cm";             // Unit for distance
  String const tmode = "";              // if "T", a "T" ist shown in the display to indicate "Test mode" (nothing else)
#endif


#if defined  UNIT_TEST_EU               // this test area can be used for testing other values, so you have not to change the settings in the main area
  // TEST distances for warning, red, yellow and green lights in cm
  int const light0s = 15;               // 15cm, e.g. Warning Lights (3 LEDs flashing) turn on 15cm below actual red light setting,
  //                                       but if actual red light setting is below light1s, warning distance becomes 1/2 of actual red light settimg.  
  int       light1s = 30;               // 30cm              RED LED, this value can change turning the Rotary Encoder
  int const light2s = 30;               // 30+30=60cm        YELLOW LED
  int const light3s = 30;               // 30+30+30=90cm     GREEN LED
  int const ultrasonic_factor = 29.1;   // div. factor 29.1 (for cm)
  int const range_limit = 400;          // measuring limit 400 cm
  int const range_min = 18;             // red light nearest distance 18 cm
  int const range_max = 150;            // red light longest distance 150 cm
  int const range_diff = 4;             // car has to move with a speed of 4 cm per second to awake LEDs and display
  int       warning_limit = light1s;
  #define LED_TIMEOUT 10                // LED timeout in seconds, if the car does not move
  #define DISPLAY_TIMEOUT 5             // Display goes to Sleep Mode in 5 seconds after LED_TIMEOUT
  String const unit = "cm";             // Unit for distance
  String const tmode = "T";             // if "T", a "T" ist shown in the display to indicate "Test  mode" (nothing else)
#endif



// for storing red light distance permanent in EEPROM
#include <EEPROM.h>

//For OLED Display SSD1306 (Driver)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin 4 (or -1 if sharing Arduino reset pin)

// physical connection of 1306 OLED display (128x32 pixel), 4 Pins.
// OLED PIN      -->  ARDUINO PIN
// -------------------------------
// GND                GND
// VCC                5V
// SCL                A5 / SCL
// SDA                A4 / SDA


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//#define LOGO_HEIGHT   16
//#define LOGO_WIDTH    16


// Definition of the startup LOGO Bitmap ( Ty's Jeep )
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



// ================== SETUP =============================
void setup()
{
  Serial.begin (115200);          // Put the Serial Monitor of ther Arduino IDE to 115200 Baud !!! If not you will get garbage output !
  while(!Serial);                 // Wait until Serial connection is set up.
  
  ts_short = __TIMESTAMP__;
  ts_short = ts_short.substring(4); 
  Serial.print(F("\n+++++ START Parking_Assist_Stop_Light V"));
  Serial.println(PROG_VERS  + " from " + ts_short + " +++++");
  if (tmode == "T")
  {
    Serial.println(F("\n*** TEST Mode ***\n"));
  }

  if (up_sec > 0)
  {
    Serial.print(F("For TEST, total System Uptime was predefined to (Seconds): "));
    Serial.println(up_sec);
  }



  // call the setMillis() function (only for test)
  #if defined STARTMILLIS
  setMillis(STARTMILLIS);
  #endif

  // setup the Pin Modes for Ultrasonic, LEDs, Buttons and Beeper
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(BUTTON_STORE_EEPROM, INPUT_PULLUP);
  pinMode(BUTTON_READ_EEPROM, INPUT_PULLUP);
  pinMode(BEEPER, OUTPUT);
  blinkLEDs(0);

  attachInterrupt(digitalPinToInterrupt(inputDT), update, CHANGE);

  // Set encoder pins as inputs  
  pinMode (inputCLK, INPUT);
  pinMode (inputDT, INPUT);
  pinMode(inputSW, INPUT_PULLUP);
  digitalWrite(trigPin, LOW);      // ensure trigPin is LOW at start

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed, not enough free dynamic memory!"));
    for(;;); // Don't proceed, loop forever

    // Hint: If you use more than 1287 bytes of dynamic memory then allocation will fail. 
    // If this occurs, try to free more dynamic memory using the F macro in Serial.print function, e.g.:
    // Serial.println(F("static text"));
    //    instead of
    // Serial.println("static text");

  }

  // Clear the buffer to suppress the Adafruit Splash Screen (generated in library Adafruit_SSD1306)
  display.clearDisplay();

  display.setRotation(0);       // <====== change Rotation to 2, if the display shows text head down
  display.dim(0);    // bright
  dimmed = 0;

  // display my Logo at start (Lights and the Jeep)
  display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(61,11);
  display.print(F("Jeep"));
  display.display();

  Serial.println("Greeting");
  greeting();   // LEDs will flash in sequence
  delay(1000);

  E_value = EEPROM.read(E_address);
  Serial.print(F("Reading EEPROM Stored Value at address ")); Serial.print(String(E_address)+"="+String(E_value));
  if (E_value == 255)
  {
    Serial.println(F(", not initialized, using default value of light1s"));
  }
  else
  {
    light1s = E_value;
    Serial.println(F(", copied to Variable light1s."));
  }

  // info display of maximal possible uptime count 
  Serial.print(F("MAX. duration millis() = "));
  Serial.print(ConvertSectoDay(4294967));
  Serial.println(F("   (=4294967295 millis, ~ 49 days)"));
  Serial.print(F("MAX. duration Uptime   = "));
  Serial.print(ConvertSectoDay(4294967295));
  Serial.println(F("   (=4294967295 Seconds, ~ 136 Years)"));

}


// ========== MAIN LOOP ===================
void loop(){

  // update the up_sec (uptime) counter when millis have overflow
  if (millis() < up_mi_old)
  {
    Serial.println(F("Loop: Update from millis overflow"));
    update_up_sec_from_millis();
  }

  // only called when TEST: call the setMillis() function 
  // simulating rapid millis() overflows
  #if defined STARTMILLIS       // STARTMILLIS are defined only on TEST
  if (millis() < 80000)
  {
    setMillis(STARTMILLIS);
  }  
  #endif

  // Check Button of Rotary Encoder
    switchState = digitalRead(inputSW); // Read the digital value of the switch (LOW/HIGH)
    // If the switch is pressed (LOW), print message
  if (switchState == LOW)
  {
    blinkLEDs(0);
    if (displaysleeping == 1)  // when display is sleeping, a click on any button wakes it up and button has no more effect
    {
      reset_LEDTimeout();   // on click on Button turn off sleep mode
      wakeDisplay(&display);
    }
    else
    {
      counter = distance1 - light1s;
      reset_LEDTimeout();   // on click on Button turn off sleep mode
      Beep(1);
      Serial.println("Switch pressed, counter="+String(counter)+", distance1="+String(distance1)+", dist set="+String(counter+light1s));
    }
  }


  // Check the STORE-TO-EEPROM Button:
  butSEEState = digitalRead(BUTTON_STORE_EEPROM);
  if (butSEEState == LOW)
  {
    blinkLEDs(0);
    if (displaysleeping == 1)  // when display is sleeping, a click on any button wakes it up and button has no more effect
    {
      reset_LEDTimeout();   // on click on Button turn off sleep mode
      wakeDisplay(&display);
      delay(2000);
      butSEEState = HIGH;
    }
    else
    {
      reset_LEDTimeout();   // on click on Button turn off sleep mode
      Serial.println(F("Button SHORT -> Store EEPROM"));
      E_value = EEPROM.read(E_address);
      Serial.println("Read EEPROM="+String(E_value)+", New Value="+String(light1));
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(0,0);
      if (E_value != light1)
      {
        EEPROM.update(E_address, light1);
        Serial.println("Write to EEPROM (update mode) at address "+String(E_address)+"="+String(light1));
        display.print("Dist="+String(light1)+unit+"\n--> EEPROM");
        display.display();
        Beep(2);
      }
      else
      {
        Serial.println(F("EEPROM Update not necessary, same content"));
        display.print("Dist="+String(light1)+unit+"\nnot saved");
        display.display();
        Beep(5);
      }
      delay(5000);
      reset_LEDTimeout();   // on click on Button turn off sleep mode
    }
  }


  // Check the READ-FROM-EEPROM Button:
  //   if pressed short (< 1 sec.) then read contents of distance from EEPROM and store it to light1
  //   if pressed long  (> 3 sec.) then show program version on OLED display
  //      after Timeout (15 sec.) returns to normal mode, but if you do a short click, show totals of  Uptime, Display Sleep Time and Display Wake Time in seconds.
  //      after another click or Timeout (30 sec.) returns to normal mode.
  Get_Button();
  if (butmode > 0 && displaysleeping == 1)  // when display is sleeping, a click on any button wakes it up and button has no more effect
  {
    #if defined DEBUG
    Serial.println(F("But-Yel-Sl"));
    #endif
    reset_LEDTimeout();   // on click on Button turn off sleep mode
    wakeDisplay(&display);
    Clear_Button();
    butmode = 0;
  }
  if (butmode == 1)
  {
    blinkLEDs(0);
    reset_LEDTimeout();   // on click on Button turn off sleep mode
    Serial.println(F("Button SHORT -> Read EEPROM"));
    E_value = EEPROM.read(E_address);
    Serial.println("Old_value_light1="+String(light1)+", Read EEPROM="+String(E_value)+", copied to light1s");
    light1s = E_value;
    counter = 0;
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("Dist="+String(light1s)+unit+"\n<-- EEPROM");
    display.display();
    Beep(1);
    delay(5000);
    reset_LEDTimeout();   // on click on Button turn off sleep mode
  }

  else if (butmode == 2) 
  {
    blinkLEDs(0);
    reset_LEDTimeout();   // on click on Button turn off sleep mode
    Serial.println(F("Button LONG -> Show program version"));
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display_prog_info();
    Beep(3);
    Clear_Button();
    start_for = millis();
    for (;;)
    {
      Get_Button();
      if (butmode == 1) { break;}
      if ((millis() - start_for) > 15000) {break;}
    }
    if (butmode == 1)                    // Show uptime info
    {
      display_uptime_info();
      Beep(1);
      start_for = millis();
      for (;;)
      {
        Get_Button();
        if (butmode == 1) { break;}
        if ((millis() - start_for) > 60000) {break;}  // continue after 60 sec
      }
    }
    butmode = 0;
    Beep(1);
    reset_LEDTimeout();
    wakeDisplay(&display);
  }
  else
  {
    #if defined DEBUG
    // Serial.println(F("No Button"));
    #endif
  }


  // Read the Ultrasonic sensor
  int j;
  dist_avg = 0;

  for (j=0; j<3; j++)
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    dist_m = duration / 2.0;
    dist_m = dist_m / ultrasonic_factor;
    if (dist_m > range_limit || dist_m == 0)
    {
      dist_m = range_limit;
    }
    dist_avg += dist_m;
    delayMicroseconds(10);
  }
  dist_avg = dist_avg / j;   
  distance = dist_avg;

  #if defined DEBUG_U         // this debug section writes a lot of lines, so enable only if you need to debug the Ultrasonic sensor
  if (dist_m_prev == 0) {dist_m_prev=distance;}
  if ((millis() - diff_ti) > 10000)
  {
    Serial.println(F("--------- Reset Diff_max ----------"));
    delay(1000);
    diff_ti = millis();
    diff_dist_m_max = 0;
    dist_m_prev = distance;
  }
  diff_dist_m = abs(distance - dist_m_prev); 
  if (diff_dist_m > diff_dist_m_max)
  {
    diff_dist_m_max = diff_dist_m;
  }
  // Serial.println(" Dist="+String(distance)+unit+"  Diff_max="+String(diff_dist_m_max));
  Serial.print(F(" Dist="));
  Serial.print(distance);
  Serial.print(unit);
  Serial.print(F("  Diff_max="));
  Serial.println(diff_dist_m_max);
  // Serial.println("?");
  #endif
  
  dist_m_prev = distance;

  // Prepare distance compare values, allow distance range from 18 to 150 cm / 7 to 60 in.
  light1 = counter + light1s;    // red value, variable with counter from Rotary Encoder
  if (light1 < range_min)
  {
    light1 = range_min;
    counter = light1 - light1s;
  }
  if (light1 > range_max)
  {
    light1 = range_max;
    counter = light1 - light1s;
  }
  light2 = light1 + light2s;     // yellow value, red value + diff. yellow
  light3 = light2 + light3s;     // green value, yellow value + diff. green
  #if defined DEBUG
  // Serial.print(" L1="+String(light1));
  #endif

  // calculate WARNING light value, depending on the Stop Distance (=red light1) set by user
  //   - below red light1 distance -> warning = red light1 dist. / 2
  //   - higher than red light1 distance -> warning = red light1 dist. - warning light0 distance 
  if ( light1 < warning_limit )
  {
    light0 = light1 / 2;
  }
  else
  {
    light0 = light1 - light0s;
  }

  distance1 = distance;


  // Check Arduino 328P CPU temperature every 5 seconds 
  if (millis() - last_cput > 5000)    // check every 5000 milliseconds
  {
    last_cput = millis();
    cpu_temp_C = GetCpuTemp();              // in degrees Celsius
    cpu_temp_F = cpu_temp_C * 1.8 + 32;     // convert to Fahrenheit
    // Serial.print(String(cpu_temp_C,1)+" °C,  ");
    // Serial.println(String(cpu_temp_F,1)+" °F");
  }

  // ---- TIMEOUT handling ----
  // if distance is green or shorter for a long time (car is parked) then turn off LEDs after a timeout.
  // if car moves more than 4 cm in 1 second, or Rotary Switch is used, turn LEDs on again

  #if defined DEBUG
  // Serial.println("DBG 0-Dist="+String(distance)+" light3="+String(light3));
  #endif
  if ( distance < light3 )
  {
    if (millis() - last_move > 500)    // check every 500 milliseconds
    {
      last_outofrange = millis();            // store the time of last out of range
      last_move = millis();            // store the time of last move measuring
      move_ps = abs(last_dist - distance) * 2;    // (500ms * 2) move per second! (cm or inch)     
      last_dist = distance;
      if ( move_ps < range_diff)
      {                                // car has not moved
        if ( timeout > 998 )           // Overflow at 998 sec and set to LED TimeOut + 1
        {
          start_timeout = millis() - ((LED_TIMEOUT + 1) * 1000);
        } 

        if ( start_timeout == 0 )
        {
          start_timeout = millis();
          timeout = 0;
        }

        timeout = abs(millis() - start_timeout) / 1000;
        
        if ( timeout > LED_TIMEOUT )
        {
          leds_off = 1;
          display.dim(1);    // dimm display down
          dimmed = 1;
          #if defined DEBUG
          // Serial.println("DBG 1");
          #endif
        }
        else
        {
          leds_off = 0;
          wakeDisplay(&display);
          #if defined DEBUG
          // Serial.println("DBG 2");
          #endif 
        }
        if ( timeout > LED_TIMEOUT + DISPLAY_TIMEOUT )   // turn off display a little later than the LEDs
        {
          go_sleepDisplay();
        }  
      }
      else  
      {                       // car has moved
        reset_LEDTimeout();   // switch on LEDs 
        wakeDisplay(&display);
        #if defined DEBUG
        // Serial.println("DBG 3");
        #endif 
      }
    }
  }    
  else
  {
    move_ps = -1;    // set Velocity to "---" (only for debug mode)
    display.dim(1);    // dimm display down
    dimmed = 1;
    #if defined DEBUG
    // Serial.print(F("millis="));
    // Serial.print(millis());
    // Serial.print(F("  LOR="));
    // Serial.println(last_outofrange);
    // Serial.print(F("cmp1="));
    // Serial.print(millis() - last_outofrange);
    // Serial.print(F("cmp2="));
    // Serial.println(DISPLAY_TIMEOUT * 1000);
    #endif
    if ((millis() - last_outofrange) > (DISPLAY_TIMEOUT * 1000))    // sleep display only after DISPLAY_TIMEOUT
    {          // Display Timeout for distance > green ended, sleep display 
      #if defined DEBUG
      // Serial.println(F("DBG 4"));
      #endif
      last_outofrange = 0;            // store the time of last out of range
      go_sleepDisplay();
    }
    else      // Display Timeout Counter for distance > green
    {
      if ( start_timeout == 0 )
      {
        start_timeout = millis();
        timeout = 0;
      }
      timeout = abs(millis() - start_timeout) / 1000;
      #if defined DEBUG
      // Serial.println("DBG5-Dist="+String(distance));
      #endif
    }
  }
  #if defined DEBUG
  // Serial.print(" TiO="+String(timeout));
  // if (move_ps > -2)
  // {
  //   Serial.print(F(" --V="));
  //   Serial.println(move_ps);
  // }
  #endif

  // Check the distance and light on the corresponding LEDs
  if (distance < light0 && leds_off == 0)
  { //WARNING LIGHT (the closest distance, danger!)
    #if defined DEBUG
    // Serial.print(" WARN");
    #endif
    blinkLEDs(1);     // blink without delay (1= blink on, 0=  blink off)
  }
  else if (distance < light1 && leds_off == 0)
  { //RED LIGHT  (you should stop here)
    #if defined DEBUG
    // Serial.print(" RED");
    #endif
    LEDState = LOW;
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }
  else if (distance >= light1 && distance < light2 && leds_off == 0)
  { //YELLOW LIGHT  (Attention, you have to stop very soon)
    #if defined DEBUG
    // Serial.print(" YELLOW");
    #endif
    LEDState = LOW;
    digitalWrite(led2,HIGH);
    digitalWrite(led1,LOW);
    digitalWrite(led3,LOW);
  }
  else if (distance >= light2 && distance < light3 && leds_off == 0)
  { //GREEN LIGHT  (you are approaching to Parking Assist)
    #if defined DEBUG
    // Serial.print(" GREEN");
    #endif
    LEDState = LOW;
    digitalWrite(led3,HIGH);
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
  }
  else
  { // LIGHTS OFF  (you are out of range)
    #if defined DEBUG
    // Serial.print(" LED OFF");
    #endif
    LEDState = LOW;
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }
  displayStopDist();
  display.display();
  
  delay(10);   // next loop, do not change!

}
// --- End of main loop ------------


// =============== Functions ==================================
void update()
{
  // Read the current state of inputCLK of the Rotary Encoder by interrupt
  currentStateCLK = digitalRead(inputCLK);
  
  if ((previousStateCLK == LOW) && (currentStateCLK == HIGH))
  {
    if (displaysleeping == 1)
    {
      start_timeout = 0;
      timeout = 0;
      leds_off = 0;
    }
    else
    {
      if (digitalRead(inputDT) == HIGH)
      { 
        counter ++;
      }
      else
      {
        counter --;
      }
      start_timeout = 0;
      timeout = 0;
      leds_off = 0;
    }
  } 
  previousStateCLK = currentStateCLK; 
}

void displayStopDist(){

  // Define d as Stop Distance
  // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  display.setCursor(38,25);
  display.print("T");           // Timeout in sec. after car no moves
  display.print(timeout);
  //Set the cursor coordinates
  display.setCursor(0,0);
  #if defined DEBUG
  display.print("D ");          // distance to car more exact with 2 decimals
  display.print(distance); 
  display.setCursor(60,0);
  display.print("V ");          // car velocity (in cm or inches per second)
  if (move_ps != -1)
  {
    display.print(move_ps); 
  }
  else
  {
    display.print("---");       // no velocity display when car out of range (> green light dist.) 
  }
  display.setCursor(115,0);
  display.print("*");
  display.print(dimmed);          // Dimm state of OLED display, 1=dimmed, 0=not dimmed
  #else
  display.setCursor(0,0);
  display.print("Parking Assist");
  display.setCursor(122,25); 
  display.print(tmode);
  display.setCursor(92,0);
  #if defined UNIT_US
  display.print(cpu_temp_F,1);
  display.print("F");               // CPU temperature in Fahrenheit
  #else
  display.print(cpu_temp_C,1);
  display.print("C");               // CPU temperature in Celsius
  #endif
  #endif
  display.setCursor(0,13); 
  display.print("Stop Dist: ");     // Stop distance (red light) set by user with Rotary Encoder
  display.setTextSize(2);
  display.print(light1);
  display.print(unit);
  display.setTextSize(1);
  display.setCursor(0,25);
  display.print(distance,0);
  display.print(unit);
}

void display_prog_info()            // Show a screen with program info
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Program information");
  display.setCursor(0,14);
  display.print("Parking Assist V");
  display.print(PROG_VERS);
  display.setCursor(0,24);
  display.print(ts_short);
  display.display();
}

void display_uptime_info()            // Show a screen with uptime, sleeptime and waketime info
{
  update_up_sec_from_millis();        // update the up_sec (uptime) from millis()
  c_uptime = up_sec;

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);

  // Calculate:  Wake Time = Uptime - Sleep Time
  // Attention: when Sleep Time becomes greater than Uptime, this occurs ***theoretically*** after 136 years overflow of Uptime ;-), 
  //   then Wake Time will be negative (represented as a big positive number -> unsigned long) 
  c_dwt = c_uptime - c_dst;

  #if defined DEBUG
  Serial.print(F("c_uptime="));
  Serial.print(c_uptime);
  Serial.print(F(" c_dst="));
  Serial.print(c_dst);
  Serial.print(F(" c_dwt="));
  Serial.println(c_dwt);
  #endif

  Serial.print(F("Uptime: "));
  Serial.print(ConvertSectoDay(c_uptime));
  Serial.print(F("  DispSlp: "));
  Serial.print(ConvertSectoDay(c_dst));
  Serial.print(F("  DispWake: "));
  Serial.println(ConvertSectoDay(c_dwt));

  display.print(F("Uptime "));
  display.print(ConvertSectoDay(c_uptime));
  display.print(F("\nDispSlp"));
  display.print(ConvertSectoDay(c_dst));
  display.print(F("\nDispWak"));
  display.print(ConvertSectoDay(c_dwt));
  display.display();
}


void update_up_sec_from_millis()            // update the up_sec (uptime) counter when millis have overflow
{
  up_sec_old = up_sec;
  up_mi = millis();
  #if defined DEBUG
  Serial.print(F("up_mi_old="));
  Serial.print(up_mi_old);
  Serial.print(F(" up_mi="));
  Serial.print(up_mi);
  Serial.print(F(" up_sec_old="));
  Serial.print(up_sec_old);
  Serial.print(F(" up_sec="));
  Serial.println(up_sec);
  #endif

  // Calculate the correct uptime in seconds, even when millis() has overflown one or several times
  if (up_mi >= up_mi_old)
  {
    diff_mi = up_mi - up_mi_old;
  } else {                          // millis() overflow
    diff_mi = up_mi_max - up_mi_old + up_mi;   // calculate the correct difference
    Serial.print(F("1: millis overflow has happened -> "));
    Serial.println(up_mi);
  }
  up_mi_old = up_mi;
  //diff_sec = (diff_mi + 500) /1000;
  diff_sec = diff_mi / 1000;  // V2.11
  up_sec += diff_sec;         // add the difference in sec to uptime in sec.      
  if (up_sec < up_sec_old)
  {
    Serial.print(F("1: up_sec overflow has happened -> "));
    Serial.println(up_sec);
  }
  up_sec_old = up_sec;

  #if defined DEBUG
  Serial.print(F("diff_mi="));
  Serial.print(diff_mi);
  Serial.print(F(" diff_sec="));
  Serial.print(diff_sec);
  Serial.print(F(" up_sec+="));
  Serial.println(up_sec);
  #endif
}


// function convert second into day,
// hours, minutes and seconds
char* ConvertSectoDay(unsigned long n)
{
  // Serial.print(F("Convert SectoDay n="));
  // Serial.println(n);
  unsigned long days = n / 86400;
  // Serial.println(String(days)+"D");
  int hours = n % 86400 / 3600;
  // Serial.println(String(hours)+"h");
  int minutes = n % 86400 % 3600 / 60;
  // Serial.println(String(minutes)+"m");
  int seconds = n % 86400 % 3600 % 60;
  // Serial.println(String(seconds)+"s");
  sprintf(date_buffer, "%5lud%02d:%02d:%02d", days, hours, minutes, seconds);
  // Serial.print(F("Convert SectoDay date_buffer="));
  // Serial.println(date_buffer);
  return date_buffer;
}


void greeting()                     // Flashing all LEDs 5 times
{
  for (int i=0; i <5; i++)
  {
    digitalWrite(led1, HIGH);
    delay(100);
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    delay(100);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
    delay(100);
    digitalWrite(led3, LOW);
  }
}


// blink 3 LEDs without delay (1= blink on, 0=  blink off)
int blinkLEDs(int n)
{
  if (n == 0)
  {
    LEDState = LOW;
    digitalWrite(led1,LEDState);
    digitalWrite(led2,LEDState);
    digitalWrite(led3,LEDState);
    return;
  }
  
  currentMillis = millis();
  if (currentMillis - LED_prevMillis >= LED_interval)
  {
    // save the last time you blinked the LED
    LED_prevMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (LEDState == LOW)
    {
      // Serial.println("***H***");
      LEDState = HIGH;
    } else {
      // Serial.println("===L===");
      LEDState = LOW;
    }

    // set the LED with the LEDState of the variable:
    digitalWrite(led1,LEDState);
    digitalWrite(led2,LEDState);
    digitalWrite(led3,LEDState);
  }
}


// Switch on LEDs
void reset_LEDTimeout()
{
  start_timeout = 0;
  timeout = 0;
  leds_off = 0;
  display.dim(0);    // bright
  dimmed = 0;
}

// Prepare Display Sleep Mode
void go_sleepDisplay()
{
  if (displaysleeping == 0)
  {
    displaysleeping = 1;
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("Display\n--> Sleep");
    display.display();
    delay(1000);
    sleepDisplay(&display);
  }
}

// put Display into Sleep Mode
void sleepDisplay(Adafruit_SSD1306* display) {
  #if defined DEBUG
  Serial.println("Display_Sleep");
  #endif
  display->ssd1306_command(SSD1306_DISPLAYOFF);
  displaysleeping = 1;
  update_up_sec_from_millis();
  dst_start = up_sec;
}

// wake up Display
void wakeDisplay(Adafruit_SSD1306* display) {
  display->dim(0);    // bright
  dimmed = 0;
  if (displaysleeping == 1)
  {
    #if defined DEBUG
    Serial.println(F("Display_WakeUp"));
    #endif
    display->ssd1306_command(SSD1306_DISPLAYON);
    displaysleeping = 0;

    // Calculate the correct display sleeptime in seconds, even when millis() has overflown one or several times
    update_up_sec_from_millis();
    dst_end = up_sec;

    if (dst_end > dst_start)
    {
      dst_dur = dst_end - dst_start;
      c_dst += dst_dur;         // V2.11 add the difference in sec to display sleeptime counter.
    }
    dst_start = dst_end;
    //c_dst += dst_dur;         // V2.11 moved up - add the difference in sec to display sleeptime counter.

    last_outofrange = millis();
    #if defined DEBUG
    Serial.print(F("last SleepTime(sec)="));
    Serial.print(dst_dur);
    Serial.print(F(" WupMillis="));
    Serial.println(last_outofrange);
    #endif
  }
}


int Beep(int beep_times)
{
  for (int i=0; i<beep_times; i++)
  {
    digitalWrite(BEEPER, HIGH);
    delay(20);
    digitalWrite(BEEPER, LOW);
    delay(160);
  }  
}


// Check READ-FROM-EEPROM-Button if pressed, short (< 1 sec.) or long (> 3 sec.) ?
// result is in butmode, 0=not pressed, 1=short press, 2=long press
void Get_Button()
{
  butEEState = digitalRead(BUTTON_READ_EEPROM);
  
  #if defined DEBUG
  // Serial.print("State="+String(butEEState)+" Time="+String(butEETimePressed)+" ms="+String(millis()));
  #endif

  if (butEEState== LOW)                  // Button pressed
  {
    if (butEETimePressed == 0)
    {
      butEETimePressed = millis();       // Store time of 1st press
    } 
    else
    {
      if ((millis() - butEETimePressed) > 3000)  // if button is hold longer than 3 seconds, then it leaves the function with result,
      {                                          // so, operator can see, that he has held the button long enough.
        butmode = 2;    // long press
        butEETimePressed = 0;            // Reset press time
      }
      
    }
  }
  else                                   // Button released or not pressed
  {
    if (butEETimePressed > 0)            // Button was pressed before
    {
      if ((millis() - butEETimePressed) > 3000)
      {
        butmode = 2;    // long press
      }
      else if ((millis() - butEETimePressed) < 1000)
      {
        butmode = 1;    // short press
      }
      else
      {
        butmode = 0;    // a long press between 1 and 3 seconds is not valid
        #if defined DEBUG
        // Serial.print(F("  ***Button invalid***"));
        #endif
      }
      butEETimePressed = 0;              // Reset press time
    }
    else
    {
      butmode = 0;      // button was not pressed
    }
  }
  #if defined DEBUG
  // Serial.println("  Butmode="+String(butmode));
  #endif
}

void Clear_Button()  // wait until button has been released
{
  for (;;)
  {
    Get_Button();
    #if defined DEBUG
    // Serial.println(F("ClearButton"));
    #endif
    if (butmode == 0) { break;}
  }  
}


// Get CPU Temperature
double GetCpuTemp(void)
{
  unsigned int wADC;
  double cpu_temp;

/*
 * The internal temperature has to be used
 * with the internal reference of 1.1V.
 * Channel 8 can not be selected with
 * the analogRead function yet.
 * Source:
 * https://www.engineersgallery.com/arduino-internal-temperature-sensor/
*/
  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
  // t = (wADC - 324.31 ) / 1.22;
  cpu_temp = (wADC - CPUTOFFSET) / 1.22;       // you have to set Constant CPUTOFFSET (at beginning of code) to calibrate your Arduino

  // The returned temperature is in degrees Celcius.
  return (cpu_temp);
}


// Set Millis() to a desired value (ONLY FOR TEST)
#if defined STARTMILLIS
void setMillis(unsigned long millis)
{
  byte sreg = SREG;
  cli();
  timer0_millis = millis;
  SREG = sreg;
  Serial.print(F("For TEST, force Millis() to "));
  Serial.println(timer0_millis);
}
#endif


// ===== End of sketch ======
