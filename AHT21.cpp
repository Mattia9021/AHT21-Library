#include "AHT21.h"
#include <Wire.h>

//#define ENABLE_DEBUG_AHT21

/*! ---------------------------------------------------------------------------------------------
 *  @brief  AHT21 object
 */
AHT21::AHT21()
    : rmAHT21Address(AHT21_I2C_ADDRESS), rmI2C(&Wire) {}

/*! ---------------------------------------------------------------------------------------------
 *  @brief  AHT21 object
 *  @param  i2c Wire bus for communication (I2C)
 */
AHT21::AHT21(TwoWire &i2c)
    : rmAHT21Address(AHT21_I2C_ADDRESS), rmI2C(&i2c) {}

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Starts communication with the AHT21
 *  @param  pAddress Address I2C (Default 0x38)
 *  @return 0 if no error occurred
 */
int AHT21::begin(unsigned char pAddress) 
{
  int rError = 0;

  // Set start point for state machine
  rmAHT21State = 0;

  // Save address
  rmAHT21Address = pAddress;

  // Inialize sensor and read status
  rError = initialize();

  // Trigger for first measure
  if(rError == 0)
  {
    delay(10);
    rError = triggerMeasure();
  }

  // Get first measure
  if(rError == 0)
  {
    delay(AHT21_WAIT_TIME);
    rError = getMeasure();
  }

  return(rError);
}

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Read data from AHT21 Sensor
 *  @param  pTemp Get Humidity [°C]
 *  @param  pHum Get Humidity [%]
 *  @param  pForceRead Force reading from sensor (More slow)
 *  @return 0 if no error occurred
 */
int AHT21::read(float *pTemp, float *pHum, bool pForceRead) 
{
  int rError;

  if(pForceRead)
  {
    rError = triggerMeasure();

    if(rError == 0)
    {
      delay(AHT21_WAIT_TIME);
      rError = getMeasure();
    }    
  }
  else
  {
    switch (rmAHT21State)
    {
    case 0:
      
      rError = triggerMeasure();

      if(rError == 0)
      {
        rmAHT21MachineTimer = millis();
        rmAHT21State++;
      }
      break;

    case 1:

      if(millis() >= (rmAHT21MachineTimer + AHT21_WAIT_TIME))
        rmAHT21State++;
      break;

    case 2:
      
      rError = getMeasure();

      if(rError == 0)
        rmAHT21State = 0;
      break;            
    
    default:
      rmAHT21State = 0;
      break;

    }
  }

  *pTemp = rmAHT21OldTemp;
  *pHum = rmAHT21OldHum;

  return(0);
}

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Initalize AHT21 Sensor and read Status
 *  @return 0 if no error occurred
 */
int AHT21::initialize(void)
{
  char data;

  rmI2C->beginTransmission(rmAHT21Address);
  rmI2C->write(0x71);
  rmI2C->endTransmission();

  rmI2C->requestFrom((unsigned char)rmAHT21Address, (unsigned char)1);

  if(rmI2C->available())
    data = rmI2C->read();
  else
    return(1);

  // Status must be 0x18
  if((data & 0x18) != 0x18)
    return(2);

  return(0);
}

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Trigger AHT21 Sensor (after this must wait 80 ms for read data)
 *  @return 0 if no error occurred
 */
int AHT21::triggerMeasure(void)
{
  int rError;

  // Trigger Read
  rmI2C->beginTransmission(rmAHT21Address);
  rmI2C->write(0xAC);
  rmI2C->write(0x33);
  rmI2C->write(0x00);
  rError = rmI2C->endTransmission();

  // Start Timer 
  rmAHT21Timer = millis();

  return(rError);
} 

/*! ---------------------------------------------------------------------------------------------
 *  @brief  Read AHT21 Sensor
 *  @return 0 if no error occurred
 */
int AHT21::getMeasure(void)
{
  int i = 0;
  char data[7];

  if(millis() >= (rmAHT21Timer + AHT21_MEASURE_TIME))
  {
    rmI2C->requestFrom( rmAHT21Address, (unsigned char)7);

    while(rmI2C->available())    // slave may send less than requested
    {
      data[i] = rmI2C->read();    // receive a byte as character

      #ifdef ENABLE_DEBUG_AHT21
        Serial.print("Received Byte: 0x"); Serial.println(data[i], HEX);
      #endif 

      i++;
    }

    // Status check bit [7] Busy indication
    if((data[0] & 0x80) == 0)
    {
      unsigned long rRawHum = (data[1] << 12) | (data[2] << 4) | (data[3] >> 4 );
      unsigned long rRawTemp = ((data[3] & 0xF) << 16) |  (data[4] << 8) | (data[5]);

      #ifdef ENABLE_DEBUG_AHT21
        Serial.print("RawTemp: 0x"); Serial.println(rRawTemp, HEX);
        Serial.print("RawHum: 0x"); Serial.println(rRawHum, HEX);
      #endif  

      // Data conversion   2^20 = 1048576
      float rTemp = (((float)rRawTemp / 1048576) * 200 ) - 50;
      float rHum = (((float)rRawHum / 1048576) * 100 );

      rmAHT21OldTemp = rTemp;
      rmAHT21OldHum = rHum;

      #ifdef ENABLE_DEBUG_AHT21
        Serial.print("Temp: "); Serial.println(rTemp);
        Serial.print("Hum: "); Serial.println(rHum);
      #endif
    }
    else
    {
      #ifdef ENABLE_DEBUG_AHT21
        Serial.println("AHT21 is busy!");
      #endif

      return(1);
    }
  }
  else
  {
    #ifdef ENABLE_DEBUG_AHT21
      Serial.println("AHT21 timer not expired!");
    #endif
  }
  
  return(0);
}
