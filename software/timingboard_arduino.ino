// This is the code for the timingboard of the distributed ground station network.
// It will track and communicate with CubeSats. And with you, the creative community,
// we can do much, much more crazy things with it! :)
// It allows the correlation of received signal with the current gps-time and further
// gps information. The timinboard provides the gps-time and the gps information and
// injects provides it as an output signal to be used with the node-io receiver.
// http://www.aerospaceresearch.net
// https://github.com/aerospaceresearch/

// It is tested with Adafruit GPS modules using MTK3329/MTK3339 driver
//    ------> http://www.adafruit.com/products/746

// The code is intended for use with Arduino Leonardo and other ATmega32U4-based Arduinos

// install Adafruit_GPS.h and Time.h via the arduino IDE...
#include <Adafruit_GPS.h>
#include <Time.h>
#include <SoftwareSerial.h>

// configurations of the pins
int pin = 13;
int resetPin = 12;

// configuration of what pin is what
// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using software serial (sketch example default):
//   Connect the GPS TX (transmit) pin to Digital 8
//   Connect the GPS RX (receive) pin to Digital 7
// If using hardware serial:
//   Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
//   Connect the GPS RX (receive) pin to matching TX1 (Digital 1)

// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
//SoftwareSerial mySerial(8, 7);
//Adafruit_GPS GPS(&mySerial);

// If using hardware serial, comment
// out the above two lines and enable these two lines instead:
Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true

void setup()  
{
  digitalWrite(resetPin, HIGH);
  delay(200);
  pinMode(resetPin, OUTPUT); 
  
  attachInterrupt(0, blink, RISING); // 0 is pin 3
  pinMode(13, OUTPUT); // led
  
  
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  
  Serial.println("reset");//print reset to know the program has been reset and 
  //the setup function happened
  delay(200);
}

uint32_t timer = millis();
long unixtime;
float unixday;
float unixweek;
long secondsofweek;
// january 1, 1970 was a thursday
String unixweek_str;
String secondsofweek_str;

int trigger = 0;
long triggered;

long steps = 86400000;
long timerset = steps;

void loop()                     // run over and over again
{
  //if(trigger == 1){
    // for gps 1pps
    //delayMicroseconds(5);
    //digitalWrite(pin, LOW);
  //}
  
  if(trigger == 1 && micros()-triggered >= 100){
   trigger = 0;
   //Serial.println(micros()-triggered);
   //Serial.println(round(GPS.seconds+GPS.milliseconds/1000.0));
   float t1t;
   t1t = round(GPS.seconds+GPS.milliseconds/1000.0);
   setTime(GPS.hour,GPS.minute,round(GPS.seconds+GPS.milliseconds/1000.0),GPS.day,GPS.month,GPS.year);
   
   unixtime = now();
   //Serial.print(t1t);
   //Serial.print(" ");
   //Serial.println(unixtime);
   //unixday = unixtime / (24.0*3600.0);
   //unixweek = unixday / 7.0;
   //secondsofweek = unixtime - long(unixweek)*7*24*3600;
   //Serial.println(int(unixweek), BIN);
   //Serial.println(secondsofweek);
   //unixweek_str = (do_binary(long(unixweek),13));
   String out_bitstring = "1101010101000101001001001011"+do_binary(unixtime,32);
   //secondsofweek_str = "1101010101000101001001001011"+do_binary(secondsofweek,20)+"1101";
   
   float floatVal= GPS.longitude;
   String stringVal = "";
   stringVal+=String(int(floatVal))+ "."+String(getDecimal(floatVal)); //combining both whole and decimal part in string with a full                                                                      //stop between them
   //Serial.print("stringVal: ");Serial.println(stringVal);              //display string value
   int index_dot = stringVal.indexOf('.');
   long longitidue_long_minute;
   longitidue_long_minute = longitidue_long_minute + long(stringVal.substring(0,index_dot-2).toInt()) * 60;
   longitidue_long_minute = longitidue_long_minute + long(stringVal.substring(index_dot-2,index_dot).toInt());
   long longitidue_long_second = long(stringVal.substring(index_dot+1,index_dot+1+5).toInt());
   //long longitidue_long_second = long(stringVal.substring(index_dot+1,stringVal.length()).toInt());
   //Serial.println(longitidue_long_second);
   
   floatVal= GPS.latitude;
   stringVal = "";
   stringVal+=String(int(floatVal))+ "."+String(getDecimal(floatVal)); //combining both whole and decimal part in string with a full                                                                      //stop between them
   //Serial.print("stringVal: ");Serial.println(stringVal);              //display string value
   index_dot = stringVal.indexOf('.');
   
   long latitdue_long_minute;
   latitdue_long_minute = latitdue_long_minute + long(stringVal.substring(0,index_dot-2).toInt()) * 60;
   latitdue_long_minute = latitdue_long_minute + long(stringVal.substring(index_dot-2,index_dot).toInt());
   long latitdue_long_second = long(stringVal.substring(index_dot+1,index_dot+1+5).toInt());
   //long latitdue_long_second = long(stringVal.substring(index_dot+1,stringVal.length()).toInt());
   //Serial.println(latitdue_long_second);
   
   String geosector = String(GPS.lon);
   if(geosector.equals("E")){
     // is plus
     //Serial.println("E0");
     out_bitstring = out_bitstring + "0";
   } else{
     // is minus
     //Serial.println("W1");
     out_bitstring = out_bitstring + "1";
   }
   //Serial.println(longitidue_long_minute);
   //Serial.println(longitidue_long_minute);
   out_bitstring = out_bitstring + do_binary(longitidue_long_minute, 14);
   //Serial.println(longitidue_long_second);
   out_bitstring = out_bitstring + do_binary(longitidue_long_second, 17);
   geosector = String(GPS.lat);
   if(geosector.equals("N")){
     //Serial.println("N");
     // is plus
     out_bitstring = out_bitstring + "0";
   } else{
     // is minus
     out_bitstring = out_bitstring + "1";
     //Serial.println("S");
   }
   out_bitstring = out_bitstring + do_binary(latitdue_long_minute, 13);
   out_bitstring = out_bitstring + do_binary(latitdue_long_second, 17);
   
   float altitude_offset = 1000; // deepest place on land about -400m below sea level
   long altitude_decim = long((GPS.altitude + altitude_offset) * 10);
   //Serial.println(altitude_decim);
   out_bitstring = out_bitstring + "1101" + do_binary(altitude_decim, 19);
   
   out_bitstring = out_bitstring + do_binary((int)(GPS.HDOP*100.0), 11);
   out_bitstring = out_bitstring + do_binary(GPS.fixquality, 4);
   out_bitstring = out_bitstring + do_binary(GPS.satellites, 7);
   
   //Serial.print((GPS.HDOP)); Serial.print(" "), Serial.println(do_binary((int)(GPS.HDOP*100.0), 11));
   //Serial.print("Fix: "); Serial.print(do_binary(GPS.fix, 1));
   //Serial.print(" quality: "); Serial.println(do_binary(GPS.fixquality, 4));
   //Serial.print("Satellites: "); Serial.println(do_binary(GPS.satellites, 7));
   
   morseout(out_bitstring);
   
   //Serial.print(out_bitstring.length());Serial.print("=>>");Serial.println(out_bitstring);
  }
  
  
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    //Serial.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 10000) { 
    timer = millis(); // reset the timer
    //morseout("1");
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    Serial.println(GPS.latitude, 6);
    Serial.println(timerset);
    Serial.println(millis());
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
    }
  }
  
  if (millis() > timerset) {
    
    Serial.println("resetting possible");
    Serial.println(timerset);
    timerset = timerset + steps;
    delay(10);
    digitalWrite(resetPin, LOW);
  }
}

String number;
String zeros;
String do_binary(long input, int bitlength){
  number = String(input, BIN);
  int i = 0;
  zeros = "";
  for(i=0; i<bitlength-number.length(); i++){
    zeros = zeros + "0";
  }
  return zeros+number;
}

int bitduration = 8;
String one = "";
int morseout(String trackme){ 
  int i = 0;
    for (i=0; i<trackme.length(); i++){
      one = String(trackme[i]);
      if(one.equals("1")){
        digitalWrite(pin, HIGH);
        delayMicroseconds(bitduration);
        digitalWrite(pin, LOW);
        //Serial.print("100");
        delayMicroseconds(bitduration);
      } else{
        digitalWrite(pin, LOW);
        delayMicroseconds(bitduration);
        digitalWrite(pin, LOW);
        //Serial.print("100");
        delayMicroseconds(bitduration);
      }
      
      delayMicroseconds(10);
    }
    return 1;
}


void blink()
{
  //digitalWrite(pin, HIGH); // for the gps 1pps
  triggered = micros();
  trigger = 1;
}

//function to extract decimal part of float
long getDecimal(float val)
{
 int intPart = int(val);
 long decPart = 10000000*(val-intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places
                                   //Change to match the number of decimal places you need
 if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
 else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
 else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
}
