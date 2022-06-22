#include <SPI.h>
// Declaration of the controller control inputs
//#define DIN_PIN     9    // 
//#define CLK_PIN     11    // 
//#define EN_PIN      10    // 


// Declaration of the PCPS module control input/output
#define EN_VF       7    // Turn ON/OFF charging PCPS module
#define FB_VF       6    // Measurement output for checking if the current pulse is ready

uint8_t *discModePointer;
uint16_t value = 0;

// Bit notation for Flipo#3 controller - set flip-discs
// Always active only two bits corresponding to control outputs 
// Check controller documentation and schematic

uint8_t setDiscArray[23][3] =
{
  {0b00000010, 0b00000010, 0b00000000},  // 0
  {0b00000000, 0b00010010, 0b00000000},  // 1
  {0b01000000, 0b00010000, 0b00000000},  // 2
  {0b00001000, 0b00010000, 0b00000000},  // 3
  {0b01000010, 0b00000000, 0b00000000},  // 4
  {0b01000000, 0b00000100, 0b00000000},  // 5
  {0b01000000, 0b01000000, 0b00000000},  // 6
  {0b01000000, 0b00000000, 0b01000000},  // 7
  {0b01000000, 0b00000000, 0b00000100},  // 8
  {0b01000000, 0b00000000, 0b00000010},  // 9
  {0b11000000, 0b00000000, 0b00000000},  // 10
  {0b00001000, 0b00000000, 0b00000100},  // 11
  {0b00001000, 0b00000000, 0b00000010},  // 12
  {0b10001000, 0b00000000, 0b00000000},  // 13
  {0b10000000, 0b00000010, 0b00000000},  // 14
  {0b00000000, 0b00000010, 0b00000010},  // 15
  {0b00000000, 0b00000010, 0b00000100},  // 16
  {0b00000000, 0b00000010, 0b01000000},  // 17
  {0b00000000, 0b01000010, 0b00000000},  // 18
  {0b00000000, 0b00000110, 0b00000000},  // 19
  {0b00001000, 0b01000000, 0b00000000},  // 20
  {0b00001000, 0b00000100, 0b00000000},  // 21
  {0b00001000, 0b00000000, 0b01000000}   // 22
};

uint8_t resetDiscArray[23][3] =
{
  {0b00000100, 0b00000000, 0b10000000},  // 0
  {0b00000000, 0b00100000, 0b10000000},  // 1
  {0b00100000, 0b00100000, 0b00000000},  // 2
  {0b00010000, 0b00100000, 0b00000000},  // 3
  {0b00100100, 0b00000000, 0b00000000},  // 4
  {0b00100000, 0b00001000, 0b00000000},  // 5
  {0b00100000, 0b10000000, 0b00000000},  // 6
  {0b00100000, 0b00000000, 0b00100000},  // 7
  {0b00100000, 0b00000000, 0b00000001},  // 8
  {0b00100000, 0b00000001, 0b00000000},  // 9
  {0b00100001, 0b00000000, 0b00000000},  // 10
  {0b00010000, 0b00000000, 0b00000001},  // 11
  {0b00010000, 0b00000001, 0b00000000},  // 12
  {0b00010001, 0b00000000, 0b00000000},  // 13
  {0b00000001, 0b00000000, 0b10000000},  // 14
  {0b00000000, 0b00000001, 0b10000000},  // 15
  {0b00000000, 0b00000000, 0b10000001},  // 16
  {0b00000000, 0b00000000, 0b10100000},  // 17
  {0b00000000, 0b10000000, 0b10000000},  // 18
  {0b00000000, 0b00001000, 0b10000000},  // 19
  {0b00010000, 0b10000000, 0b00000000},  // 20
  {0b00010000, 0b00001000, 0b00000000},  // 21
  {0b00010000, 0b00000000, 0b00100000}   // 22
};





uint8_t discNumber11 = 0b00100100;

uint8_t discNumber1 = 0b00010000;
uint8_t discNumber2 = 0b00000000;
uint8_t discNumber3 = 0b00100000;


const int slaveSelectPin = 10;

void setup() 
{
 pinMode(slaveSelectPin, OUTPUT);
SPI.begin();

  pinMode(EN_VF, OUTPUT);
  digitalWrite(EN_VF, LOW); 

  pinMode(FB_VF, OUTPUT);
  digitalWrite(FB_VF, LOW); 

  ShiftOutDataX(0);            // Clear Flipo #3 Controller

//Serial.begin(115200);
//delay(3000);

  PrepareCurrentPulse();      // Prepare current pulse


}

void loop()
{
  for(int i = 0; i < 46; i++)
  {
    SetDisc(i);
    delay(1);
  }
  //delay(1000);

  for(int i = 0; i < 46; i++)
  {
    ResetDisc(i);
    delay(1);
  }
  //delay(1000);
}

// First charging - setup call
void PrepareCurrentPulse(void)
{
  CurrentPulse();
}

void CurrentPulse()
{
  digitalWrite(EN_VF, HIGH);            // Turn ON PCPS module- charging begin

delay(1);
 
  digitalWrite(EN_VF, LOW);             // Turn ON PCPS module- charging complete 
}

void SetDisc(uint8_t discNumber)
{
  CurrentPulse();                       // Prepare current pulse - charging begin
  ShiftOutData(discNumber);    // Turn on flip-disc controller corrsponding outputs
  digitalWrite(FB_VF, HIGH);
  delayMicroseconds(1000);                             // Flip-disc required 1ms current pulse to flip
digitalWrite(FB_VF, LOW);
  ShiftOutDataX(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}

void ResetDisc(uint8_t discNumber)
{
  CurrentPulse();                       // Prepare current pulse - charging begin
  ShiftOutData2(discNumber);    // Turn on flip-disc controller corrsponding outputs
  digitalWrite(FB_VF, HIGH);
  delayMicroseconds(1000);                             // Flip-disc required 1ms current pulse to flip
digitalWrite(FB_VF, LOW);                            // Flip-disc required 1ms current pulse to flip

  ShiftOutDataX(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}



void ShiftOutDataX(uint8_t discNumber)
{
  digitalWrite(slaveSelectPin, LOW);            // Transfer data begin
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(0);

  digitalWrite(slaveSelectPin, HIGH);           // Transfer data complete
}

void ShiftOutData(uint8_t discNumber)
{
  digitalWrite(slaveSelectPin, LOW);            // Transfer data begin

  if(discNumber < 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(resetDiscArray[discNumber][i]);
      //shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, resetDiscArray[discNumber][i]);   // Send data to the controller
    }
  }
     
  if(discNumber >= 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(resetDiscArray[discNumber - 23][i]);
      //shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, resetDiscArray[discNumber - 23][i]);   // Send data to the controller
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }
  }

  digitalWrite(slaveSelectPin, HIGH);          // Transfer data complete
}

void ShiftOutData2(uint8_t discNumber)
{
  digitalWrite(slaveSelectPin, LOW);             // Transfer data begin

  if(discNumber < 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(setDiscArray[discNumber][i]);
    }
  }
     
  if(discNumber >= 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(setDiscArray[discNumber - 23][i]);
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }
  }

  digitalWrite(slaveSelectPin, HIGH);          // Transfer data complete
}
