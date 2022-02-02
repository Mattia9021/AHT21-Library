#ifndef AHT21_H
#define AHT21_H

#include <Arduino.h>
#include <Wire.h>

#define AHT21_I2C_ADDRESS   0x38
#define AHT21_MEASURE_TIME    80 // in ms
#define AHT21_WAIT_TIME      100 // in ms

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Class that stores state and functions for interacting with AHT21
 */
class AHT21
{
public:
  AHT21();
  AHT21(TwoWire &i2c);
  int begin(unsigned char pAddress = AHT21_I2C_ADDRESS); 
  int read(float *pTemp, float *pHum, bool pForceRead = false);

private:
  int initialize(void);
  int triggerMeasure(void);
  int getMeasure(void);

  unsigned char rmAHT21Address;
  TwoWire *rmI2C;

  float rmAHT21OldTemp;
  float rmAHT21OldHum;
  int rmAHT21State;
  unsigned long rmAHT21Timer;
  unsigned long rmAHT21MachineTimer;
  
};

#endif
