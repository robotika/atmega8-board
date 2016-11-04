/*!
* \file hwlayer.h
* \brief hardware interface
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef HWLAYER_H
#define HWLAYER_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef WIN32
#include <inttypes.h>
#else
#include "stdint.h"
#endif

#define PACKET_START 0xAB
#define ECHO_CHAR    'D'

#define SERVO_BOT_CMD 0x10  // values 0x00 - 0x0F are reserved

//! interface structure (should be general to all used microcontrollers)
struct HWRead
{
	uint8_t R_timer; //! ~500Hz
  uint8_t R_digitalInputs;
  uint8_t R_analog[8];
};

struct HWWrite
{
  uint8_t W_executeAt;
  uint8_t W_watchDog;
  uint8_t W_servo[4];
  uint8_t W_digitalOutputs;
};

extern struct HWRead hwReadBuffer;
extern struct HWWrite hwWriteBuffer;

extern volatile int8_t readBufferValid;
extern volatile int8_t writeBufferValid;

//! initialize all ports, timers, interrupts ...
void initHW(void);


//! no byte available
#define BYTE_NOT_AVAILABLE -1

//! send byte over serial line
void sendUART(uint8_t in_byteToSend);

//! non-blocking recieve byte (return BYTE_NOT_AVAILABLE if theere is no byte available) 
int16_t receiveUART(void);

#ifdef __cplusplus
}
#endif

#endif

