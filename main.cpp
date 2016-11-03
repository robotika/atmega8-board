/**
 *   Simple example of working with Daisy-like robot (Daisy, Fatima, Ferda, ...)
 * 
 *   Author: jiri.isa -at- matfyz.cz
 *   Created: 16.10.2005
 *   Modified: 15.11.2005
 *   
 */

#include "Logger.h"
#include "RealHardware.h"

#ifdef __unix__
#include <termios.h>	//for B9600 baudrate
#endif

/* Flush output, wait for next input.
 */
void sync(AHardware *hw)
{
	//schedule next execution
	hw->W_executeAt = hw->R_timer + 8;

	//reset watchdog
	hw->W_watchDog = 0xFF;
	
	hw->synchronize();
}


int main(void)
{
#ifdef __unix__
  AHardware *hw = new RealHardware("dummy", "/dev/ttyS0", B9600);
#else
  AHardware *hw = new RealHardware("dummy", "COM7", 9600);
#endif

  // AHardware *hw = new Logger("dummy_051115_0617.log");
  // AHardware *hw = new Logger("dummy_04_23_09_05.log", Logger::CHECK_OUTPUT);
  
  hw->W_servo[0] = 0; // init HWRead structure
  hw->W_servo[1] = 0;
  sync(hw);

  // move servo0 from left to right and back, while servo1 reflects analog0
  uint8_t i;
  for(i = 30; i < 220; i++)
  {
    hw->W_servo[0] = i;
    hw->W_servo[1] = hw->R_analog[0];
    sync(hw);
  }

  for(i = 220; i > 30; i--)
  {
    hw->W_servo[0] = i;
    hw->W_servo[1] = hw->R_analog[0];
    sync(hw);
  }

  delete hw;
  return 0;
}
