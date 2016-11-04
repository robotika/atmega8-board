/*!
* \file RealHardware.cpp
* \brief real hardware
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.3
*  Date: 2005/11/14
*/

#include "RealHardware.h"
#ifdef __unix__
	#include "SerialCommLinux.h"
	#include <sys/time.h>
	#include <time.h>
#elif defined(_WIN32)
	#include "SerialComm.h"
#else
	#error "Unknown platform"
#endif
#include "LogCommLine.h"

RealHardware::RealHardware(const char *in_robotName, const char *in_comPort, int in_baudRate)
: m_lastSleepTime(0)
{
#if defined(UNDER_CE)
	m_realComm = new SerialComm(TEXT("COM1:"));
#elif defined(__linux__)
	m_realComm = new SerialCommLinux(in_comPort, in_baudRate);
#elif defined(__unix__)
	#error "Unknown platform. Specify your serial port device."
#else
  m_realComm = new SerialComm((TCHAR*)in_comPort, in_baudRate);
#endif  
  if (!m_realComm->connected())
  {
    
#ifdef UNDER_CE
	  MessageBox(0,TEXT("Error: not connected!"),TEXT("Error"),MB_OK);
#else
	  EXCEPTION("Error: not connected!"); 
#endif

  }

  while(m_realComm->waitForByte(1) != -1)
    ;

  //build file name
  char s[100];
#if defined(_WIN32 )
  SYSTEMTIME Time; 
  GetLocalTime(&Time);
#ifdef UNDER_CE
  sprintf(s,"daisy_ce_%02d_%02d_%02d_%02d.log",Time.wMonth,Time.wDay,Time.wHour,Time.wMinute);
#else
  sprintf(s,"%s_%02d%02d%02d_%02d%02d.log",in_robotName, Time.wYear % 100, Time.wMonth,Time.wDay,Time.wHour,Time.wMinute);
#endif
#elif defined(__unix__)
  struct timeval Time;
  gettimeofday(&Time, NULL);
  char dateString[100];
  strftime(dateString, 100, "%Y_%m_%d_%H_%M_%S", localtime(&(Time.tv_sec)));
  snprintf(s, 100, "%spc%s.log", in_robotName, dateString);
#endif
  
  m_loggedComm = new LogCommLine(m_realComm, s);

  restoreCommunication();
}

RealHardware::~RealHardware()
{
  delete m_loggedComm;
  delete m_realComm;
}

void RealHardware::restoreCommunication()
{
  const int MAX_NUM_TRIES = 20;
  int i;
  for(i = 0; i < MAX_NUM_TRIES; i++)
  {
    m_loggedComm->sendByte(ECHO_CHAR);
    if(m_loggedComm->waitForByte(100) == ECHO_CHAR)
    {
      while (m_loggedComm->waitForByte(100) == ECHO_CHAR)
        ;
      return;
    }
  }
  EXCEPTION("Communication failed!");
}


void RealHardware::synchronize()
{
  const int timeout = 1000; // ms

  int tmp;
  do
  {
    m_loggedComm->sendByte(PACKET_START);
    m_loggedComm->sendByte(W_executeAt);
    m_loggedComm->sendByte(W_watchDog);
    m_loggedComm->sendByte(W_servo[0]);
    m_loggedComm->sendByte(W_servo[1]);
    m_loggedComm->sendByte(W_servo[2]);
    m_loggedComm->sendByte(W_servo[3]);
    m_loggedComm->sendByte(W_digitalOutputs);

    m_loggedComm->sendByte(256 - (W_executeAt + W_watchDog
      + W_servo[0] + W_servo[1] + W_servo[2] + W_servo[3] + W_digitalOutputs));

    // unknown reason for "patch"
    m_loggedComm->sendByte(ECHO_CHAR);
    m_loggedComm->sendByte(ECHO_CHAR);

    tmp = m_loggedComm->waitForByte(timeout);
    if(tmp != PACKET_START)
    {
      restoreCommunication();
    }
  } while( tmp != PACKET_START);

  R_timer = m_loggedComm->waitForByte(timeout);
  uint8_t enc0 = m_loggedComm->waitForByte(timeout);  // ignored encoders
  uint8_t enc1 =m_loggedComm->waitForByte(timeout);  // ignored encoders
  R_digitalInputs = m_loggedComm->waitForByte(timeout);
  
  for(size_t i = 0; i < sizeof(HWRead::R_analog); i++)
  {
    R_analog[i] = m_loggedComm->waitForByte(timeout);
  }

  unsigned char checkSum = m_loggedComm->waitForByte(timeout) + enc0 + enc1;
  checkSum += R_timer;
  checkSum += R_digitalInputs;
  for(size_t i = 0; i < sizeof(HWRead::R_analog); i++)
  {
    checkSum += R_analog[i];
  }
}
