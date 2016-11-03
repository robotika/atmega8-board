/*!
* \file hwlayer.h
* \brief hardware interface
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "hwlayer.h"

// comment/uncoment based on clock speed (note, that for 8MHz you have to change
// also calibration byte
#define CLOCK_1MHZ

#ifdef CLOCK_1MHZ
// for 1MHz processor
 #define UART_BAUD_RATE_9600        12
 #define UART_BAUD_RATE_38400       2
 #define UART_BAUD_RATE UART_BAUD_RATE_9600
#else
// for 8MHz processor (U2X=1)
 #define UART_BAUD_RATE_38400       25
 #define UART_BAUD_RATE UART_BAUD_RATE_38400
 #define CALIBRATION_BYTE  0x9A
#endif

// lower bits has to be set to 1
#define RECEIVE_BUFFER_MASK 0x07
static volatile uint8_t g_buf[RECEIVE_BUFFER_MASK + 1];
static volatile uint8_t g_bufWriteIndex=0, g_bufReadIndex=0;


static volatile uint8_t servoIndex;

struct HWRead hwRead;
struct HWWrite hwWrite;

struct HWRead hwReadBuffer;
struct HWWrite hwWriteBuffer;

int8_t volatile readBufferValid;
int8_t volatile writeBufferValid;

uint8_t volatile currentADChannel;

//---------------------------------------------------------
// SERVO implementation
//
// Servo runs on 50Hz, so the time interval is 20ms. The control
// signal is 0.5ms to 2.5ms with center in 1.5ms.
// In the previous implementation we could handle 5 servos, where
// main timer overflowed every 2ms. If possible we will use the same
// strategy: main clock is 1MHz, 

void initClockCalibration(void)
{
#ifndef CLOCK_1MHZ
  // 8MHz
  OSCCAL = CALIBRATION_BYTE;
#endif
}

void initPorts(void)
{
  // outputs
  DDRB |= _BV(PB0); // blinking LED - output (or servo 0)
  
  DDRB |= _BV(PB1); // servo[0]
  DDRD |= _BV(PD7); // servo[1]
  DDRB |= _BV(PB2); // servo[2]
  DDRD |= _BV(PD6); // servo[3]

  DDRD |= _BV(PD5); // digital outputs
  DDRB |= _BV(PB6);
  DDRB |= _BV(PB7);
  
  // turn on the internal pull-up resistors for inputs
  PORTD |= _BV(PD2);
  PORTD |= _BV(PD3);
  PORTD |= _BV(PD4); 
}

void initVariables(void)
{
  int i;
  
  hwRead.R_timer = 0;
  hwRead.R_digitalInputs = 0;

  for(i = 0; i < sizeof(hwRead.R_analog); i++)
  {
    hwRead.R_analog[i] = 0;
  }
  
  hwWrite.W_watchDog = 0;
  
  for(i = 0; i < sizeof(hwWrite.W_servo); i++)
  {
    hwWrite.W_servo[i] = 0;
  }

  readBufferValid = 0;
  writeBufferValid = 0;
  currentADChannel = 0;
}

void initTimer2(void) // synchronize control structure with the world at 500Hz
{	
  TCNT2 = 6;     // initial value for 1ms (not imported)
#ifdef CLOCK_1MHZ
  TCCR2 = 2;     // clock prescale select CK/8
#else
  TCCR2 = 4;     // 8Hmz -> clock prescale select CK/64
#endif
  TIMSK |= _BV(TOIE2);    // enable timer interupt on overflow
  TIMSK |= _BV(OCIE2);    // enable timer interupt on compare match
}

void initAnalog(void)
{
  ADMUX = 0x20; // select channel ADC0, AREF as reference, left adjusted
  ADCSR = 0xCF; // enable conversion, start single conversion, 
                     //enable interrupt, prescale /128
}

void initUART(void)
{
  g_bufWriteIndex = 0;
  g_bufReadIndex = 0;

  // double USART transmission speed
  UCSRA = (1<<U2X);
  
  // enable RxD/TxD and receive interrupt
  UCSRB = ((1<<RXCIE)|(1<<RXEN)|(1<<TXEN));

  // set 8bit, 1stop bit, no parity, asynchronous operation
  UCSRC = ((1<<URSEL)|(3<<UCSZ0));

  // set baud rate
  UBRRH = ((unsigned char)(UART_BAUD_RATE>>8));
  UBRRL = ((unsigned char)UART_BAUD_RATE);
}

void initHW(void)
{
  initClockCalibration();
  initPorts();
  initVariables();
  initTimer2();
  initAnalog();
  initUART();
  sei();                 /* enable interrupts */
}

//--------------------------------------------------
//! 8 bit timer overflow - synchronize structure
SIGNAL(SIG_OVERFLOW2)
{
  hwRead.R_timer++;

  if(hwRead.R_timer == 0)
  {
    PORTB |= _BV(PB0);
  }
  else if(hwRead.R_timer == 128)
  {
    PORTB &= ~(_BV(PB0));
  }

  if(writeBufferValid)
  {
    if( (int8_t)(hwWriteBuffer.W_executeAt - hwRead.R_timer) <= 0)
    {
      hwWrite = hwWriteBuffer;
      writeBufferValid = 0;
      hwReadBuffer = hwRead;
      readBufferValid = 1;
    }
  }  

  // apply watch-dog
  if(hwWrite.W_watchDog == 0)
  {
    // turn off everything
    int8_t i;
    for(i = 0; i < sizeof(hwWrite.W_servo); i++)
    {
      hwWrite.W_servo[i] = 0;
    }	
  }

  // watch dog
  if(hwWrite.W_watchDog != 0)
  {
    hwWrite.W_watchDog--;
  }

  
  servoIndex++;
  if(servoIndex >= 10)
  {
    servoIndex = 0;
  }
  
  // "digital output" (servo[3])
  if(hwWrite.W_servo[3] == 0)
    PORTD &= ~(_BV(PD6));
  else if(hwWrite.W_servo[3] == 255)
    PORTD |= _BV(PD6);
  // ignore values between 0 and 255

  
  if(servoIndex < 2*sizeof(hwWrite.W_servo) && hwWrite.W_servo[servoIndex/2] != 0)
  {
    switch(servoIndex)
    {
      case 0:
        PORTB &= ~(_BV(PB1));
        break;
      case 1:
        PORTB |= _BV(PB1);
        break;
      case 2:
        PORTD &= ~(_BV(PD7));
        break;
      case 3:
        PORTD |= _BV(PD7);
        break;
      case 4:
        PORTB &= ~(_BV(PB2));
        break;
      case 5:
        PORTB |= _BV(PB2);
        break;
      case 6:
        PORTD &= ~(_BV(PD6));
        break;
      case 7:
        PORTD |= _BV(PD6);
        break;
      default:
        break;
    }

    if( (servoIndex & 0x01) == 0)
    {
      // even cases
      OCR2 = (6 + 125 + 125/2);
    }
    else
    {
      OCR2 = hwWrite.W_servo[servoIndex/2];
    }  
  }

  hwRead.R_digitalInputs = (~(PIND >> 2)) & 0x7;

  // outputs
  if(hwWrite.W_digitalOutputs & 0x1)
    PORTB |= _BV(PB6);
  else
    PORTB &= ~(_BV(PB6));

  if(hwWrite.W_digitalOutputs & 0x2)
    PORTB |= _BV(PB7);
  else
    PORTB &= ~(_BV(PB7));

  if(hwWrite.W_digitalOutputs & 0x4)
    PORTD |= _BV(PB5);
  else
    PORTD &= ~(_BV(PB5));
}
//--------------------------------------------------
//! timer2 - compare match (update servos)
SIGNAL(SIG_OUTPUT_COMPARE2)
{
  if(servoIndex < 2*sizeof(hwWrite.W_servo) && hwWrite.W_servo[servoIndex/2] != 0)
  {
    switch(servoIndex)
    {
      case 0:
        PORTB |= _BV(PB1);
        break;
      case 1:
        PORTB &= ~(_BV(PB1));
        break;
      case 2:
        PORTD |= _BV(PD7);
        break;
      case 3:
        PORTD &= ~(_BV(PD7));
        break;
      case 4:
        PORTB |= _BV(PB2);
        break;
      case 5:
        PORTB &= ~(_BV(PB2));
        break;
      case 6:
        PORTD |= _BV(PD6);
        break;
      case 7:
        PORTD &= ~(_BV(PD6));
        break;
      default:
        break;
    }
  }
}

//--------------------------------------------------
//! analog-digital conversion completed
SIGNAL(SIG_ADC)
{  
  hwRead.R_analog[currentADChannel] = ADCH;

  currentADChannel++;
  if(currentADChannel >= sizeof(hwRead.R_analog))
      currentADChannel=0;
  
  // select next channel
  ADMUX = (0x20 | currentADChannel);
  ADCSR |= _BV(ADSC); // start a single conversion
}

//--------------------------------------------------
SIGNAL(SIG_UART_RECV)      
/* signal handler for receive complete interrupt */
{
  g_buf[g_bufWriteIndex] = UDR;
  g_bufWriteIndex = (g_bufWriteIndex + 1) & RECEIVE_BUFFER_MASK;

  // note, that if g_bufWriteIndex == g_bufReadIndex -> buffer overflow
}

//! send byte over serial line
void sendUART(unsigned char in_byteToSend)
{
  // wait for UART data register empty flag
  while (!bit_is_set(UCSRA, UDRE))
    ;
	
  UDR = in_byteToSend;
}


//! non-blocking recieve byte (return NO_BYTE_AVAILABLE if theere is no byte available) 
int16_t receiveUART(void)
{
  int16_t returnVal = BYTE_NOT_AVAILABLE;

  if(g_bufReadIndex != g_bufWriteIndex)
  {
    returnVal = g_buf[ g_bufReadIndex ];
    g_bufReadIndex = (g_bufReadIndex + 1) & RECEIVE_BUFFER_MASK;
  }

  return returnVal;
}
//--------------------------------------------------

