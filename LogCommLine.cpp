/*!
* \file LogCommLine.cpp
* \brief loggind of communication.
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#include "LogCommLine.h"

#ifndef UNDER_CE
	#include "Exception.h" 
#endif

LogCommLine::LogCommLine(ACommLine * in_commLine, char *in_filename)
: m_commLine(in_commLine)
{
  m_fd = fopen(in_filename, "wb");
  if(m_fd == NULL)
  {
#ifdef UNDER_CE
	  PRINT("Cannot open log file");
#else
	  EXCEPTION("Cannot open log file"); 
#endif
	  
  }
}

LogCommLine::LogCommLine(char *in_filename)
{
  m_fd = fopen(in_filename, "rb");
  m_commLine = 0;
}

LogCommLine::~LogCommLine()
{
  fclose(m_fd);
}

//! send single byte
void LogCommLine::sendByte(BYTE in_byteToSend)
{
  if(m_commLine)
  {
    m_commLine->sendByte(in_byteToSend);
    fputc(OUTPUT_BYTE, m_fd);
    fputc(in_byteToSend, m_fd);
    fflush(m_fd);
  }
  else
  {
    int cmd = fgetc(m_fd); // should be OUTPUT_BYTE
    int data = fgetc(m_fd); // should be in_byteToSend
  }
}

/*!
* \brief wait X ms (infinitely for negative number) for a byte to be received
* \retval -1 for time out
*/
int LogCommLine::waitForByte(int in_waitMs)
{
  int data;
  if(m_commLine)
  {
    data = m_commLine->waitForByte(in_waitMs);
    if(data >= 0)
    {
      fputc(INPUT_BYTE, m_fd);
      fputc(data, m_fd);
      fflush(m_fd);
    }
    else
    {
      fputc(INPUT_TIME_OUT, m_fd);
      fputc(0xff, m_fd);
      fflush(m_fd);
    }
  }
  else
  {
    int cmd = fgetc(m_fd); // should be INPUT_BYTE or INPUT_TIME_OUT
    data = fgetc(m_fd);
    if((cmd == INPUT_TIME_OUT) || (data == EOF))
      data = -1;
  }
  return data;
}

//! was the connection established ok?
bool LogCommLine::connected()
{
  if(m_commLine)
    return (m_fd != NULL) && m_commLine->connected();
  else
    return (m_fd != NULL);
}
