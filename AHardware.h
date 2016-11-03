/*!
* \file AHardware.h
* \brief general interface to a hardware (via HWLayer structure)
*
*  URL: http://robotika.cz/
*  
*  Revision: 1.2
*  Date: 2005/11/01
*/

#ifndef ABSTRACTHARDWARE_H
#define ABSTRACTHARDWARE_H

#include "robobios/hwlayer.h"
#include "Exception.h"

class AHardware : public HWRead, public HWWrite
{
public:
  virtual ~AHardware() {}
  virtual void synchronize() = 0; 
};

#endif
