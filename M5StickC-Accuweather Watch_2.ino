#include <M5StickC.h>

#include <WiFi.h>
#include <NTPClient.h>
//github.com/taranais/NTPClient
#include <WiFiUdp.h>


#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

 

// Replace with your network credentials
const char* ssid     = "XXXXXXXXXXXXXXXXXXXX";
const char* password = "XXXXXXXXXXXXXXXXXXXXX";



//////////////////Variables para el accuweather 
const String endpoint = "http://dataservice.accuweather.com/currentconditions/v1/XXXXXXXXXXXXX.json?apikey=XXXXXXXXXXXXXXXXXXXXXXXXXX&language=es";
//const String key = "yourAPIkey";
const char* weathertext= " ";
const char* tiempo =0;
int temp =0;
bool hasprecipitation= 0;
long int epochtime =0;
const char* unidad_metrica_temp=" ";
bool isDayTime = 0;

const static String daysOfWeek[] = {
    "Domingo",
    "Lunes",
    "Martes",
    "Miercoles",
    "Jueves",
    "Viernes",
    "Sabado"
    
};

 

//funcion obtiene dia de la semana 
 String getDayOfWeekIndex(int d, int m, int y) {
   int n =0;
    n = ((14 - m) / 12);
    y = (y - n);
    m = (m + (12 * n) - 2);
    return daysOfWeek[((d + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7)];
}

 

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp; //dia
String timeStamp; //hora

 
/////////////////////////////////////////////////////////////////////SETUP
void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10, 0, 2);
  M5.Lcd.println("");


   

 ////////////////////////////////Conexion wifi y tiempo de zona

 // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7200);
/*Hora estándar:  UTC/GMT +1 hora
Horario de verano:  +1 hora----------->Compensación de uso horario corriente:  UTC/GMT +2 horas
Abreviatura de uso horario: CEST
*/ 


  ////////////////////////////////////////Fin tiempo de zona 

  /////////////////////////////Parsea  JSON a Array 
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    //http.begin(endpoint + key); //Specify the URL
     http.begin(endpoint); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode); //200 OK! http code
        Serial.println(payload); //payload = contenido

  //StaticJsonBuffer<5000> jsonBuffer;
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10) + 490;
  DynamicJsonBuffer jsonBuffer(capacity);

  //JsonObject& root = jsonBuffer.parseObject(payload);

JsonArray& root = jsonBuffer.parseArray(payload);

// Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
JsonObject& root_0 = root[0];
  
const char* root_0_LocalObservationDateTime = root_0["LocalObservationDateTime"]; // "2019-07-30T23:31:00+02:00"
long root_0_EpochTime = root_0["EpochTime"]; // 1564522260
const char* root_0_WeatherText = root_0["WeatherText"]; // "Mayormente despejado"
int root_0_WeatherIcon = root_0["WeatherIcon"]; // 34
bool root_0_HasPrecipitation = root_0["HasPrecipitation"]; // false
bool root_0_IsDayTime = root_0["IsDayTime"]; // false

JsonObject& root_0_Temperature_Metric = root_0["Temperature"]["Metric"];
float root_0_Temperature_Metric_Value = root_0_Temperature_Metric["Value"]; // 12.8
const char* root_0_Temperature_Metric_Unit = root_0_Temperature_Metric["Unit"]; // "C"
int root_0_Temperature_Metric_UnitType = root_0_Temperature_Metric["UnitType"]; // 17

JsonObject& root_0_Temperature_Imperial = root_0["Temperature"]["Imperial"];
int root_0_Temperature_Imperial_Value = root_0_Temperature_Imperial["Value"]; // 55
const char* root_0_Temperature_Imperial_Unit = root_0_Temperature_Imperial["Unit"]; // "F"
int root_0_Temperature_Imperial_UnitType = root_0_Temperature_Imperial["UnitType"]; // 18

const char* root_0_MobileLink = root_0["MobileLink"]; // "http://m.accuweather.com/es/es/vitoria-gasteiz/309383/current-weather/309383"
const char* root_0_Link = root_0["Link"]; // "http://www.accuweather.com/es/es/vitoria-gasteiz/309383/current-weather/309383"


  


  // Extract values
  Serial.println(root_0_LocalObservationDateTime);
  Serial.println(root_0_EpochTime);
  Serial.println(root_0_WeatherText);
  Serial.println(root_0_HasPrecipitation);
  Serial.println(root_0_Temperature_Metric_Value);
  Serial.println(root_0_Temperature_Metric_Unit);
  weathertext= root_0_WeatherText;
  tiempo = root_0_LocalObservationDateTime;
  epochtime =  root_0_EpochTime;
  temp = root_0_Temperature_Metric_Value;
  unidad_metrica_temp = root_0_Temperature_Metric_Unit;
  hasprecipitation= root_0_HasPrecipitation;
  isDayTime  = root_0_IsDayTime;
 
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
  




  
 
}
 ////////////////////////////////FIN SETUP

 
/////////////////////////////////////////////////////////////////////LOOP
void loop() {
  // put your main code here, to run repeatedly:

 
//////////////////////////////////////////////////////Formateo de Dia y hora
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  delay(1000);


  
       /* M5.Lcd.printf();
        M5.Lcd.print();
        M5.Lcd.println();*/ 
        
        ////////////////////////////////////////////////////////////////Visualizacion en pantalla LCD del M5StickC
  M5.Lcd.setCursor(0, 1);
    M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.print("Fecha: " ); M5.Lcd.println(dayStamp);  
  M5.Lcd.print("Hora: "); M5.Lcd.println(timeStamp);  

  M5.Lcd.setTextColor(TFT_RED);
  //M5.Lcd.println(dayStamp);  
  String diasemana = getDayOfWeekIndex(day(epochtime), month(epochtime),  year(epochtime));
  M5.Lcd.print(diasemana); M5.Lcd.setTextColor(TFT_ORANGE); M5.Lcd.printf(" Bat:%.2fV\r\n",M5.Axp.GetVbatData() * 1.1 / 1000); 
  M5.Lcd.setTextColor(TFT_BLUE);
  M5.Lcd.println(weathertext); 
  M5.Lcd.setTextColor(TFT_GREEN);
  M5.Lcd.printf("Temp: %02d",temp);  M5.Lcd.print(unidad_metrica_temp);  

   
    M5.Lcd.setTextColor(TFT_PURPLE);
if (hasprecipitation){
  M5.Lcd.print(" Llueve."); 
}else
{
  M5.Lcd.print(" No llueve."); 
  }
    M5.Lcd.setTextColor(TFT_BLACK);
if(isDayTime){
  M5.Lcd.print("Dia."); 
  }else{
  M5.Lcd.print("Noche.");  
    }


  
  delay(500);
  delay(1000*60*60); 
} 
/////////////////////////////////////////////////////////////////////FIN LOOP
