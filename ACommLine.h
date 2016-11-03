/*!
* \file ACommLine.h
* \brief Abstract class for communication.
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef _ABSTRACTCOMMLINE_H_
#define _ABSTRACTCOMMLINE_H_

//! Types - should be moved & shared
typedef unsigned char BYTE;

//! input/output communication channel
class ACommLine
{
public:
  virtual ~ACommLine(){}

  //! send single byte
  virtual void sendByte(BYTE a_toSend) = 0;

  //! send buffer of bytes (can be replaced by faster blocked implementation)
  virtual void sendBuffer(BYTE *a_buf, int a_len)
  {
    while(a_len-- > 0)
      sendByte(*a_buf++);
  }

  /*!
  * \brief wait X ms (infinitely for negative number) for a byte to be received
  * \retval -1 for time out
  */
  virtual int waitForByte(int a_waitMs = -1) = 0;

  //! was the connection established ok?
  virtual bool connected() = 0;
};

#endif 
// _ABSTRACTCOMMLINE_H_
