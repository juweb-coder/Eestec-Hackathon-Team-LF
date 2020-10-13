//------------------------------//
//TEAM LF - EESTEC HACKATHON//
//------------------------------//


//---------------------------------------------------------------------------------------------//
//LIBRARIES//

#include <Arduino.h>
#include <dht.h>
dht DHT;
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//CAPACITIVE MOISTURE SENSOR//

const int moistureSensorPin = A0; // Analog pin sensor is connected to
const int dry = 600; //calibration Values for the sensor min/max 
const int wet = 250; //calibration Values for the sensor min/max 
int moistureSensorValue = 0; //somewhere to store the value read from the soil moisture sensor
int moisturePercentage = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//TEMPERATURE AND HUMIDITY SENSOR//

const int tempHumiditySensorPin = 8; // digital pin sensor is connected to
int readTempHumiditySensor = 0; //somewhere to store the value read from the temperature and humidity sensor
float temperatureFloat = 0.0;
float humidityFloat = 0.0;
int temperatureInt = 0;
int humidityInt = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//LIGHT SENSOR//

const int lightSensorPin = A1; // analog pin sensor is connected to
const int light = 250; //calibration Values for the sensor min/max 
const int dark = 50; //calibration Values for the sensor min/max 
int lightSensorValue = 0; //somewhere to store the value read from the soil moisture sensor
int lightPercentage = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//NPN TRANSISTOR//

const int npnTransistor = 6; // digital pin transistor is connected to

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//TIMER//

long checkIntervall = 600000; //after 10mins
long time;
unsigned long previousTime = 0;

//---------------------------------------------------------------------------------------------//                  


//---------------------------------------------------------------------------------------------//
//GLOBAL VARIABLES//

bool startup = true;

//---------------------------------------------------------------------------------------------//   

  
void setup() {
  Serial.begin(9600);

} //end setup




void loop() {

//---------------------------------------------------------------------------------------------//
//TIMER//

time = millis() - previousTime;
  
if(time >= checkIntervall) { //after time which is set
  previousTime = time; 
  startMeasuring();
} else if(checkIntervall < 0) { //after time overflow 
  previousTime = 0;
} else if(startup) { //if started up  measure sensor values one time directly
  startMeasuring();
}
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//WIFI MODULE//
/*used Data to transfer:
 * Moisture: moisturePercentage
 * Temperature: temperatureInt
 * Humidity: humidityInt
 * Light: lightPercentage
 */

//---------------------------------------------------------------------------------------------//

}



void startMeasuring() {
  
  digitalWrite(npnTransistor, HIGH); //turn on Transistor to power up sensors
  delay(10000); //delay to start up sensors 
  
//---------------------------------------------------------------------------------------------//
//CAPACITIVE MOISTURE SENSOR//

  
moistureSensorValue = analogRead(moistureSensorPin);

moisturePercentage = map(moistureSensorValue, wet, dry, 100, 0);

if (moisturePercentage >= 100)
{
    moisturePercentage = 100;
} 
else if (moisturePercentage <= 0)
{
  moisturePercentage = 0;
} 
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//TEMPERATURE AND HUMIDITY SENSOR//

  
readTempHumiditySensor = DHT.read11(tempHumiditySensorPin); //read DHT11 Sensor Signal
temperatureFloat = DHT.temperature;
humidityFloat = DHT.humidity;
temperatureInt = (int)temperatureFloat; //typecat float to int
humidityInt = (int)humidityFloat; //typecat float to int

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//LIGHT SENSOR//

  
lightSensorValue = analogRead(lightSensorPin);

lightPercentage = map(lightSensorValue, dark, light, 0, 100);

if (lightPercentage >= 100)
{
    lightPercentage = 100;
} 
else if (lightPercentage <= 0)
{
  lightPercentage = 0;
} 
//---------------------------------------------------------------------------------------------//

digitalWrite(npnTransistor, LOW); //turn off Transistor to save energy
startup = false;
                           
} //end loop
