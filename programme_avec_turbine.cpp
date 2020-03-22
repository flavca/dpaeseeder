#include <TinyGPS++.h>
#include "NexButton.h"
#include <Nextion.h>
#include "NexDualStateButton.h"

    char data;
    double latitude;
    double longitude;
    double alt; //altitude
    double vitesse;
    double heures;
    double minutes;
    unsigned long nbre_sat;
    double vitessems;
    double doseha;
    double utc1;

    
NexDSButton turbinesw = NexDSButton(0, 5, "turbinesw");


    NexTouch *nex_Listen_List[] = 
{
    &turbinesw,
    NULL
};
     
    TinyGPSPlus gps;

    void turbineswPopCallback(void *ptr)
{    
        uint32_t number5 = 0;  // Create variable to store value we are going to get
  turbinesw.getValue(&number5);  // Read value of dual state button to know the state (0 or 1)

  if(number5 == 1){  // If dual state button is equal to 1 (meaning is ON)...
    digitalWrite(5, HIGH);  // Turn ON internal LED
  }else{  // Since the dual state button is OFF...
    digitalWrite(5, LOW);  // Turn OFF internal LED
  }

        
        
    
}
    
          void nextionend(){
Serial2.write(0xff);
Serial2.write(0xff);
Serial2.write(0xff);
      }
     
    void setup() 
    {
      //Initialize serial and wait for port to open:
      Serial1.begin(9600);
      Serial2.begin(9600);
      pinMode(5, OUTPUT);
       nexInit();
      turbinesw.attachPop(turbineswPopCallback, &turbinesw);
          }
     
    void loop() {
     
   
      while (Serial1.available()) {
         
        data = Serial1.read();
       
        gps.encode(data);
        
       // if (gps.location.isUpdated())
        //{
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          
          vitesse = gps.speed.kmph();
          nbre_sat = gps.satellites.value();
          heures = gps.time.hour();
           alt = gps.altitude.meters();
           minutes = gps.time.minute();
           vitessems = gps.speed.mps();
           utc1=heures+1;
     




        //}
      }
      String command1 = "speedtrac.txt=\""+String(vitesse,2)+"\"" ; 
Serial2.print(command1);
nextionend();
String command4 = "nbrsat.txt=\""+String(nbre_sat)+"\"" ; 
Serial2.print(command4);
nextionend();
String command5 = "hour.txt=\""+String(utc1)+"\"" ; 
Serial2.print(command5);
nextionend();
String command3 = "alt.txt=\""+String(alt,1)+"\"" ; 
Serial2.print(command3);
nextionend();
String command6 = "min.txt=\""+String(minutes)+"\"" ; 
Serial2.print(command6);
nextionend();
String command2 = "speedms.txt=\""+String(vitessems,2)+"\"" ; 
Serial2.print(command2);
nextionend();
nexLoop(nex_Listen_List);  
    }
