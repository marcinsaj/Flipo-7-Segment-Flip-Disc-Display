// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

#define NUM_BUTTONS 7
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {10, 9, 8, 7, 4, 5, 6};
bool buttonState[7] = {0, 0, 0, 0, 0, 0, 0};

Bounce * buttons = new Bounce[NUM_BUTTONS];

#define DIN_PIN     A1
#define EN_PIN      A0
#define CLK_PIN     A2
#define EN_VF       A3
#define FB_VF       A7

uint16_t resetDot[]=
{
  0b0000000100001000,
  0b0000000100100000,
  0b0000000100000100,
  0b0000001100000000,
  0b0000000110000000,
  0b0100000100000000,
  0b0001000100000000
}; 

uint16_t setDot[]=
{
  0b0000100000000001,
  0b0000100000010000,
  0b0000100000000010,
  0b0000100001000000,
  0b0000110000000000,
  0b1000100000000000,
  0b0010100000000000
};

bool randomArray[20][7] =
{
  {1, 1, 1, 1, 1, 1, 1},
  {0, 1, 0, 1, 0, 1, 0},
  {1, 0, 1, 0, 1, 0, 1},
  {1, 1, 0, 1, 0, 1, 1},
  {0, 1, 1, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1},
  {0, 0, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 1},
  {0, 1, 0, 0, 0, 1, 1},
  {1, 0, 0, 1, 0, 0, 1},
  {0, 1, 0, 0, 1, 0, 1},
  {0, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 0, 0, 1, 1},
  {0, 0, 0, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 0, 0},
  {1, 1, 1, 0, 1, 1, 1},
  {1, 0, 1, 1, 1, 0, 0},
  {1, 0, 0, 0, 1, 1, 1}
};

uint8_t newPattern = 0; 
uint8_t oldPattern = 0; 
uint16_t dotNumber;

void setup() {
 // Serial.begin(9600);
 // delay(3000);

  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);
  
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW); 

  pinMode(EN_VF, OUTPUT);
  digitalWrite(EN_VF, LOW); 

  ShiftOutData(0);

  for (int i = 0; i < NUM_BUTTONS; i++) 
  {
    buttons[i].attach( BUTTON_PINS[i], INPUT_PULLUP);       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
    buttonState[i] = 0;
  }

  CurrentPulse();
  ResetAllDiscs();
}

// the loop function runs over and over again forever
void loop() 
{
  for (int i = 0; i < NUM_BUTTONS; i++)  
  {
    // Update the Bounce instance :
    buttons[i].update();
    // If it fell, flag the need to toggle the LED
    if (buttons[i].fell()) 
    {
      CurrentPulse();
      buttonState[i] = !buttonState[i];

      if(buttonState[i] == 1) ShiftOutData(setDot[i]);
      else ShiftOutData(resetDot[i]);

      delay(2);
      ShiftOutData(0);
    }
  }
SetRandomDiscs();
delay(2000);

}
void SetRandomDiscs(void)
{
  do
  {
    newPattern = random(0, 19);
  } while(newPattern == oldPattern);

  for(int i = 0; i < 7; i++)
  {
        CurrentPulse();
    if(randomArray[newPattern][i] == 1) ShiftOutData(setDot[i]);
    else ShiftOutData(resetDot[i]); 
        delay(2);
    ShiftOutData(0); 
  }
  oldPattern = newPattern;
}

void ResetAllDiscs(void)
{
  for(int i = 0; i < 7; i++)
  {
    CurrentPulse();
    ShiftOutData(resetDot[i]);
    delay(2);
    ShiftOutData(0);
  }
}

void CurrentPulse()
{
  int value = 0;
  digitalWrite(EN_VF, HIGH);

  do
  {
    value = analogRead(FB_VF);
    //delayMicroseconds(10);
    //Serial.println(value);
  } while (value < 600);
 
  digitalWrite(EN_VF, LOW);
}

void ShiftOutData(uint16_t dotNumber)
{ 
  digitalWrite(EN_PIN, LOW);   
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, highByte(dotNumber));
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, lowByte(dotNumber));
  digitalWrite(EN_PIN, HIGH); 
}
