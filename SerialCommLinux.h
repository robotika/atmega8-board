/*!
* \file SerialCommLinux.h
* \brief implementation for Linux
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef _SERIALCOMMLINUX_H_
#define _SERIALCOMMLINUX_H_

#include "ACommLine.h"
#include <stdio.h>
#include <termios.h>

//! Serial communications for linux
class SerialCommLinux : public ACommLine
{
public:
  //! Constructor
  SerialCommLinux(const char *a_pathName = "/dev/ttyS0", speed_t a_baudRate = B38400);
  
  //! Destructor
  virtual ~SerialCommLinux();

  //! send single byte
  virtual void sendByte(BYTE a_toSend);

  /*!
  * \brief wait X ms (infinitely for negative number) for a byte to be received
  * \retval -1 for time out
  */
  virtual int waitForByte(int a_waitMs = -1);

  //! was the connection established ok?
  virtual bool connected();

private:

  //! handle on port
  int m_handle;
  struct termios m_oldtio, m_newtio;
  //! Time out
  int m_timeOutMs;
};

#endif
 // _SERIALCOMMLINUX_H_
 
