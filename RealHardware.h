/*!
* \file RealHardware.h
* \brief real hardware (using serial line for synchronization)
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef REALHARDWARE_H
#define REALHARDWARE_H

#include "AHardware.h"
#include "ACommLine.h"

class RealHardware : public AHardware
{
public:
	RealHardware(const char *in_robotName, const char *in_comPort, int in_baudRate);
  
  virtual ~RealHardware();

  virtual void synchronize();

private:
  void restoreCommunication();

private:
  ACommLine *m_realComm;
  ACommLine *m_loggedComm;
  unsigned int m_lastSleepTime;
};

#endif
