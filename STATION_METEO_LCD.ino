

// Librarie capteur DHT: https://github.com/adafruit/DHT-sensor-library
// Librairie capteur Adafruit: https://github.com/adafruit/Adafruit_Sensor

// Humidite sol
//0-45 — completement sec
// 620 maximum humidite sol constate

// valeur tension obtenue direction vent : N = 887 - NE = 815 - E = 930 - SE = 865 - S = 946 - SO = 790 - O = 845 - NO = 750

//calibre mentionné dans le datasheet pluviometre : 0.2794 mm de pluie créé un contact digital


// ---------------------- CONNEXIONS ELECTRONIQUES---------------------------
 
// Capteur humidite et temperature
// Connecter pin 1 du capteur au +5V
// Connecter pin 2 du capteur a DHTPIN 
// Connect pin 3 a GND
// Connecter une resistance 10K de DHTPIN a +5V

// Capteur humidite sol
// Connecter pin + a pin allumage_captsol qui sert d'allumage intermittent du capteur (pour pb corrosion)
// Connecter pin - a GND
// Connect pin S au data = capteurhumsol

// Capteur photoresistane
// Connecter patte 1 a  pin analogique capteur_lum
// Connecter patte 1 a resistance 10K puis vers GND
// Connecter patte 2  a +5V

// Capteur direction du vent
// Connecter pin 1 (fil rouge) du capteur a capteur_direction
// Connecter pin 2 (fil vert) a GND
// Connecter pin 1 (fil rouge) du capteur vers resistance 10K puis +5V

// Capteur vitesse du vent
// Connecter pin 1 (fil jaune) du capteur a capteur_vitessevent
// Connecter pin 2 (fil vert) a +5V
// Connecter pin 1 (fil jaune) du capteur vers resistance 10K puis GND
// Connecter pin 1 (fil jaune) du capteur vers condensateur 100 nF puis GND pour eviter les rebonds

// Capteur pluviometrie
// Connecter pin 1 (fil rouge) du capteur a capteur_pluie
// Connecter pin 2 (fil vert) a +5V
// Connecter pin 1 (fil rouge) du capteur vers resistance 10K puis GND
// Connecter pin 1 (fil rouge) du capteur vers condensateur 100 nF puis GND pour eviter les rebonds


// ---------------------- DEFINITION VARIABLES ET BIBLIOTHEQUES ---------------------------
#include "DHT.h"
#include <LiquidCrystal.h>
#include <stdio.h> // pour fonction sprintf

#define DHTPIN A0  // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// declaration type de capteur DHT
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

float humidite_air;
float temperature;
float temp_ressentie;

const int capteur_humsol = A1;
const int allumage_captsol = 30; 
int mesurehumsol = 0;
float humsol = 0;
const int max_humsol = 620;

int capteur_lum = A2; 
int lum = 0; 

const int capteur_direction = A3;
int mesuredirection;
String direction_vent = "rien";
String nord = "Nord";
String nordest = "Nord-Est";
String est= "Est";
String sudest = "Sud-Est";
String sud = "Sud"; 
String sudouest = "Sud-Ouest";
String ouest = "Ouest";
String nordouest = "Nord-Ouest";

const int capteur_vitessevent = 1; //correspond a pin 3: capteur attacheinterupt sur une Uno ou Mega
const int tempsmesure = 3;      //en Secondes, pour capteur vitesse vent et pluvio, sert aussi de délai
int vent_ct = 0;
float vitessevent = 0.0;
unsigned long time = 0;
const float calibrevitessevent = 2.4;

const int capteur_pluie = 0; //correspond a pin 2: capteur attacheinterupt sur une Uno ou Mega
int pluie_ct = 0;           // compteur d'impulsion de pluie
float pluie = 0.0;          // valeur de pluviometrie
const float calibrepluie = 0.2974;

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7 ;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// ---------------------- SETUP ------------------------------------------------------

void setup() {
  
Serial.begin(9600);
Serial.print(F("Hello World!"));
Serial.println(F("Test du Capteur DHT!"));
dht.begin();

pinMode(allumage_captsol, OUTPUT);
digitalWrite(allumage_captsol, LOW);

time = millis();

lcd.begin(16, 2);

lcd.print("hello, world!");

}

// ---------------------- LOOP ------------------------------------------------------

void loop() {
// Delai

delay(2000);

// Lecture des capteurs
  
humidite_air = dht.readHumidity();
temperature = dht.readTemperature();

digitalWrite(allumage_captsol, HIGH);  
delay(10);
mesurehumsol = analogRead(capteur_humsol);
digitalWrite(allumage_captsol, LOW);

lum = map(analogRead(capteur_lum),0, 1023, 0, 100);

mesuredirection = analogRead(capteur_direction);

vent_ct = 0;
pluie_ct = 0;
time = millis();
attachInterrupt(capteur_vitessevent, compteurvent, RISING);
attachInterrupt(capteur_pluie, compteurpluie, RISING);

delay(1000 * tempsmesure);

detachInterrupt(capteur_vitessevent);
detachInterrupt(capteur_pluie);
vitessevent = (float)vent_ct / (float)tempsmesure * calibrevitessevent;
pluie = (float)pluie_ct / (float)tempsmesure * calibrepluie;

// vérification lectures
  
if (isnan(humidite_air) || isnan(temperature)) {
    Serial.println(F("Impossible de lire depuis le capteur temperature et humidite"));
    return;
}

//Calculs 

temp_ressentie = dht.computeHeatIndex(temperature, humidite_air, false); // false car pas de fahrenheit
  
humsol = map(mesurehumsol, 0, max_humsol, 0, 100);

if (mesuredirection > 880 && mesuredirection < 900) {
direction_vent = nord;
}
else if (mesuredirection > 800 && mesuredirection < 835) {
direction_vent = nordest;
}
else if (mesuredirection > 920 && mesuredirection < 937) {
direction_vent = est;
}
else if (mesuredirection > 856 && mesuredirection < 879) {
direction_vent = sudest;
}
else if (mesuredirection > 938 && mesuredirection < 970) {
direction_vent = sud;
}
else if (mesuredirection > 770 && mesuredirection < 799) {
direction_vent = sudouest;
}
else if (mesuredirection > 836 && mesuredirection < 855) {
direction_vent = ouest;
}
else if (mesuredirection > 720 && mesuredirection < 769) {
direction_vent = nordouest;
}

//Serial prints de Debug

//Serial.print("Valeur numérique luminosite ");
//Serial.print(analogRead(capteur_lum)); // lecture de la photoresistance
//Serial.print("valeur numérique humidite sol: " );
//Serial.print(mesurehumsol);
//Serial.print("valeur numérique direction: " );
//Serial.println(mesuredirection);

// Affichage serial Print des mesures
Serial.print(F("Humidite: "));
Serial.print(humidite_air,1);
Serial.print(F("%  Temperature: "));
Serial.print(temperature,1);
Serial.print(F("°C "));
Serial.print(F("Temperature ressentie: "));
Serial.print(temp_ressentie,1);
Serial.print(F("°C "));
Serial.print(F(" Humidite sol: " ));
Serial.print(humsol,1);
Serial.print(F("% "));
Serial.print(F(" Luminosite: "));
Serial.print(lum);
Serial.print(F("% "));
Serial.print(F(" Direction du vent: "));
Serial.print(direction_vent);
Serial.print(F(" Vitesse vent: "));
Serial.print(vitessevent,1);   
Serial.print(" km/h");
Serial.print(" Pluie: ");
Serial.print(pluie);   
Serial.println(" mm");

//Affichage LCD
lcd.clear();
lcd.setCursor(0, 0);
//lcd.print(temperature,1);
//lcd.print("°C");
//lcd.print(humidite_air,1);
//lcd.print("% ");

char message[16] = "";
sprintf(message,"T%dC H%d%% TR%dC", (int)temperature, (int)humidite_air, (int)temp_ressentie);
lcd.print(message);
lcd.setCursor(0, 1);
char message2[16] = "";

char direction_vent_[direction_vent.length()+1]; // tableau de char de la taille du String +1 (caractère de fin de ligne)
direction_vent.toCharArray(direction_vent_,direction_vent.length()+1); // récupère le param dans le tableau de char 
sprintf(message2,"%s", direction_vent_);
lcd.print(message2);
}

void compteurvent() {
  vent_ct ++;
}

void compteurpluie() {
  pluie_ct ++;
}
