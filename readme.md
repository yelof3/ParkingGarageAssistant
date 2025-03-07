Project Garage Stop Lights
============================


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






Further notices:
==========================================

Please note that my modified project has more hardware implemented than Ty Palowski's:
---------------------------------------------------------------------------------------
1. a blue LED to show the power state, connected by a resistor to the 3.3V Pin of the Arduino Nano.

2. a red button to store the desired stop distance to Arduino's EEPROM.
   (the original project looses the value on a restart or power loss and you had to set it again!)
   Automatic storing to EEPROM is not recommended, because it shortens the life of Arduino's EEPROM !

3. a yellow button to read the desired stop distance from the EEPROM and overwrite the displayed value.
   (At boot time, the program automatically reads the stored value from EEPROM).

4. a green button start restart the Arduino, simply connected to Arduino's RESET Pin.
   (You could also pull the mains plug and put it back in, but that's too stupid for me)
   
5. a beeper (not a speaker), which makes a beep sound, when connected to 5 volts.

6. the Ultrasonic distance module is connected via a cable and mounted in a separate box, because you have to mount
   the display unit at eyes height and the ultrasonic at the height of the car's front bumpers.
   (I don't park my car reverse like Ty Palowski)
   

All the wireing settings of the hardware is documented well in the progam code (see above and in the program).
Read carefully the setting and using instructions (see above and in the program).



The further enhancements on the display screen are:
---------------------------------------------------
1. the button functions come with hints and notices when pressed (not the green restart button).

2. the program version number and compile date/time is displayed by pressing 3 seonds the yellow button.

3. At the program version display screen you can hit the yellow button again, so an information is shown about:
   - Computer total UPTIME
   - total SLEEP TIME (when display is OFF)
   - total WAKEUP TIME (when display is running)
   

There is no clock in Arduino.
The Arduino's millis() counts the uptime in milli seconds. It has due to the storage limits of the unsigned long variable
an overflow at 49,7 days.
To be able to show an uptime of more than than 49,7 days, I converted the millis into seconds and
store it in an unsigned long variable.
But every time the millis overflow, I have to check it and sum up my own seconds value.
It's a little bit tricky the do this well.
My routine should be able to show an uptime up to 49710 days, this is about 136 years -- God might give you a very long life ;-)

Everything else is documented in the code.
I enhanced the code from original 210 lines to 1423 lines !!!
The storage capacity of the Arduino Nano is nearly reached (95% used) !

Well, if you have any questions, please email. 
Comments and suggestions also also welcome.

Cheers, Hans (Coolman111)
May 4, 2024,   program version 2.11
