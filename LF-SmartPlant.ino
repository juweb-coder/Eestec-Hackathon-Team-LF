//---------------------------------------//
//SMARTPLANT - TEAM LF - EESTEC HACKATHON//
//---------------------------------------//


//---------------------------------------------------------------------------------------------//
//LIBRARIES//

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <dht.h>
#include <EEPROM.h>
dht DHT;
//---------------------------------------------------------------------------------------------//




//---------------------------------------------------------------------------------------------//
//WIFI AND WEBSERVER//
const int RX = 0;
const int TX = 1;
String AP = "SmartPlant";       // AP NAME
String PASS = "SaveTheWorld"; // AP PASSWORD
String API = "GUP87Z8AK71MPWLR";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
SoftwareSerial esp8266(RX,TX); 
//---------------------------------------------------------------------------------------------//
//



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
bool configurationFinished;
int eepromOffset = 1;
char udp_packet[3];
//---------------------------------------------------------------------------------------------//   




//----------------------------------------VOID SETUP-------------------------------------------//
//---------------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(9600);

//---------------------------------------------------------------------------------------------//
//WIFI AND WEBSERVER//
configurationFinished = EEPROM.read(0); //check if already was configurated at EEPROM address 0 (first byte)

if(configurationFinished){ //if was already configured before
  int newStr1AddrOffset = readStringFromEEPROM(eepromOffset, &AP); //read configured wifi data from EEPROM memory
  int newStr2AddrOffset = readStringFromEEPROM(newStr1AddrOffset, &PASS);
  int newStr3AddrOffset = readStringFromEEPROM(newStr2AddrOffset, &API);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");  //set station-mode
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}else{ //if first time configuration 
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=2",5,"OK");  //set access-point-mode
  sendCommand("AT+CWSAP=\""+ AP +"\",\""+ PASS +"\",\""+ 5 +"\",\""+ 3 +"\"",20,"OK"); //set SSID and PW for Access-Point
}

//---------------------------------------------------------------------------------------------//

} //end setup
//----------------------------------------END SETUP--------------------------------------------//
//---------------------------------------------------------------------------------------------//




//----------------------------------------VOID LOOP--------------------------------------------//
//---------------------------------------------------------------------------------------------//
void loop() {


if(configurationFinished == false){ //if was not already configured and now is acting as access point for receiving data
   if(esp8266.available()){
      for (int k = 0; k < 2; k++)
  {
  udp_packet[k] = esp8266.read(); //waiting for router SSID and Password data being submitted by user (via Access-Point connection)
  }
  AP = udp_packet[0]; 
  PASS = udp_packet[1]; 
  API = udp_packet[2]; 
  configurationFinished = true;
  }
}else{
  if(EEPROM.read(0) == false){ //if configuration wasn't already completed in the loops before
  int str1AddrOffset = writeStringToEEPROM(eepromOffset, AP); //write WIFI and API (Server) data to EEPROM if power is interrupted the data will be still available
  int str2AddrOffset = writeStringToEEPROM(str1AddrOffset, PASS);
  int str3AddrOffset = writeStringToEEPROM(str2AddrOffset, API);
  sendCommand("AT+CWMODE=1",5,"OK");  //set station-mode
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  EEPROM.write(0,1); //write first byte in EEPROM true to let the board know if it was alreasy configurated after power loss
  }
}

//---------------------------------------------------------------------------------------------//
//TIMER//

time = millis() - previousTime;
  
if(time >= checkIntervall) { //after time which is set (standard: 10min)
  previousTime = time; 
  startMeasuring();
  startSending();
} else if(checkIntervall < 0) { //after time overflow 
  previousTime = 0;
} else if(startup) { //if started up  measure sensor values one time directly
  startMeasuring();
  startSending();
}
//---------------------------------------------------------------------------------------------//


} //end loop
//--------------------------------------------END LOOP-----------------------------------------//
//---------------------------------------------------------------------------------------------//




//----------------------------------VOID STARTMEASURING----------------------------------------//
//---------------------------------------------------------------------------------------------//
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
//------------------------------------END STARTMEASURING---------------------------------------//
//---------------------------------------------------------------------------------------------//




//-----------------------------------VOID STARTSENDING-----------------------------------------//
//---------------------------------------------------------------------------------------------//
void startSending(){
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
}
//-----------------------------------END STARTSENDING------------------------------------------//
//---------------------------------------------------------------------------------------------//




//-----------------------------------VOID SENDCOMMAND------------------------------------------//
//---------------------------------------------------------------------------------------------//
void sendCommand(String command, int maxTime, char readReplay[]) {

  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);
    if(esp8266.find(readReplay))
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
 //-----------------------------------END SENDCOMMAND-------------------------------------------//
 //---------------------------------------------------------------------------------------------//



 
//------------------------------------INT WRITESTRINGTOEEPROM----------------------------------//
//------------------------------------INT READSTRINGFROMEEPROM---------------------------------//
int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
int newStrLen = EEPROM.read(addrOffset);
char data[newStrLen + 1];
for (int i = 0; i < newStrLen; i++)
{
data[i] = EEPROM.read(addrOffset + 1 + i);
}
data[newStrLen] = '\0';
*strToRead = String(data);
return addrOffset + 1 + newStrLen;
}
//------------------------------------END WRITESTRINGTOEEPROM----------------------------------//
//------------------------------------END READSTRINGFROMEEPROM---------------------------------//
