/*!
* \file SerialComm.cpp
* \brief Wrap around serial communication
*
* Current solution is directly for Win32, but can be simply split.
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/
#include "SerialComm.h"
#include <stdio.h>

#ifdef UNDER_CE
#  include "../iPAQConsole/stdafx.h"
#  define PRINT(X) MessageBox(0,TEXT(X),TEXT("Error"),MB_OK)
#else
#  define PRINT(X) printf(X)
#endif

///////////////////////////////////////////////////////////
//! initialize communication

//SerialComm::SerialComm(TCHAR *a_comName, TCHAR *a_comParams)
SerialComm::SerialComm(TCHAR *a_comName, int in_baudRate)
{
  m_timeOutMs = 1000; // one second

  m_hComm = CreateFile( a_comName,
                    GENERIC_READ | GENERIC_WRITE, 
                    0, 
                    0, 
                    OPEN_EXISTING,
                    0,
                    0);
  if (m_hComm == INVALID_HANDLE_VALUE)
  {;
	PRINT("Error Opening Port\n");
    return ; //-1; //md
  }

#ifndef UNDER_CE
	fprintf(stderr, "%s Opened\n", a_comName); 
#endif

   if(!GetCommTimeouts(m_hComm, &m_timeouts))
  {
	   PRINT("Cannot set timeouts for COM!\n");
  }
  else
  {
    COMMTIMEOUTS newTimeOuts = m_timeouts;

#ifdef UNDER_CE
	fprintf(stderr, "Original Read timeouts (%d, %d, %d)\n",
        m_timeouts.ReadIntervalTimeout,
        m_timeouts.ReadTotalTimeoutMultiplier,
        m_timeouts.ReadTotalTimeoutConstant );
#endif
    newTimeOuts.ReadIntervalTimeout = MAXDWORD; // no limit between two chars
    newTimeOuts.ReadTotalTimeoutMultiplier = 0;
    newTimeOuts.ReadTotalTimeoutConstant = m_timeOutMs;

    if(!SetCommTimeouts(m_hComm, &newTimeOuts))
		PRINT("Cannot set new timeouts for COM!\n");
  }


  DCB dcb;

  FillMemory(&dcb, sizeof(dcb), 0);
  dcb.DCBlength = sizeof(dcb);
  //38400,n,8,1
  GetCommState (m_hComm, &dcb);

  // Change the DCB structure settings.
  dcb.BaudRate = in_baudRate;       // Current baud 
  dcb.ByteSize = 8;                 // Number of bits/bytes, 4-8 
  dcb.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space 
  dcb.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2 

	// set new state
	//
	if (!SetCommState(m_hComm, &dcb))
		PRINT("SetCommState Error\n");
}


///////////////////////////////////////////////////////////
//! terminate communication
SerialComm::~SerialComm()
{
  if(!SetCommTimeouts(m_hComm, &m_timeouts))
	  PRINT("Cannot restore old timeouts for COM!\n");

  CloseHandle(m_hComm);
}

///////////////////////////////////////////////////////////
//! get single byte
bool SerialComm::getByte(BYTE *b_byte)
{
  DWORD dwRead;

  if (ReadFile(m_hComm, b_byte, 1, &dwRead, NULL))
  {
    if(dwRead == 1)
      return true;
    return false;
  }
  printf("ReadFile error\n");
  return false;
}

///////////////////////////////////////////////////////////
//! check, if the connection was well established
bool SerialComm::connected()
{
  return m_hComm != INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////
//! send single byte
void SerialComm::sendByte(BYTE a_byte)
{
  sendBuffer(&a_byte, 1);
}

///////////////////////////////////////////////////////////
//! send buffer of n bytes
void SerialComm::sendBuffer(BYTE *a_buf, unsigned int a_len)
{
  DWORD dwWritten;

  // NOTE: here could be test of suceessfull writes depending on return
  // value
  if (!WriteFile(m_hComm, a_buf, a_len, &dwWritten, NULL ) || (dwWritten != a_len))
  {
    printf("WriteFile Error (written %d bytes instead of %d.\n", dwWritten, a_len);
  }
}


///////////////////////////////////////////////////////////
//! interface for abstract class
int SerialComm::waitForByte(int a_waitMs)
{
  // there is a problem in USB-RS232 driver -> needs setting ALWAYS!
//  if(m_timeOutMs != a_waitMs)
  {
    COMMTIMEOUTS newTimeOuts = m_timeouts;

    newTimeOuts.ReadIntervalTimeout = MAXDWORD; // no limit between two chars
    newTimeOuts.ReadTotalTimeoutMultiplier = 0;
    newTimeOuts.ReadTotalTimeoutConstant = a_waitMs; // bad - old - version!!! m_timeOutMs;

    if(!SetCommTimeouts(m_hComm, &newTimeOuts))
      printf("Cannot set new timeouts for COM!\n");
    m_timeOutMs = a_waitMs;
  }


  BYTE byte;
  if(!getByte(&byte))
    return -1;

  return byte;
}

