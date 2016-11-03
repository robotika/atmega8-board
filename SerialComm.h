/*!
* \file SerialComm.h
* \brief Wrap around serial communication
*
* Current solution is directly for Win32, but can be simply split.
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef _SerialComm_H_
#define _SerialComm_H_

#include <windows.h>
// this can be already defined
typedef unsigned char BYTE;

#include "ACommLine.h"

class SerialComm : public ACommLine
{
public:
  //SerialComm(char *a_comName = "COM2", char *a_comParams = "38400,n,8,1"); pavel
	SerialComm(TCHAR *a_comName, int in_baudRate);
  ~SerialComm();

  /*!
  * \brief wait X ms (infinitely for negative number) for a byte to be received
  * \retval -1 for time out
  */
  virtual int waitForByte(int a_waitMs = -1);


  virtual void sendByte(BYTE a_byte);

  //! send buffer of n bytes
  virtual void sendBuffer(BYTE *a_buf, unsigned int a_len);

  //! was the connection established ok?
  virtual bool connected();

private:
  HANDLE m_hComm; // COM port handle
  int m_timeOutMs; // time out in miliseconds

  // set time-outs for reading, and preserve original values (md)
  COMMTIMEOUTS m_timeouts;

  bool getByte(BYTE *b_byte);
};

#endif // _SerialComm_H_

