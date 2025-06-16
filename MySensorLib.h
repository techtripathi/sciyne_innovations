#ifndef MY_SENSOR_LIB_H
#define MY_SENSOR_LIB_H
/**
 * This lib is register under GPL liscence
 */

#include <Arduino.h>
void sciyne_init();
void sciyne_init();

class MySensorLib {
  public:
    MySensorLib(int pin);     // Constructor
    void sciyne_init();             // Initialization
    void sciyne_run();         // Sample method

  private:
    int _pin;
};
#endif
