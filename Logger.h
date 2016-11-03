/*!
* \file Logger.h
* \brief simulated hardware from a log file
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef LOGGER_H
#define LOGGER_H

#include "AHardware.h"

class Logger : public AHardware
{
public:
  enum Mode
  {
    UPDATE_ALL, // fill structure with both output and input bytes
    CHECK_OUTPUT // make sure, that output bytes are the same as in the log file
  };

  Logger(const char *in_filename, Mode in_mode = UPDATE_ALL);
  virtual ~Logger();

  virtual void synchronize();

private:
  unsigned char getLogByte(unsigned char in_expectedPrefix);
private:
  FILE *m_fd;
  Mode m_mode;
};

#endif
