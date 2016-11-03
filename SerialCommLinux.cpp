/*!
* \file SerialCommLinux.cpp
* \brief implementation for Linux
*
* \author md -at- robotika.cz, jiri.isa -at- matfyz.cz
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#include "SerialCommLinux.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
/* change this definition for the correct port */

//because we want to forbidd IUCLC (mapping to lower case) where it could happen and ignore the option otherwise (it is not defined by POSIX)
#ifndef IUCLC
#define IUCLC 0
#endif /*IUCLC*/

///////////////////////////////////////////////////////////
/*!
* Open com port
*
* \param a_pathName port to open
* \param a_baudRate baud rate to use
*/
SerialCommLinux::SerialCommLinux(const char *a_pathName,  speed_t a_baudRate )
{
  // open file for read and write
  m_handle = open(a_pathName, O_RDWR | O_NOCTTY );
  if(m_handle < 0)
  {
    printf("Open FAILED!\n");
    return;
  }

  tcgetattr(m_handle,&m_oldtio); /* save current serial port settings */
  tcgetattr(m_handle,&m_newtio); /*and get the working copy */

  // speed,
  cfsetispeed(&m_newtio, a_baudRate);
  cfsetospeed(&m_newtio, a_baudRate);    
    
  //char size
  m_newtio.c_cflag &= ~(CSIZE);	//clear bits used for char size
  m_newtio.c_cflag |= CS8;

  //no parity
  m_newtio.c_cflag &= ~(PARENB);	//clear parity enable
  m_newtio.c_iflag &= ~(INPCK);	//disable input parity checking
  
  //one stop bit
  ;	//default

  //no sw flow control
  m_newtio.c_cflag &= ~(IXON | IXOFF);

  //no hw flow control
  m_newtio.c_cflag &= ~(CRTSCTS); //this is not POSIX compliant! It might be omitted, if the device is already set to 'no hardware flow control"

  //raw output
  m_newtio.c_lflag = 0;		//no local flags
  m_newtio.c_oflag &= ~(OPOST);	//no output processing
  m_newtio.c_oflag &= ~(ONLCR);	//don't convert line feeds

  //no input processing
  m_newtio.c_iflag &= ~(INPCK | PARMRK | BRKINT | INLCR | ICRNL | IUCLC | IXANY);

  // ignore break conditions
  m_newtio.c_iflag |= IGNBRK;

  //enable input
  m_newtio.c_cflag |= CREAD;

    /*
      initialize all control characters
      default values can be found in /usr/include/termios.h, and are given
      in the comments, but we don't need them here
    */
    m_newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    m_newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    m_newtio.c_cc[VERASE]   = 0;     /* del */
    m_newtio.c_cc[VKILL]    = 0;     /* @ */
    m_newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    m_newtio.c_cc[VTIME]    = m_timeOutMs/100;     /* inter-character timer (in 0.1s)*/
    m_newtio.c_cc[VMIN]     = 0;     /* non-blocking read */
    m_newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    m_newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    m_newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    m_newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    m_newtio.c_cc[VEOL]     = 0;     /* '\0' */
    m_newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    m_newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    m_newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    m_newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    m_newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    // now clean the modem line and activate the settings for the port
    tcflush(m_handle, TCIFLUSH);
    if(tcsetattr(m_handle, TCSANOW, &m_newtio) == -1)
    {
      fprintf(stderr, "tcsetattr Error\n");
    }
    
   // set default timeout
  m_timeOutMs = 10000; // 1s (warning - there resolution here is only 0.1s)
}

///////////////////////////////////////////////////////////
/*!
* Destructor
*/
SerialCommLinux::~SerialCommLinux()
{
  if(m_handle >= 0)
  {
    tcsetattr(m_handle,TCSANOW,&m_oldtio);
    close(m_handle);
  }
}

///////////////////////////////////////////////////////////
/*!
* \brief was the connection established ok?
*
* \return true if connected
*/
bool SerialCommLinux::connected()
{
  return m_handle >= 0;
}


///////////////////////////////////////////////////////////
/*!
* \brief send single byte
*
* \param a_toSend byte to send
*/
void SerialCommLinux::sendByte(BYTE a_toSend)
{
  //int res = write(m_handle, &a_toSend, 1);
  write(m_handle, &a_toSend, 1);
  //TODO Let user know, if this failed
}

///////////////////////////////////////////////////////////
/*!
* \brief wait X ms (infinitely for negative number) for a byte to be received
*
* \param a_waitMs wait timout
*
* \retval -1 for time out other wise byte
*/
int SerialCommLinux::waitForByte(int a_waitMs)
{
  if(m_handle < 0)
    return -1;

  if(a_waitMs != m_timeOutMs)
  {
    m_timeOutMs = a_waitMs;

    if(m_timeOutMs > 0)
    {
      int toSet = m_timeOutMs/100;
      if(toSet == 0)
      {
        //! problem with minimal timeout (to rough resolution)
        toSet = 1;
      }

      m_newtio.c_cc[VTIME]    = toSet;     /* inter-character timer (in 0.1s)*/
      m_newtio.c_cc[VMIN]     = 0;     /* non-blocking read */
    }
    else
    {
      m_newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
      m_newtio.c_cc[VMIN]     = 1;     /* blocking read for 1 character */
    }

    // now clean the modem line and activate the settings for the port
    //TODO following line wasn't commented out. Why would we throw data away?
    //tcflush(m_handle, TCIFLUSH);
    tcsetattr(m_handle, TCSANOW, &m_newtio);
  }

  ssize_t res;
  char data;
  res = read(m_handle, &data, 1);
  if(res == 0)
  {
    return -1;
  }
  
  return (BYTE)data;
}
