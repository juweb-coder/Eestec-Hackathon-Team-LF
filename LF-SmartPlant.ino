//---------------------------------------//
//SMARTPLANT - TEAM LF - EESTEC HACKATHON//
//---------------------------------------//


//---------------------------------------------------------------------------------------------//
//LIBRARIES//

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <dht.h>
dht DHT;
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//WIFI AND WEBSERVER//
const int RX = 0;
const int TX = 1;
String AP = "FritzBox7490";       // AP NAME
String PASS = "password"; // AP PASSWORD
String API = "GUP87Z8AK71MPWLR";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//CAPACITIVE MOISTURE SENSOR//

const int moistureSensorPin = A1; // Analog pin sensor is connected to
const int dry = 600; //calibration Values for the sensor min/max 
const int wet = 250; //calibration Values for the sensor min/max 
int moistureSensorValue = 0; //somewhere to store the value read from the soil moisture sensor
int moisturePercentage = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//TEMPERATURE AND HUMIDITY SENSOR//

const int tempHumiditySensorPin = 2; // digital pin sensor is connected to
int readTempHumiditySensor = 0; //somewhere to store the value read from the temperature and humidity sensor
float temperatureFloat = 0.0;
float humidityFloat = 0.0;
int temperatureInt = 0;
int humidityInt = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//LIGHT SENSOR//

const int lightSensorPin = A0; // analog pin sensor is connected to
const int light = 250; //calibration Values for the sensor min/max 
const int dark = 50; //calibration Values for the sensor min/max 
int lightSensorValue = 0; //somewhere to store the value read from the soil moisture sensor
int lightPercentage = 0;

//---------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------//
//NPN TRANSISTOR//

const int npnTransistor = 4; // digital pin transistor is connected to

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

//---------------------------------------------------------------------------------------------//
//WIFI AND WEBSERVER//
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
//---------------------------------------------------------------------------------------------//

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
 //used Data to transfer:
 // Moisture: moisturePercentage
 //Temperature: temperatureInt
 //Humidity: humidityInt
 // Light: lightPercentage

 String getData = "GET /update?api_key="+ API +"&field1="+moisturePercentage+"&field2="+temperatureInt+"&field3="+humidityInt+"&field4="+lightPercentage;
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 

//---------------------------------------------------------------------------------------------//

} //end loop


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
                           
} //end measuring function



void sendCommand(String command, int maxTime, char readReplay[]) {
/*  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" "); */
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 } //end sendCommand function
