#import "WProgram.h"       /* Needed for access to Serial library */

extern void display_clear(void);
void keypad_isr(void);

const int keypad_D0 = 5;
const int keypad_D1 = 6;
const int keypad_D2 = 7;
const int keypad_D3 = 8;
  
// Set the Serial Baud Rate
// Attach an interrupt handler for pin d3 
// Call keypad_isr when the interreupt is triggered (LOW)
void setup_keypad()
{
  //d5 -> LSB
  //d6
  //d7
  //d8 -> MSB
  
  Serial.print("Setup Keypad");
  pinMode(keypad_D0, INPUT);
  pinMode(keypad_D1, INPUT);
  pinMode(keypad_D2, INPUT);
  pinMode(keypad_D3, INPUT);
  attachInterrupt(1, keypad_isr, FALLING);
  Serial.print("Setup Complete");
}

void keypad_isr()
{
 uint8_t val = digitalRead(keypad_D0) | (digitalRead(keypad_D1) << 1) | (digitalRead(keypad_D2) << 2) | (digitalRead(keypad_D3) << 3); 
 // Assumption: data is ready on keypad
 // Read whatever is needed
 // Call display_process if it was a * or #
  
 // Choice 0 - *
 if (val == 0x0C) 
 {
   Serial.print("Choice 0 "); 
 }
 else if (val == 0x0F)  // Choice 1 - D
 { 
    Serial.print("Choice 1 ");
 }  
 else // Else, for now, just CLEAR and then print to LCD panel
 {
   display_clear();
   Serial.print(val, HEX);
 }
}



