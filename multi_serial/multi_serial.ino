// Multi-Serial Sketch 0.2
// by: CyberPopPunk
// started: 3/31/19
// updated: 4/6/19

/*
    TODO:
    1. REFINE COUNTDOWN/COUNTUP FOR TIMER
    2. ADD GLITCH FUNCTIONALITY to input parse
    3. SETUP LIGHT INPUT
    4. SWITCH STATEMENT FOR INPUT PARSE
    5. DOCSTRINGS!
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define DISPLAY_ADDRESS   0x70

Adafruit_7segment clockDisplay = Adafruit_7segment();

//*****Display Variables
bool colon = true;
int currentMinutes;
int currentSeconds;
int serial_minutes;
int serial_seconds;

//**********Serial Variables
String inputString = "";
bool stringComplete = false;


//************ DOOR VARIABLES
bool door_open;

void setup() {
  Serial.begin(115200);
  Serial.println("Clock Starting!");

  //reserve 200 bytes for the Serial input string
  inputString.reserve(200);

  //initialize clock display at I2C address from above
  clockDisplay.begin(DISPLAY_ADDRESS);
  blank();
  delay(1000);

}

void loop() {
  /*
    if (stringComplete) {
    inputParse(inputString);
    reset_values();
    }
  */
  /*setSegDisplay(3, 45);
    runTimer(-10, 500);
  */
  error();
}
void setSegDisplay(int minutes, int seconds) {

  //makes the input time formatted for display (add two 0 spaces to left digits)
  int displayValue = minutes * 100 + seconds;
  /*Serial.print("     Value to Display:");
    Serial.print("'");
    Serial.print(displayValue);
    Serial.println("'");
  */

  clockDisplay.print(displayValue, DEC);

  // Add zero padding when in 24 hour mode and it's midnight.
  // In this case the print function above won't have leading 0's
  // which can look confusing.  Go in and explicitly add these zeros.
  if (minutes < 10) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(0, 0);
    if (minutes == 0) {
      clockDisplay.writeDigitNum(1, 0);
      // Also pad when the 10's minute is 0 and should be padded.
      if (seconds < 10) {
        clockDisplay.writeDigitNum(3, 0);
      }
    }
  }
  clockDisplay.drawColon(colon);
  clockDisplay.writeDisplay();
  currentMinutes = minutes;
  currentSeconds = seconds;
}

void runTimer(int seconds_to_run, int clock_speed) {
  int time_track = seconds_to_run;
  while (time_track != 0) {
    //countdown timer that reduces to 0
    // -1 minutes if for keeping track of minutes after both minutes and secs reach 0
    if (currentMinutes > -1) {
      currentSeconds -= seconds_to_run / abs(seconds_to_run);
      if (currentSeconds < 0) {
        currentSeconds = 59;
        currentMinutes -= 1;
      }
      if (currentSeconds > 59) {
        currentSeconds  = 0;
        currentMinutes += 1;
      }
    }
    setSegDisplay(currentMinutes, currentSeconds);
    delay(clock_speed);
    time_track = abs(time_track) - 1;
    Serial.print("time left: ");
    Serial.println(time_track);
  }
  timerEnd();
  blank();
  return;
}

void timerEnd() {
  Serial.print("TIMER END!");
  clockDisplay.print(0x0000, HEX);
  haywire(15);
  return;
}

void haywire(int duration) {
  int maxtime = duration;
  while (duration >= 0) {
    Serial.print("Duration: ");
    Serial.println(duration);
    randomSeed(analogRead(0));
    int randomDigits = random(10000);
    clockDisplay.print(randomDigits, DEC);
    if (randomDigits < 1000) {
      clockDisplay.writeDigitNum(0, 0);
      if ( randomDigits < 100) {
        clockDisplay.writeDigitNum(1, 0);
      }
      if (randomDigits < 10) {
        clockDisplay.writeDigitNum(2, 0);
      }
    }
    clockDisplay.drawColon(colon);
    clockDisplay.writeDisplay();
    delay(random(200, 750));
    if (duration < maxtime / 2) {
      blank();
      delay(random(25, 80));
      if (random(10) > 4) {
        //clockDisplay.print(0xERR, HEX);
      }
    }
    duration -= 1;
  }
}

void blank() {
  clockDisplay.print(10000, DEC);
  clockDisplay.writeDisplay();
}
void error() {
  //displays Err. on clock
  
  // writeDigitRaw() bitmask lelgend [B, decimal,mid,top_left,bot_left,bottom, bot_right, top_right, top]
  // write 'Err.'
  clockDisplay.writeDigitRaw(0, B01111001);
  clockDisplay.writeDigitRaw(1, B01010000);
  clockDisplay.writeDigitRaw(3, B11010000);
  clockDisplay.writeDigitRaw(4, B00000000);
  clockDisplay.writeDisplay();
}

void timeConvert(int total_sec) {
  Serial.println("Total Seconds Input:");
  Serial.println(total_sec);
  serial_minutes = total_sec / 60;
  serial_seconds = total_sec % 60;
  Serial.print("Serial Minutes: ");
  Serial.print(serial_minutes);
  Serial.print("       Serial Seconds: ");
  Serial.println(serial_seconds);
}

void serialEvent() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    //add it to input stirng
    inputString += inChar;
    if ( inChar == '\n') {
      Serial.print("Serial Input Complete, Input String is: ");
      Serial.println(inputString);
      stringComplete = true;
    }
  }
}

void inputParse(String parseString) {

  Serial.println("Parsing input...");
  ///****************************   TIMER INPUT
  if ( parseString[0] == 'T') {
    Serial.println("Input is Timer Tnstructions!");
    int clock_set_test = parseString.substring(1).toInt();
    timeConvert(clock_set_test);
    setSegDisplay(serial_minutes, serial_seconds);
  }

  //*****************************   LIGHT INPUT
  else if (parseString[0] == 'L') {
    Serial.print("INPUT IS LIGHT DATA!");
    Serial.println("...But you haven't done anything with this yet!");
  }

  //*****************************   DOOR INPUT
  else if (parseString[0] == 'D') {
    Serial.println("INPUT IS DOOR DATA!");
    if (parseString.substring(1).toInt() == 0) {
      door_open = false;
    }
    else if (parseString.substring(1).toInt() == 1) {
      door_open = true;
    }
    if (door_open) {
      Serial.print("Doors Unlocked!");
    }
    else {
      Serial.print("Doors Locked!");
    }
  }
}

void reset_values() {
  Serial.println("Clearing Input String");
  inputString = "";
  stringComplete = false;
  Serial.print("\n\n\n");
}

