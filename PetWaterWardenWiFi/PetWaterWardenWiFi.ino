/************************************************
  * Author: Eloy Salinas
  * Project: Pet Water Warden
  * Company: Make Magazine // RadioShack
  * Date: 8/8/13
  * Version: 1.0
  * http://makezine.com/projects/petwaterwarden/
  *************************************************/
  //Twitter capabilities thanks to http://arduino-tweet.appspot.com/
  //Wifi Shield Twitter thanks to https://github.com/gauravg11/arduinolibs
 
  #include <SPI.h>
  #include <WiFi.h>
  #include <Twitter.h>
 
  //Twitter info
  // Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
  Twitter twitter("Your-Token-Here");
 
  // Message to post to twitter in case of a fail
  char msg[] = "Pet Water Warden Error: Please check my water!";
 
  //Network information
  char ssid[] = "SSID";     //  your network SSID (name)
  char pass[] = "Password";  // your network password
  int status = WL_IDLE_STATUS;     // the Wifi radio's status
 
  //Set output pin 8 to control switchtail/pump
  const int pump = 8;
 
  //Failsafe off
  long timerA = 0;
 
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
 
  //Set pump output pin
  pinMode(pump, OUTPUT);
 
  //Start with pump off
  digitalWrite(pump, LOW);
 
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while(true);
  }
 
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:    
    status = WiFi.begin(ssid, pass);
   
    // wait 10 seconds for connection:
    delay(10000);
  }
 
  printWifiData();
 
 
 
  //Test Twitter
  Serial.println("Pet Water Warden testing twitter connection...");  
 
  if (twitter.post("Pet Water Warden is up and running!!")) {  // Twitter that we are up and running
    int status = twitter.wait(&Serial);     // wait for a response from twitter                    
    if (status == 200) {    // if Twitter responds 200
      Serial.println(", Tweet is OK!");  // print success
      Serial.println();  // print a blank line, used for debugging
    }
    else {
      Serial.print("Tweet failed : code ");
      Serial.println(status);  // print error code
      Serial.println();  // print a blank line, used for debugging
    }
  }
  else {
    Serial.println("Connection to Twitter failed.");
  }
 
}
 
 
void loop(){
 
  //Read the input on A0-A1
  //High and Low Sensors
  int sensorLow = analogRead(A1);
  int sensorHigh = analogRead(A0);
 
  //Convert to a voltage
  float voltageLow = sensorLow * (5.0 / 1023.0);
  float voltageHigh = sensorHigh * (5.0 / 1023.0);  
 
  //Sensor States
  int lowState = 0;
  int highState = 0;
 
  //Are the sensors on or off?
  //Write states, Active Low
  if (voltageLow >= 3.5){lowState = 0;}
  else if (voltageLow < 3.5){lowState = 1;}
 
  if (voltageHigh >= 3.5){highState = 0;}
  else if (voltageHigh < 3.5){highState = 1;}
 
  //Turn on the pump?
  if(highState == 1 && lowState == 1){
    digitalWrite(pump, LOW);
    timerA = 0;  
  }else if(highState == 0 && digitalRead(pump) == LOW){
    //FailSafe Timers
    timerA = 0;
    digitalWrite(pump, HIGH);
    timerA = millis();
    Serial.print("Starting timer: ");    
    Serial.println(timerA);
  }
 
  //My pet bowl fills in about 45 sec, adjust to the size
  // of your pet bowl
  if( (millis() - timerA) >= 45000 && timerA != 0){
    digitalWrite(pump, LOW);  
    Serial.println(timerA);
    Serial.println(millis());
    timerA = 0;
    //Either no water left or the pump didn't turn off, bad sensor?
    tweetFail();
    Serial.println("Either no water left or the pump didn't turn off, bad sensor?");
  }
 
 
  //Debug Prints
  Serial.print("Low Sensor: ");
  Serial.println(lowState);
  Serial.print("High Sensor: ");
  Serial.println(highState);
 
  //Check Sensors 10 sec
  delay(10000);
 
 
}
 
 
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
}
 
void tweetFail(){
    if (twitter.post(msg)) {  // Twitter that we are up and running
    int status = twitter.wait(&Serial);     // wait for a response from twitter                    
    if (status == 200) {    // if Twitter responds 200
      Serial.println(", Tweet is OK!");  // print success
      Serial.println();  // print a blank line, used for debugging
    }
    else {
      Serial.print("Tweet failed : code ");
      Serial.println(status);  // print error code
      Serial.println();  // print a blank line, used for debugging
      //If Tweet fails try again
      tweetFail();
    }
  }
  else {
    Serial.println("Connection to Twitter failed.");
  }
}
