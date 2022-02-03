#include <Wire.h>
#include <AHT21.h>

AHT21 sensorAHT21(Wire);

void setup(void) 
{
  int rError;
  Serial.begin(115200);

  Wire.begin();

  rError = sensorAHT21.begin();

  if (rError)
  {
    Serial.println("Failed to initialize AHT21");
    delay(100000);
  }

}

void loop(void) 
{
  float temp, hum;
  int rError;

  rError = sensorAHT21.read(&temp, &hum);

  if(rError == 0)
  {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" Humidity: ");
    Serial.println(hum);
  }
  else
    Serial.println("Failed to read AHT21");
  
  delay(1000);
}
