/*!
* \file LogCommLine.h
* \brief loggind of communication.
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef _LOGCOMMLINE_H_
#define _LOGCOMMLINE_H_

#include "ACommLine.h"
#include <stdio.h>

//! input/output communication channel
class LogCommLine : public ACommLine
{
public:
  // logging external com
  LogCommLine(ACommLine * in_commLine, char *in_filename);
  // replay from file com
  LogCommLine(char *in_filename);

  virtual ~LogCommLine();

  //! send single byte
  virtual void sendByte(BYTE a_toSend);

  /*!
  * \brief wait X ms (infinitely for negative number) for a byte to be received
  * \retval -1 for time out
  */
  virtual int waitForByte(int a_waitMs = -1);

  //! was the connection established ok?
  virtual bool connected();

  enum
  {
    OUTPUT_BYTE = 0,
    INPUT_BYTE = 1,
    INPUT_TIME_OUT = 2
  };

private:

  ACommLine *m_commLine;
  FILE *m_fd;

};

#endif 
// _LogCommLine_H_
