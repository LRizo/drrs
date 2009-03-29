#include <EEPROM.h>
#import "display.h"
#import "keypad.h"
#import "txvr.h"
#import "config.h"
#import "WProgram.h"

//#define KEYPAD_DEBUG // For keypad debugging
//#define UNIT_TEST

//const char *messages[] = { "Hello", "Epic!", "Goal!", "Pasta", "DKCX." };
//const char ack[] = {'A', 'C', 'K', '.', ' '};

volatile boolean keypad_if;
volatile uint8_t g_configid;
extern const int txvr_ce_port;

// Memory Test
#define PACKET_RING_SIZE	3
PACKET packets[PACKET_RING_SIZE];

void setup (void)
{
  txvr_setup_ports();
  keypad_setup_ports();

  /* Begin Configure SPI */
  SPCR = 0b01010010;
  int clr = SPSR;
  clr = SPDR;
  delay (10);
  /* End Configure SPI */
  delay(5); 
  txvr_setup ();
  delay(100);
  display_setup_lcd ();
  keypad_setup();
  display_setup();
  config_setup();
  Serial.print ("Setup ack. ");
  delay(500);
  display_mainmenu();
  keypad_if = false;
  #ifdef UNIT_TEST
  //test_ram_write();
  #endif

  if (config_get_id() == 1)
   list_test_send();
}

void loop(void)
{
  display_clear();
  txvr_receive_payload();  
  queue_transmit();
  delay(6000);
  #ifdef KEYPAD_DEBUG
  //Do nothing, wait for keypad interrupt    
    keypad_service();
  #endif

  txvr_receive_payload();
}

char spi_transfer (volatile char data)
{
  SPDR = data;
  //Start the transmission
  while (!(SPSR & (1 << SPIF)))
    //Wait the end of the transmission
    {
    };
  return SPDR;
  //return the received byte
}


#ifdef UNIT_TEST
void test_ram_write()
{  
  for (int i = 0; i < PACKET_RING_SIZE; i++) {
    packets[i].msgheader = 0xBC;
    packets[i].msglen = 0xAA;
    memset(packets[i].msgpayload, 0xCC, 32);
  }
}

void test_ram_read()
{
  bool failure = false;
  for (int i = 0; i < PACKET_RING_SIZE; i++) {
    if (packets[i].msgheader != 0xBC)
      failure = true;
    if (packets[i].msglen != 0xAA)
      failure = true;
    for (int j = 0; j < 32; j++)
      if (packets[i].msgpayload[j] != 0xCC)
	failure = true;
  }
  Serial.print("TestRAM ");
  if (failure == true)
    Serial.print("FAILED");
  else
    Serial.print("PASSED");
  delay(3000);
}
#endif

