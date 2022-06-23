#include <SPI.h>
// Declaration of the controller control inputs
#define DIN_PIN     11    // 
#define EN_PIN      10    // 
#define CLK_PIN     13    // 

const int slaveSelectPin = 7;




// Declaration of the PCPS module control input/output
#define CH_VF       9    // Turn ON/OFF charging PCPS module
#define PL_VF       8    // Measurement output for checking if the current pulse is ready

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

//   0  1  2  3  4
//  19           5
//  18           6
//  17 20 21 22  7
//  16           8
//  15           9
//  14 13 12 11 10

boolean digitArray[10][23] = 
{
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},  //  0
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  1
  {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},  //  2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1},  //  3
  {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},  //  4
  {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},  //  5
  {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  //  6
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  7
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  //  8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1}   //  9
};

uint8_t setDiscDot[2] =
{
  0b01000010,
  0b11000000  
};

uint8_t resetDiscDot[2] =
{
  0b00001100,
  0b00001001   
};

void setup() 
{
  SPI.begin();

  //pinMode(DIN_PIN, OUTPUT);
  //digitalWrite(DIN_PIN, LOW);
 
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
 
  //pinMode(CLK_PIN, OUTPUT);
  //digitalWrite(CLK_PIN, LOW);

  pinMode(CH_VF, OUTPUT);
  digitalWrite(CH_VF, LOW); 

  pinMode(PL_VF, OUTPUT);
  digitalWrite(PL_VF, LOW); 


  ShiftOutDataX(0);            // Clear Flipo #3 Controller



  //PrepareCurrentPulse();      // Prepare current pulse


}

void loop()
{
for(int i = 0; i < 10; i++)
{
  for(int j = 0; j < 23; j++)
  {
    if(digitArray[i][j] == 1) SetDisc(j);
    else ResetDisc(j);
  }
  delay(1000);







}
}

// First charging - setup call
void PrepareCurrentPulse(void)
{
  CurrentPulse();
}

void CurrentPulse()
{
  digitalWrite(CH_VF, HIGH);            // Turn ON PCPS module- charging begin

delayMicroseconds(500);
 
  digitalWrite(CH_VF, LOW);             // Turn ON PCPS module- charging complete 
    digitalWrite(PL_VF, HIGH);
  delay(1);                             // Flip-disc required 1ms current pulse to flip
digitalWrite(PL_VF, LOW);
delayMicroseconds(500);
}

void SetDisc(uint8_t discNumber)
{
                     // Prepare current pulse - charging begin
  ShiftOutData(discNumber);    // Turn on flip-disc controller corrsponding outputs
  CurrentPulse();  
  ShiftOutDataX(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}

void ResetDisc(uint8_t discNumber)
{
  ShiftOutData2(discNumber);    // Turn on flip-disc controller corrsponding outputs
  CurrentPulse();  

  ShiftOutDataX(0);                      // Absolutely required!
                                        // This function here turns off the current pulse 
                                        // and cleans the controller outputs
}


void ShiftOutDataX(uint8_t discNumber)
{
  digitalWrite(EN_PIN, LOW);            // Transfer data begin
  for(int i = 0; i <= 6; i++)
  {
    SPI.transfer(0);
  }
  digitalWrite(EN_PIN, HIGH);           // Transfer data complete
}

void ShiftOutData(uint8_t discNumber)
{
  digitalWrite(EN_PIN, LOW);            // Transfer data begin

  if(discNumber < 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(resetDiscArray[discNumber][i]);   // Send data to the controller
    }
  }
     
  if(discNumber >= 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(resetDiscArray[discNumber - 23][i]);   // Send data to the controller
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }
  }

  digitalWrite(EN_PIN, HIGH);           // Transfer data complete
}

void ShiftOutData2(uint8_t discNumber)
{
  digitalWrite(EN_PIN, LOW);            // Transfer data begin

  if(discNumber < 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(setDiscArray[discNumber][i]);   // Send data to the controller
    }
  }
     
  if(discNumber >= 23)
  {
    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(setDiscArray[discNumber - 23][i]);   // Send data to the controller
    }

    for(int i = 0; i < 3; i++)
    {
      SPI.transfer(0);
    }
  }

  digitalWrite(EN_PIN, HIGH);           // Transfer data complete
}
