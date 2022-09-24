
#include <Encoder.h>
Encoder encoderReadout(2,7);

uint16_t motorPosition = 0;

uint8_t ampA[] = {0,53,104,150,190,221,243,254,254,243,221,190,150,104,53,0,0,53,104,150,190,221,243,254,254,243,221,190,150,104,53,0};
uint8_t ampB[] = {221,243,254,254,243,221,190,150,104,53,0,0,53,104,150,190,221,243,254,254,243,221,190,150,104,53,0,0,53,104,150,190};
uint8_t ampC[] = {221,190,150,104,53,0,0,53,104,150,190,221,243,254,254,243,221,190,150,104,53,0,0,53,104,150,190,221,243,254,254,243};
bool hiloA[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Port 4
bool hiloB[] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0}; // Port 5
bool hiloC[] = {1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1}; // Port 6

uint8_t count = 0;
uint8_t throttle = 0;

void setup() {
  Serial.begin(115200);
  pinMode(9,OUTPUT);   // 8 kHz B
  pinMode(10,OUTPUT);  // 8 kHz C
  pinMode(3,OUTPUT);   // 8 kHz A
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(A0,INPUT);

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A |= (1<<WGM10) | (1<<COM1A1) | (1<<COM1B1);   
  TCCR1B |= (1<<CS11) | (1<<WGM12); 
  
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= (1<<WGM20) | (1<<WGM21) | (1<<COM2B1); 
  TCCR2B |= (1<<CS21) | (1<<WGM22); 
  OCR2A = 255; // Do not change - reset value for Timer2 - Don't use pin 11?

}

void loop() {
  long newMotorPosition;
  newMotorPosition = encoderReadout.read();
  if(newMotorPosition != motorPosition) {
    if(newMotorPosition > 2047) {
      newMotorPosition = newMotorPosition - 2048;
      encoderReadout.write(newMotorPosition);
    }
    if(newMotorPosition < 0) {
      newMotorPosition = newMotorPosition + 2048;
      encoderReadout.write(newMotorPosition);
    }
    motorPosition = newMotorPosition;  
    uint8_t motorcount = B00011111 & (motorPosition >> 3);  // Constrain to 0-31
    count = motorcount;
    // Need to include a verification check to reset encoderReadout to 0 when Z is triggered.
    // In case we lose track of counts.  How likely?
    // Maybe we should use the interrupt to ensure this happens? instead of counting...
  }
  
  throttle = analogRead(A0);
  OCR2B = ampA[count]*(throttle/255);  // pin 3
  OCR1A = ampB[count]*(throttle/255);  // pin 9
  OCR1B = ampC[count]*(throttle/255);  // pin 10

  // bits 4,5,6 are for ports 4,5,6
  PORTD = PORTD | (B00000000 | ((hiloA[count] << 4) | (hiloB[count] << 5) | (hiloC[count] << 6)));  //turn pin on
  PORTD = PORTD & (B11111111 ^ ((!(hiloA[count]) << 4) | (!(hiloB[count]) << 5) | (!(hiloC[count]) << 6)));  //turn pin off
  
  delay(1);
}
