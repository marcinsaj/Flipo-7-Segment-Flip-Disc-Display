#include <SPI.h>
// Declaration of the controller control inputs
#define DIN_PIN     11    // 
#define EN_PIN      A7    // 
#define CLK_PIN     13    // 

const int slaveSelectPin = 7;



// Declaration of the PCPS module control input/output
#define CH_VF       A2    // Turn ON/OFF charging PCPS module
#define PL_VF       A3    // Measurement output for checking if the current pulse is ready

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

bool digitArray[10][23] = 
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

uint8_t resetDiscDot[2] =
{
  0b01000010,
  0b11000000  
};

uint8_t setDiscDot[2] =
{
  0b00001100,
  0b00001001   
};
uint8_t dig[4];
uint8_t digit[6] = {0, 0, 0, 0,0,0};

int j =0 , k = 0, m = 0;

void setup() 
{
  SPI.begin();
  Serial.begin(9600);

delay(3000);
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

  ClearDisplay();
  PrepareCurrentPulse();      // Prepare current pulse
      FlipDiscDot1(1,1);
    FlipDiscDot2(1,1);
}

void loop()
{ 

for(int i = 0; i < 10; i++)
{
  int a = i + 1;
  int b = i + 2;
  int c = i + 3;
  int d = i + 4;
  int e = i + 5;
  
  if(a > 9) a = 0;
  if(b > 9) b = 0;
  if(c > 9) c = 0;
  if(d > 9) d = 0;
  if(e > 9) e = 0;
FlipDiscDisplay(i,a,b,c,d,e);
delay(1000);  
}
    


}

void FlipDiscDot1(bool dot1, bool dot2)
{
  bool dotArray[2];
  dotArray[0] = dot1;
  dotArray[1] = dot2;

  for(int dot = 0; dot < 2; dot++)
  {  
    digitalWrite(EN_PIN, LOW);
    for(int i = 0; i < 6; i++) SPI.transfer(0);
    if(dotArray[dot] == 1) SPI.transfer(setDiscDot[dot]);
    if(dotArray[dot] == 0) SPI.transfer(resetDiscDot[dot]);
    for(int i = 0; i < 12; i++) SPI.transfer(0);
    SPI.transfer(0);
    digitalWrite(EN_PIN, HIGH); 
    CurrentPulse();  
  }  
}

void FlipDiscDot2(bool dot1, bool dot2)
{
  bool dotArray[2];
  dotArray[0] = dot1;
  dotArray[1] = dot2;

  for(int dot = 0; dot < 2; dot++)
  {  
    digitalWrite(EN_PIN, LOW);
    SPI.transfer(0);
    for(int i = 0; i < 12; i++) SPI.transfer(0);
    if(dotArray[dot] == 1) SPI.transfer(setDiscDot[dot]);
    if(dotArray[dot] == 0) SPI.transfer(resetDiscDot[dot]);
    for(int i = 0; i < 6; i++) SPI.transfer(0);
    digitalWrite(EN_PIN, HIGH); 
    CurrentPulse();  
  }  
}

void FlipDiscDisplay(uint8_t digit6, uint8_t digit5, uint8_t digit4, uint8_t digit3, uint8_t digit2, uint8_t digit1)
{
  PrepareCurrentPulse();
  bool discStatus = 0;

  digit[0] = digit1;
  digit[1] = digit2;
  digit[2] = digit3;
  digit[3] = digit4;
  digit[4] = digit5;
  digit[5] = digit6;

  for(int displayNumber = 0; displayNumber < 6; displayNumber++)
  {
    for(int discNumber = 0; discNumber < 23; discNumber++)
    {
      digitalWrite(EN_PIN, LOW);

      for(int i = 5 - displayNumber; i > 0; i--) 
      {
        for(int j = 0; j < 3; j++) SPI.transfer(0);
      }

      if(displayNumber >= 4) 
      {
        SPI.transfer(0);
      }
      
      if(displayNumber >= 2) 
      {
        SPI.transfer(0);
      }

      discStatus = digitArray[digit[displayNumber]][discNumber];
        
      for(int byteNumber = 0; byteNumber < 3; byteNumber++)
      {
        if(discStatus == 0) SPI.transfer(setDiscArray[discNumber][byteNumber]);
        if(discStatus == 1) SPI.transfer(resetDiscArray[discNumber][byteNumber]);
      } 

      for(int i = displayNumber; i < 5; i++) 
      {
        for(int j = 0; j < 3; j++) SPI.transfer(0);
      }

      if(displayNumber < 2) 
      {
        SPI.transfer(0);
      }
      
      if(displayNumber < 4) 
      {
        SPI.transfer(0);
      }


      digitalWrite(EN_PIN, HIGH); 
      CurrentPulse();  
    } 
  }
}

// First charging - setup call
void PrepareCurrentPulse(void)
{
  digitalWrite(CH_VF, HIGH);
  delay(10);
  digitalWrite(CH_VF, LOW);
}

void CurrentPulse(void)
{ 
  digitalWrite(PL_VF, HIGH);
  delay(1);                             // Flip-disc required 1ms current pulse to flip
  digitalWrite(PL_VF, LOW);
  digitalWrite(CH_VF, HIGH);
  delayMicroseconds(500);
  digitalWrite(CH_VF, LOW);

  ClearDisplay();
}

void ClearDisplay(void)
{
  digitalWrite(EN_PIN, LOW);
  for(int i = 0; i < 20; i++) SPI.transfer(0);
  digitalWrite(EN_PIN, HIGH);
}
