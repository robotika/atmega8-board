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

#define UP_DOWN_SERVO 3
#define LEFT_RIGHT_SERVO 2

#define LEFT_LIMIT 0x04
#define UP_LIMIT   0x08  // 9??
#define DOWN_LIMIT 0x10

void stop(AHardware *hw)
{
  hw->W_servo[0] = 0; // no idea why it is needed :(
  hw->W_servo[1] = 0;
  hw->W_servo[2] = 0;
  hw->W_servo[3] = 0;
  sync(hw);
}

void move_left(AHardware *hw)
{
  int i;
  for(i = 0; i < 100; i++)
  {
    hw->W_servo[0] = 10; // no idea why it is needed :(
    hw->W_servo[1] = 10;
    hw->W_servo[LEFT_RIGHT_SERVO] = 30;
    sync(hw);
    if((hw->R_digitalInputs & LEFT_LIMIT) == 0)
      break;
  }
  stop(hw);
}

void move_right(AHardware *hw)
{
  int i;
  for(i = 0; i < 100; i++)
  {
    hw->W_servo[0] = 10; // no idea why it is needed :(
    hw->W_servo[1] = 10;
    hw->W_servo[LEFT_RIGHT_SERVO] = 200;
    sync(hw);
    if(hw->R_analog[0] > 128)
      break;
  }
  stop(hw);
}

void move_down(AHardware *hw)
{
  int i;
  for(i = 0; i < 100; i++)
  {
    hw->W_servo[0] = 10; // no idea why it is needed :(
    hw->W_servo[1] = 10;
    hw->W_servo[UP_DOWN_SERVO] = 30;
    sync(hw);
    if((hw->R_digitalInputs & DOWN_LIMIT) == 0)
      break;
  }
  stop(hw);
}

void move_up(AHardware *hw)
{
  int i;
  for(i = 0; i < 1000; i++)
  {
    hw->W_servo[0] = 250; // no idea why it is needed :(
    hw->W_servo[1] = 250;
    hw->W_servo[UP_DOWN_SERVO] = 250;
    sync(hw);
    if((hw->R_digitalInputs & UP_LIMIT) == 0)
      break;
  }
  stop(hw);
}

int main(void)
{
#ifdef __unix__
  AHardware *hw = new RealHardware("dummy", "/dev/ttyS0", B9600);
#else
  AHardware *hw = new RealHardware("dummy", "\\\\.\\COM15", 38400);
#endif

  // AHardware *hw = new Logger("dummy_051115_0617.log");
  // AHardware *hw = new Logger("dummy_04_23_09_05.log", Logger::CHECK_OUTPUT);
  
  hw->W_servo[0] = 0; // init HWRead structure
  hw->W_servo[1] = 0;
  hw->W_servo[2] = 0;
  hw->W_servo[3] = 0;
  hw->W_digitalOutputs = 0;
  sync(hw);

  //move_left(hw);
  move_right(hw);
  //move_down(hw);
  //move_up(hw);
  //move_down(hw);

  delete hw;
  return 0;
}
