#include <Adafruit_CircuitPlayground.h>
#include <string.h>
#include <math.h>

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data

bool newData = false;
bool readingPots = false;
int delayCounter = 0;

int effect = 0;
int pot1 = 2047;
int rainbowSpeed = 15;
int numFX = 8;
int readTimer = 0;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  
  CircuitPlayground.strip.setBrightness(15);
}

void loop() {
  recvDataFromPedal();
  showNewData();
  parseData();

  readTimer++;
  if (readTimer == 1000) {
    readTimer = 0;
    if(CircuitPlayground.rightButton()){
      pot1 = min(pot1 + 500, 4095);
      displayPotLvl(pot1);
      Serial.print("pot1: ");
      Serial.println(pot1);
      readingPots = true;
    } else if (CircuitPlayground.leftButton()) {
      pot1 = max(pot1 - 500, 0);
      displayPotLvl(pot1);
      Serial.print("pot1: ");
      Serial.println(pot1);
      readingPots = true;
    } else {
      readingPots = false;
    }
  }
  
  // Check for any button presses by checking their state twice with
  // a delay inbetween.  If the first press state is different from the
  // second press state then something was pressed/released!
//  bool leftFirst = CircuitPlayground.leftButton();
//  bool rightFirst = CircuitPlayground.rightButton();
//  delay(10);
//
//  // Now check for buttons that were released.
//  bool leftSecond = CircuitPlayground.leftButton();
//  bool rightSecond = CircuitPlayground.rightButton();
//
//  if(leftFirst && !leftSecond && effect != 0){
//    effect -= 1;
//    Serial.print("effect: ");
//    Serial.println(effect);
//  }
//  if(rightFirst && !rightSecond && effect != numFX-1){
//    effect += 1;
//    Serial.print("effect: ");
//    Serial.println(effect);
//  }
  if(!readingPots){
    if(effect != numFX-1){
      for(int i=0; i<10; ++i) {
        CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.colorWheel((effect + 1) * 256 / (numFX)));
      }
    } else {
      // Make an offset based on the current millisecond count scaled by the current speed.
      uint32_t offset = millis() / rainbowSpeed;
      // Loop through each pixel and set it to an incremental color wheel value.
      for(int i=0; i<10; ++i) {
        CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.colorWheel(((i * 256 / 10) + offset) & 255));
      }
    }
  }
  // Show all the pixels.
  CircuitPlayground.strip.show();
}

void displayPotLvl(int lvl){
  CircuitPlayground.clearPixels();
  int numLights = (((float)lvl/4095.0) * 10) - 1;
  for(int i = numLights; i >= 0; --i) {
    CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.colorWheel(0));
  }
}

void recvDataFromPedal() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    //newData = false;
  }
}

void parseData() {
  if (newData){
    // split the data into its parts
    char * strtokIndx; // this is used by strtok() as an index
    
    strtokIndx = strtok(receivedChars,",");
    effect = atoi(strtokIndx);
    
  //  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  //  integerFromPC = atoi(strtokIndx);     // convert this part to an integer
    newData = false;
  }
}
