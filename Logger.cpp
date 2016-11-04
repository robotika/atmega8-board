/*!
* \file Logger.cpp
* \brief simulated hardware from a log file
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#include "Logger.h"
#include "LogCommLine.h"

#include <iostream>
using namespace std;

Logger::Logger(const char *in_filename, Mode in_mode)
: m_mode(in_mode)
{
  m_fd = fopen(in_filename, "rb");
  if(m_fd == NULL)
  {
    EXCEPTION("Cannot open log file!"); 
  }
}

Logger::~Logger()
{
  if(m_fd != NULL)
  {
    fclose(m_fd);
  }
}

void Logger::synchronize()
{
  unsigned char tmp,tmp2;
  unsigned char len=0, cmd=0;
  
  do
  {
    do
    {
      tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
      if(tmp == ECHO_CHAR)
      {
        while(tmp == ECHO_CHAR)
        {
          tmp = getLogByte(LogCommLine::INPUT_BYTE);
        }
      }
    } while (tmp != PACKET_START);

    if(m_mode == CHECK_OUTPUT)
    {
      tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
      if( W_executeAt != (uint8_t)tmp) 
      {
//        EXCEPTION("Generated output differs (PWM0)!");
        cerr << "Generated output differs (executeAt)!" << (int)W_executeAt << "\t" << (int)tmp << endl;
        W_executeAt = tmp;
      }

      if( W_watchDog != getLogByte(LogCommLine::OUTPUT_BYTE))
      {
        EXCEPTION("Generated output differs (watchdog)!");
      }
      tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
      tmp2 = getLogByte(LogCommLine::OUTPUT_BYTE);
      if(
          (W_servo[0] != tmp) ||
          (W_servo[1] != tmp2))
      {
        EXCEPTION("Generated output differs (Servos HAND)!");
      }
      
      tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
      tmp2 = getLogByte(LogCommLine::OUTPUT_BYTE);
      
      if(
          (W_servo[2] != tmp) ||
          (W_servo[3] != tmp2))
      {
        //EXCEPTION("Generated output differs (Servos (fire & sharp))!");
      }
      tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
      if(W_digitalOutputs != tmp)
      {
        EXCEPTION("Generated output differs (digitalOutput)!");
      }
    }
    else
    {
      W_executeAt = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_watchDog = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_servo[0] = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_servo[1] = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_servo[2] = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_servo[3] = getLogByte(LogCommLine::OUTPUT_BYTE);
      W_digitalOutputs = getLogByte(LogCommLine::OUTPUT_BYTE);
    }

    tmp = getLogByte(LogCommLine::OUTPUT_BYTE);
  } while( (unsigned char)(W_executeAt + W_watchDog 
    + W_servo[0] + W_servo[1] + W_servo[2] + W_servo[3] + W_digitalOutputs + tmp));

  tmp = getLogByte(LogCommLine::INPUT_BYTE);
  // !!! TODO !!! check tmp = PACKET_START
  R_timer = getLogByte(LogCommLine::INPUT_BYTE);
  R_digitalInputs = getLogByte(LogCommLine::INPUT_BYTE);
  
  for(size_t i = 0; i < sizeof(HWRead::R_analog); i++)
  {
    R_analog[i] = getLogByte(LogCommLine::INPUT_BYTE);
  }

  tmp = getLogByte(LogCommLine::INPUT_BYTE);
  // !!! TODO !!! check tmp = check sum !!!
}

unsigned char Logger::getLogByte(unsigned char in_expectedPrefix)
{
  if(m_fd == NULL)
  {
    EXCEPTION("NULL m_fd pointer!"); 
	  
  }

  int c = fgetc(m_fd);
  if(c == EOF)
  {
    EXCEPTION("End of file");
  }
  
  // there is a problem what to do with time-out values
  if(c != in_expectedPrefix && c != LogCommLine::INPUT_TIME_OUT)
  {
    EXCEPTION("Unexpected character!");
  }

  c = fgetc(m_fd);
  if(c == EOF)
  {
    EXCEPTION("End of file");
  }
  
  return c & 0xFF;
}
