/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
/* Ethernet libraries */
#include <SPI.h>
#include <Ethernet.h>

#define __USE_DHCP__ 1
// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network

// for static IP users.
IPAddress ip(222, 98, 173, 232);
IPAddress gateway(222, 98, 173, 254);
IPAddress subnet(255, 255, 255, 192);
// fill in your Domain Name Server address here:
IPAddress myDns(8, 8, 8, 8); // google puble dns

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "GQDPH4QJKXKB37TI";
String writeAPIKey = "FFMJBK2396TA8B1D";
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

// error at 1.6.4 IDE, so add declaration of function.
void startEthernet();
void updateThingSpeak(String tsData);
void updateTwitterStatus(String tsData);

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*-----( Declare Variables )-----*/
//NONE

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);  // Used to type in characters
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight 
  startEthernet();  // Start Ethernet on Arduino

//-------- Write characters on the display ------------------
// NOTE: Cursor Position: (CHAR, LINE) start at 0  
  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("Syrian Refugees");
  delay(300);
  lcd.setCursor(0,1);
  lcd.print("Route 1988-2013");
  delay(300);  

// Wait and then tell user they can start the Serial Monitor and type in characters to
// Display. (Set Serial Monitor option to "No Line Ending")
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("Countries & Refu");
  lcd.setCursor(0,1);
  lcd.print("gee population");  
}/*--(end setup )---*/


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  int i=0;
  int nRefugeeData;
  String sRefugeeData;
    // when characters arrive over the serial port...
      
      // Print Update Response to Serial Monitor
      if (client.available()) {
          char c = client.read();
          //Serial.print(
      }
  
      // Disconnect from ThingSpeak
      if (!client.connected() && lastConnected) {
          //Serial.println("...disconnected");
          //Serial.println();
          client.stop();
      }
  
      // Update ThingSpeak
      if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval)) {
         // Serial.println("field1="+analogValue0);
         // updateThingSpeak("field1="+analogValue0);
          // Update Twitter via ThingTweet
          if (Serial.available()) {
          // wait a bit for the entire message to arrive
          delay(100);
          // clear the screen
          lcd.clear();
          // read all the available characters
          while (Serial.available() > 0) {
              // display each character to the LCD
              
              //lcd.write(Serial.read()); 
              sRefugeeData = Serial.readString();
              lcd.print(sRefugeeData); 
              String tweetmsg = "Syrian Refugee Route and Population "+ sRefugeeData;
              tweetmsg += " @IxdGold";
              updateTwitterStatus(tweetmsg);     
               
            }     
          }       
      }
  
      // Check if Arduino Ethernet needs to be restarted
      if (failedCounter > 3 ) {
          startEthernet();
      }
      lastConnected = client.connected();

}/* --(end main loop )-- */

void startEthernet()
{
    client.stop();

    //Serial.println("Connecting Arduino to network...");
    //Serial.println();

    delay(1000);

    // Connect to network amd obtain an IP address using DHCP
    // initialize the ethernet device
#if defined __USE_DHCP__
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
    if (Ethernet.begin() == 0)
#else
    if (Ethernet.begin(mac) == 0)
#endif
    {
        //Serial.println("DHCP Failed, reset Arduino to try again");
        //Serial.println();
    } else {
        //Serial.println("Arduino connected to network using DHCP");
        //Serial.println();
    }
#else
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
    Ethernet.begin(ip, myDns, gateway, subnet);
#else
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
#endif
   // Serial.println("Arduino connected to network using Static IP : ");
    //Serial.println(Ethernet.localIP());
#endif

    delay(1000);
}

void updateThingSpeak(String tsData)
{
    if (client.connect(thingSpeakAddress, 80)) {
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(tsData.length());
        client.print("\n\n");

        client.print(tsData);

        lastConnectionTime = millis();

        if (client.connected()) {
            //Serial.println("Connecting to ThingSpeak...");
            //Serial.println();

            failedCounter = 0;
        } else {
            failedCounter++;

            //Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");
            //Serial.println();
        }

    } else {
        failedCounter++;

        //Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");
        //Serial.println();

        lastConnectionTime = millis();
    }
}


void updateTwitterStatus(String tsData)
{
    client.stop();
    if (client.connect(thingSpeakAddress, 80)) {
        // Create HTTP POST Data
        tsData = "api_key="+thingtweetAPIKey+"&status="+tsData;

        client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(tsData.length());
        client.print("\n\n");

        client.print(tsData);

        lastConnectionTime = millis();

        if (client.connected()) {
            //Serial.println("Connecting to ThingSpeak...");
            //Serial.println();

            failedCounter = 0;
        } else {
            failedCounter++;

            //Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");
            //Serial.println();
        }

    } else {
        failedCounter++;

        //Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");
        //Serial.println();

        lastConnectionTime = millis();
    }
}
/* ( THE END ) */


