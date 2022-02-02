#include <Wire.h>
#include <AHT21.h>

AHT21 sensorAHT21(Wire);

void setup(void) 
{
  Serial.begin(115200);

  Wire.begin();

  sensorAHT21.begin();

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
    Serial.print("Humidity: ");
    Serial.println(hum);
  }
  else
    Serial.println("Failed to read AHT21");
  
  delay(100);
}
