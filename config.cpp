#include <EEPROM.h>
#import "WProgram.h"
#import "config.h"
#import "display.h"
#import "txvr.h"
#import <stdio.h>

boolean inSetID;
boolean inSetBright;
boolean inSetFreq;

char id_list[] = {'A','B','C','D'};
uint8_t oldID;
uint8_t oldFreq;

void config_setup(void)
{
  display_clear();
  delay(50);
  config_set_bright(157);
  //Serial.print("Config Setup Complete");
}

void config_set_id_callback(void)
{
  config_next_id();
}

void config_set_freq_callback(void)
{
  config_next_freq();
}

void config_set_bright_callback(void)
{
  inSetBright = true;
  config_next_bright();
}

void config_set_id(uint8_t newId)
{
  uint8_t currId = config_get_id();
  if (newId != currId)
    EEPROM.write(EEPROM_CONFIG_ADDR, newId);
}

uint8_t config_get_id(void)
{
  return EEPROM.read(EEPROM_CONFIG_ADDR);
}

// Return the ASCII/char version of the id, assumes no ID is bigger
// than id_list's length.
char config_get_text_id(void) {
  return id_list[config_get_id() & 0x3];
}

void config_next_freq(void)
{

  const uint8_t low = 0;
  const uint8_t med = 37;
  const uint8_t high = 50;
  char msg[17];
  uint8_t freq = config_get_freq();
  if(!inSetFreq)
  {
    oldFreq = config_get_freq();
    inSetFreq = true;
    memset(currDisplay, ' ', 32);
    int len = snprintf(msg, 17, "Current:%s", (freq == med) ? "2.437GHz" : ((freq > med) ? "2.450GHz" : "2.400GHz"));
    memcpy(currDisplay, msg, len);
    sprintf(msg, "<BACK  NEW FREQ>");
    memcpy(currDisplay + 16, msg, 16);
  }
  else
  { 
    uint8_t newFreq;
    if(freq > med)
    {
      freq = low;
      newFreq = low;
    }
    else if (freq < med)
    {
      freq = med;
      newFreq = med;
    }
    else
    {
      freq = high;
      newFreq = high;
    }
      
    memset(currDisplay, ' ', 32);
    int len = snprintf(msg, 17, "Current:%s", (oldFreq == med) ? "2.437GHz" : ((oldFreq > med) ? "2.450GHz" : "2.400GHz"));
    memcpy(currDisplay, msg, len);
    len = snprintf(msg, 17, "NewFreq:%s", (newFreq == med) ? "2.437GHz" : ((newFreq > med) ? "2.450GHz" : "2.400GHz"));
    memcpy(currDisplay + 16, msg, len);
    config_set_freq(newFreq);
  }
}

void config_next_id(void)
{
  if(!inSetID)
  {
    inSetID = true;
    char msg[17];
    memset(currDisplay, ' ', 32);
    int len = snprintf(msg, 17, "Current ID: %c", id_list[config_get_id()]);
    oldID = config_get_id();
    memcpy(currDisplay, msg, len);
    sprintf(msg, "<BACK    NEW ID>");
    memcpy(currDisplay + 16, msg, 16);
  }  
  else
  {
    uint8_t newID = (config_get_id() + 1) % 4;
    char msg[17];
    memset(currDisplay, ' ', 32);
    int len = snprintf(msg, 17, "Current ID: %c", id_list[oldID]);
    memcpy(currDisplay, msg, len);
    len = snprintf(msg, 17, "New ID: %c", id_list[newID]);
    memcpy(currDisplay + 16, msg, len);
    config_set_id(newID);
  }
}

void config_next_bright(void)
{
  char msg[17];
  const uint8_t low = 140;
  const uint8_t med = 147;
  const uint8_t high = 157;
  memset(currDisplay, ' ', 32);
  uint8_t bright = config_get_bright();
  Serial.print("bright:");
  Serial.print(bright, HEX);
  delay(2000);
  int len = snprintf(msg, 17, "Current Bright: %s", (bright == med) ? "Med" : ((bright > med) ? "High" : "Low")); 
  memcpy(currDisplay, msg, len);  
  if(bright > med) 
    config_set_bright(high);
  else if(bright < med)
    config_set_bright(high);
  else
    config_set_bright(high);  
}


uint8_t config_get_freq(void)
{
  return EEPROM.read(EEPROM_FREQ_ADDR);
}

void config_set_freq(uint8_t freqOffset)
{
  noInterrupts();
  uint8_t currFreq = config_get_freq();
  if (freqOffset != currFreq)
    EEPROM.write(EEPROM_FREQ_ADDR, freqOffset);

  txvr_set_frequency(freqOffset);
  interrupts();
}

uint8_t config_get_bright(void)
{
  return EEPROM.read(EEPROM_BRIGHT_ADDR);
}

// Range is 128 (Off) to 157 (Full)
void config_set_bright(uint8_t brightLevel)
{
  if (brightLevel < 128)
    brightLevel = 128;
  else if (brightLevel > 157)
    brightLevel = 157;

  uint8_t currLevel = config_get_bright();
  if (brightLevel != currLevel)
    EEPROM.write(EEPROM_BRIGHT_ADDR, brightLevel);

  display_set_bright(brightLevel);
}
