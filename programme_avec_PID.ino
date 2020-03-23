#include <TinyGPS++.h>
#include <SimpleTimer.h>
#include "NexButton.h"
#include <Nextion.h>
#include "NexDualStateButton.h"

#define SWETALON 42
#define PWMDISTRI 2

SimpleTimer timer;                 // Timer pour échantillonnage
const int _MOTEUR =  9;            // Digital pin pour commande moteur
unsigned int tick_codeuse = 0;     // Compteur de tick de la codeuse
int cmd = 0;                       // Commande du moteur
 
const int frequence_echantillonnage = 50;  // Fréquence du pid
const int rapport_reducteur = 29;          // Rapport entre le nombre de tours de l'arbre moteur et de la roue
const int tick_par_tour_codeuse = 32; // Nombre de tick codeuse par tour de l'arbre moteur
int PWM;
 
float consigne_moteur_nombre_tours_par_seconde = 5.;  //  Nombre de tours de roue par seconde
 
float erreur_precedente = consigne_moteur_nombre_tours_par_seconde;
float somme_erreur = 0;   // Somme des erreurs pour l'intégrateur
float kp = 300;           // Coefficient proportionnel
float ki = 5.5;           // Coefficient intégrateur
float kd = 100;           // Coefficient dérivateur

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

void gpsdata () {
   while (Serial1.available()) 
    {
         
        data = Serial1.read();
        gps.encode(data);
        
       
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          
          vitesse = gps.speed.kmph();
          nbre_sat = gps.satellites.value();
          heures = gps.time.hour();
           alt = gps.altitude.meters();
           minutes = gps.time.minute();
           vitessems = gps.speed.mps();
           utc1=heures+1;
           if (utc1==24)
           {
            utc1=0;
           }
        
    }
}
        
    
          void nextionend(){
Serial2.write(0xff);
Serial2.write(0xff);
Serial2.write(0xff);
      }

     void refreshdatascreen ()
{
       
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
}

void serialEvent1()
{
    gpsdata();
     
}

    void setup() 
    {
      
      Serial.begin(115200);
      Serial1.begin(9600);
      Serial2.begin(19200);
      pinMode(5, OUTPUT);
      pinMode(2, OUTPUT);
      pinMode(15, OUTPUT);
      pinMode(50, OUTPUT);
      pinMode(A10, OUTPUT);
      pinMode(SWETALON, INPUT);

      pinMode(_MOTEUR, OUTPUT);     // Sortie moteur
    analogWrite(_MOTEUR, 255);    // Sortie moteur à 0
 
    delay(5000);                  // Pause de 5 sec pour laisser le temps au moteur de s'arréter si celui-ci est en marche
 
    attachInterrupt(0, compteur, CHANGE);    // Interruption sur tick de la codeuse (interruption 0 = pin2 arduino mega)
    timer.setInterval(1000/frequence_echantillonnage, asservissement);  // Interruption pour calcul du PID et asservissement
       nexInit();
      turbinesw.attachPop(turbineswPopCallback, &turbinesw);

          }
  /* Interruption sur tick de la codeuse */
void compteur(){
    tick_codeuse++;  // On incrémente le nombre de tick de la codeuse
}

/* Interruption pour calcul du PID */
void asservissement()
{
    // Réinitialisation du nombre de tick de la codeuse
    int tick = tick_codeuse;
    tick_codeuse=0;
 
    // Calcul des erreurs
    int frequence_codeuse = frequence_echantillonnage*tick;
    float nb_tour_par_sec = (float)frequence_codeuse/(float)tick_par_tour_codeuse/(float)rapport_reducteur;
    float erreur = consigne_moteur_nombre_tours_par_seconde - nb_tour_par_sec;
    somme_erreur += erreur;
    float delta_erreur = erreur-erreur_precedente;
    erreur_precedente = erreur;
 
    // PID : calcul de la commande
    cmd = kp*erreur + ki*somme_erreur + kd*delta_erreur;
 
    // Normalisation et contrôle du moteur
    if(cmd < 0) cmd=0;
    else if(cmd > 255) cmd = 255;
    analogWrite(_MOTEUR, 255-cmd);

}

void loop() {

      
if (digitalRead(SWETALON) == HIGH){
  int valeur = analogRead(A10);
  
PWM= map(valeur, 100, 1023, 0, 255);

  analogWrite(3, PWM);
  
}
else 
{

refreshdatascreen ();
nexLoop(nex_Listen_List);
}

    }
  

