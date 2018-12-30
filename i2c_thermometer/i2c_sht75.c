   /* Reads SHT75 Thermometer and Prints Value over Serial */

// ------- Preamble -------- //

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
//#include <avr/power.h>

#include "pinDefines.h"
#include "USART.h"
#include "TEMP_HUMI.h"

// -------- Definesc 
#define DATA_REG_OUT   DDRC |= _BV(PC4); 
#define DATA_REG_IN   DDRC &=~ _BV(PC4); 

#define SET_SCK      PORTC |= _BV(PC5); 
#define CLEAR_SCK      PORTC &=~ _BV(PC5); 

#define SET_DATA      PORTC |= _BV(PC4); 
#define CLEAR_DATA      PORTC &=~ _BV(PC4); 

#define noACK 0 
#define ACK 1 

//SHT7x commands 
#define STATUS_REG_W 0x06 //000 0011 0 
#define STATUS_REG_R 0x07 //000 0011 1 
#define MEASURE_TEMP 0x03 //000 0001 1 
#define MEASURE_HUMI 0x05 //000 0010 1 
#define RESET 0x1e        //000 1111 0 

// SHTxx constants
#define C1  -4.0 
#define C2  0.0405 
#define C3  -0.0000028 

#define D1  -40.1 
#define D2  0.01 

#define T1  0.01 
#define T2  0.00008 


// -------- Functions --------- //

/****************************************************************************/ 
// START TRANSMISSION
//       _____         _________ 
// DATA:      |_______| 
//           ___     ___ 
// SCK : ___|   |___|   |______ 
/****************************************************************************/ 

void s_transstart(void) 
{ 
DATA_REG_OUT; //DATA as output 
_delay_us(10);
SET_DATA; 
CLEAR_SCK; 
_delay_us(10); 
SET_SCK; 
_delay_us(10); 
CLEAR_DATA; 
_delay_us(10); 
CLEAR_SCK; 
_delay_us(10); 
SET_SCK; 
_delay_us(10); 
SET_DATA; 
_delay_us(10); 
CLEAR_SCK; 
_delay_us(10); 
} 

/****************************************************************************/ 
// WRITE BYTE
/****************************************************************************/ 

uint8_t s_write_byte(uint8_t value) 
{ 
uint8_t i, error=0; 

DATA_REG_OUT; //DATA as output 
CLEAR_SCK; 
  
   for (i=0x80;i>0; i/=2) //mask 
   { 
      if (( i & value )) 
      { 
        SET_DATA; 
      } 
      else 
      { 
        CLEAR_DATA; 
      }
      
      SET_SCK; 
      _delay_us(10);
      CLEAR_SCK; 
   } 
   DATA_REG_IN; //DATA as input 
   _delay_us(10);
   SET_SCK; 

   //Check ack 
      if(bit_is_set(PINC,4)) 
      { 
      error=1; 
      } 
         if(bit_is_clear(PINC,4)) 
         { 
         error=0; 
         } 

   _delay_us(10);
   CLEAR_SCK; 
   return error; //error=1 if no ack from sht
   } 

/****************************************************************************/ 
// READ BYTE
/****************************************************************************/ 

uint8_t s_read_byte(uint8_t ack) 

{ 
uint8_t i, val=0; 

DATA_REG_IN; //DATA as input
   for (i=0x80;i>0;i/=2) //mask
   { 
   SET_SCK; 

      if (bit_is_set(PINC,4)) 
      { 
      val=(val | i); //read bit
      } 

   CLEAR_SCK; 
   } 

DATA_REG_OUT; //DATA as output

if(ack==1) 
{ 
CLEAR_DATA; 
} 
if(ack==0) 
{ 
SET_DATA; 
} 

SET_SCK; 
_delay_us(10);
CLEAR_SCK; 
DATA_REG_IN; //DATA as input
return val; 
} 

//---------------------------------------------------------------------------------- 
// Communication reset: DATA=1 i 9 *SCK 
//       _____________________________________________________         ________ 
// DATA:                                                      |_______| 
//          _    _    _    _    _    _    _    _    _        ___     ___ 
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______ 
//---------------------------------------------------------------------------------- 

void s_connectionreset(void) 
{  
  uint8_t i; 
  DATA_REG_OUT; 
  SET_DATA; 
  CLEAR_SCK; 
  _delay_us(10);         
  for(i=0;i<9;i++)                  
     { 
     SET_SCK; 
     _delay_us(10); 
    CLEAR_SCK; 
  } 
  s_transstart();                   //start transmisji 
} 


void printFloat(float number) {
  number = round(number * 100) / 100; /* round off to 2 decimal places */
  transmitByte('0' + number / 10);                       /* tens place */
  transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
  transmitByte('.');
  transmitByte('0' + (number * 10) - floor(number) * 10);    /* tenths */
                                                   /* hundredths place */
  transmitByte('0' + (number * 100) - floor(number * 10) * 10);
  printString("\r\n");
}


/****************************************************************************/ 
// 7SEGMENT DISPLAY
/****************************************************************************/ 


void sendCommand(uint8_t command, uint8_t value)
{
    SLAVE_SELECT;
    SPI_tradeByte(command);
    SPI_tradeByte(value);
    SLAVE_DESELECT;
} 

void displayNumber(uint16_t number) 
{
    uint8_t i, j, numberOfDigits=4;

    for(i = 0; i < numberOfDigits ; i++)
    {
            j=number % 10;
            sendCommand(numberOfDigits - i, j);
            if(i == 2)
            {
                sendCommand(2, j | 0b10000000);
            }
            number = number / 10;
    }
}

/****************************************************************************/ 
// MAIN 
/****************************************************************************/ 

int main (void) 
{ 
    float Fvalue_temp, Fvalue_humi;
    uint16_t Lvalue_temp, Lvalue_humi, Tvalue;  
    uint8_t byte_1, byte_2, i;  

    //clock_prescale_set(clock_div_1);                             /* 8MHz */ 
    
    initUSART();
    initSPI();
    
    SLAVE_SELECT;
    sendCommand(12, 1); // Normal Operation Mode
    sendCommand(15, 0); // Close Display Test
    sendCommand(10, 1); // Set Display Intensity(0-15)
    sendCommand(11, 4); // Command to limit digit scan of 7221 
    sendCommand(9, 255); // Command to decode. Standard 7segment Display
    SLAVE_DESELECT;
    
    printString("\r\n====  i2c Thermometer  ====\r\n");

    DDRC |= (1 << PC4)|(1 << PC5);

  // ------ Event loop ------ //
  while (1) {
    
    //Measuring Temperature
    
    s_transstart();  
    
    s_write_byte(MEASURE_TEMP); 

    while(bit_is_set(PINC,4));
    
    byte_1 = s_read_byte(ACK); 
    byte_2 = s_read_byte(ACK); 
    s_read_byte(ACK);          
          
    Lvalue_temp = (byte_1 * 256) + byte_2; 
    Fvalue_temp = D1 + (D2 * (float)Lvalue_temp); 
    
    //Measuring Humidity      
    
    s_connectionreset();
    
    s_write_byte(MEASURE_HUMI);
    
    while(bit_is_set(PINC,4));
    
    byte_1 = s_read_byte(ACK); 
    byte_2 = s_read_byte(ACK);  
    s_read_byte(noACK); 

    Lvalue_humi = (byte_1 * 256) + byte_2; 
    Fvalue_humi = C1 + (C2 * (float)Lvalue_humi) + (C3 * (float)(Lvalue_humi * Lvalue_humi)); 
    

// Print it on the serial terminal over UART
    printFloat(Fvalue_temp);
    printFloat(Fvalue_humi);
    printString("\r\n");

// Print it on the 7segments over SPI
    Tvalue = (round(Fvalue_temp) * 100)+ round(Fvalue_humi);
    printWord(Tvalue);
    printString("\r\n");

    displayNumber(Tvalue); 

    _delay_ms(700);

// Enter into shut down mode
    SLAVE_SELECT;
    sendCommand(12, 0);
    SLAVE_DESELECT;
    
    for(i = 0; i < 3 ; i++)
    {
        _delay_ms(1000);
    }   
    
    SLAVE_SELECT;
    sendCommand(12, 1);
    SLAVE_DESELECT;

  }                                                  /* End event loop */

return (0); 
}//end main
