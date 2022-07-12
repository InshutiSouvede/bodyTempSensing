//Libraries
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

// Needed variables
#define REDPIN 14
#define GREENPIN 16
#define TEMPVOUT A0
#define BUZZPIN 12

int lcdColumns = 16;
int lcdRows = 2;
WiFiClient wifiClient;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {

    Serial.begin(9600);

    Serial.println("================================");
    Serial.println("===== BODY TEMPERATURE =====");
    Serial.println("================================");
    
    // LEDs
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BUZZPIN, OUTPUT);
    // LCD
    lcd.begin(5,4);
    lcd.init();
    lcd.backlight();    

    // SETUP WiFi
     WiFi.begin("RCA-WiFi", "rca@2019");
}

void loop() {

  // Write to LCD
  
  lcd.setCursor(0,0);
  lcd.print("BODY TEMPERATURE");
  delay(1000);

  // Read Temperature Meter
  int analogValue = analogRead(TEMPVOUT);
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
//  float celsius = millivolts*100;
  float celsius = millivolts/10;

  Serial.print("Degrees RED=   ");
  Serial.println(celsius);

  String mData={};
  String dummy_device = "340722SPE0342022";
  
  if(celsius > 35 ){
    // Turn on RED LED
    digitalWrite(REDPIN, HIGH);
    // Turn off GREEN LED
    digitalWrite(GREENPIN, LOW);  
      
      
//    mData = "device="+dummy_device+"&temperature="+(String)celsius;
    mData = "device="+dummy_device+"&distance="+(String)celsius;
    sendData(80, "192.168.1.150", "/iot/" , mData); 
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Not Normal"+(String)celsius); 
    
    digitalWrite(BUZZPIN, HIGH);  
     delay(1000);
    digitalWrite(BUZZPIN, LOW);
    

    
  }else{
    digitalWrite(BUZZPIN, LOW);
    // Turn on RED LED
    digitalWrite(REDPIN, LOW);
    // Turn off GREEN LED
    digitalWrite(GREENPIN, HIGH);

//    mData = "device="+dummy_device+"&temperature="+(String)celsius;
  mData = "device="+dummy_device+"&distance="+(String)celsius;
    
    
    sendData(80, "192.168.1.150", "/iot/" , mData);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Normal "+(String)celsius);   
  }

  delay(1000);

}


void sendData(const int httpPort, const char* host,const char* filepath , String data){

  
  
  wifiClient.connect(host, httpPort); 
  wifiClient.println("POST "+(String)filepath+" HTTP/1.1");
  wifiClient.println("Host: " + (String)host);
  wifiClient.println("User-Agent: ESP8266/1.0");
  wifiClient.println("Content-Type: application/x-www-form-urlencoded");
  wifiClient.println("Content-Length: " +(String)data.length());
  
  wifiClient.println();
  wifiClient.print(data);

  Serial.println("Response: " + wifiClient.readStringUntil('\n'));
  Serial.println("Sent data: " + data);

}
