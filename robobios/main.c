/*!
* \file main.c
* \brief main program for atmega8 RoboBIOS
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#include "hwlayer.h"

uint8_t waitForByte(void)
{
  int16_t retVal;
  do
  {
    retVal = receiveUART();
  } while(retVal == BYTE_NOT_AVAILABLE);

  return retVal & 0xff;
}

#define SEND_WITH_CHECKSUM(X) sendUART( (X) ); tmpSum += (X);

void sendWelcomeMsg(void)
{
  sendUART('A');
  sendUART('T');
  sendUART('m');
  sendUART('e');
  sendUART('g');
  sendUART('a');
  sendUART('8');
  sendUART(' ');
  sendUART('v');
  sendUART('1');
  sendUART('.');
  sendUART('2');
  sendUART('\r');
  sendUART('\n');
}

int main(void)
{
  uint8_t i, len, cmd;
  struct HWRead hwReadCopy;
  struct HWWrite hwWriteCopy;
  
  uint8_t tmp, tmpSum;
  
  initHW();
  sendWelcomeMsg();
  for(;;)
  {
    while( (tmp = waitForByte()) != PACKET_START)
    {
      if(tmp == ECHO_CHAR)
      {
        sendUART(ECHO_CHAR);
      }
    }

    tmpSum = 0;
    tmpSum += len = waitForByte();
    tmpSum += cmd = waitForByte();
    
    // support only one message for servobot
    if((len != sizeof(hwWriteCopy) + 1) || (cmd != SERVO_BOT_CMD))
      continue;

    for(i = 0; i + 1 < len; i++)
      tmpSum += ((uint8_t*)&hwWriteCopy)[i] = waitForByte();
          
    tmpSum += waitForByte();

    if(tmpSum == 0)
    {
      hwWriteBuffer = hwWriteCopy;
      readBufferValid = 0;
      writeBufferValid = 1;
      
      while(readBufferValid == 0)
      {
        ;
      }
      hwReadCopy = hwReadBuffer;
      
      tmpSum = 0;
      sendUART(PACKET_START);
      SEND_WITH_CHECKSUM( sizeof(hwReadCopy) + 1 );
      SEND_WITH_CHECKSUM( SERVO_BOT_CMD | 0x80 ); // response command
      SEND_WITH_CHECKSUM( hwReadCopy.R_timer );
      SEND_WITH_CHECKSUM( hwReadCopy.R_digitalInputs );
      for(i = 0; i < sizeof(hwReadCopy.R_analog); i++)
      {
        SEND_WITH_CHECKSUM( hwReadCopy.R_analog[i] );
      }
      sendUART(-tmpSum);
    }
  }
}

