#include<Arduino.h>
#include<ESPAsyncWebServer.h>
#include<SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <Wire.h>


const char *ssid = "Vodafone CMR-D20B";
const char *password = "30786932";
const int ledventi = 23;
const int ledpompe = 22;
const int ledled = 19;
const int led = 5;
const int venti = 15;
const int pompe = 4;
const int humisol = 25;//analogique
const int pinlum = 36;//analogique
const int dhtpin = 26;
int pourcentage = 0;
int etat=1;
int var;
//Servo myServo;
//etat led
bool etatLed = 0;
char texteEtatLed[2][10]={"éteinte","allumée"};

//etat pompe
bool etatpompe = 0;
char texteEtatpompe[2][10]={"en repos","en action"};

//etat venti
bool etatventi = 0;
char texteEtatventi[2][10]={"en repos","en action"};


DHT dht(dhtpin, DHT11);

//interface
//
//
//
//





AsyncWebServer server(80);

void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(115200);
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
   pinMode(ledled, OUTPUT);
    pinMode(ledventi, OUTPUT);
     pinMode(ledpompe, OUTPUT);
  digitalWrite(led, LOW);
   digitalWrite(ledled, LOW);
    digitalWrite(ledpompe, LOW);
     digitalWrite(ledventi, LOW);
  pinMode(pompe,OUTPUT);
   digitalWrite(pompe, HIGH);
  pinMode(venti,OUTPUT);
   digitalWrite(venti, HIGH);
  pinMode(humisol, INPUT);
  pinMode(pinlum, INPUT);

  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
  WiFi.begin(ssid, password);
	Serial.print("Tentative de connexion...");
	
	while(WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(100);
	}
	
	Serial.println("\n");
	Serial.println("Connexion etablie!");
	Serial.print("Adresse IP: ");
	Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/serre_connecte.html", "text/html");
  });
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  ///lecture de contenue 


  server.on("/lireLuminosite", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int val = analogRead(pinlum);
    String luminosite = String(val);
    request->send(200, "text/plain", luminosite);
  });

  server.on("/liretemperature", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String temperature;
    float t = dht.readTemperature()*10 ;
    if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
        temperature = String(21.8);
  }
  else {
    Serial.println(t);
       temperature = String(t);
  }
    request->send(200, "text/plain", temperature);
  });

server.on("/lirehumidite_sol", HTTP_GET, [](AsyncWebServerRequest *request)
  {
float s = analogRead(humisol);
  float f = map(s,4095, 0, 0,100);
    String humsol = String(f);
    
    request->send(200, "text/plain", humsol);
  });

server.on("/lirehumidite_air", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String humair;
  float h = dht.readHumidity();
    if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
     humair = String(27);
  }
  else {
    Serial.println(h);
    humair = String(h);
  }
    request->send(200, "text/plain", humair);
  });


///etat



server.on("/etatLed", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    char* a = texteEtatLed[etatLed];
    String etatLed = String(a);
    request->send(200, "text/plain", etatLed);
  });

server.on("/etatpompe", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    char* b = texteEtatpompe[etatpompe];
    String etatpompe = String(b);
    request->send(200, "text/plain", etatpompe);
  });
  server.on("/etatventi", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    char* c = texteEtatventi[etatventi];
    String etatventi = String(c);
    request->send(200, "text/plain", etatventi);
  });
  
//////control


  server.on("/onled", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, HIGH);
     digitalWrite(ledled, HIGH);
    etatLed = 1;
    request->send(200);
  });
server.on("/connecte", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    etat =etat+1;
    request->send(200);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, LOW);
     digitalWrite(ledled, LOW);
    etatLed = 0;
    request->send(200);
  });

  server.on("/onpompe", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(pompe, LOW);
     digitalWrite(ledpompe, HIGH);
    etatpompe = 1;
    request->send(200);
  });
    server.on("/offpompe", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(pompe, HIGH);
     digitalWrite(ledpompe, LOW);
    etatpompe = 0;;
    request->send(200);
  });


  server.on("/onventi", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(venti, LOW);
     digitalWrite(ledventi, HIGH);
    etatventi = 1;
    request->send(200);
  });

  server.on("/offventi", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(venti, HIGH);
     digitalWrite(ledventi, LOW);
    etatventi = 0;
    request->send(200);
  });

  server.begin();
  Serial.println("Serveur actif!");
 }

void loop()
{
var = etat%2;
if((analogRead(pinlum)<1000 ) && (var == 0)){
  digitalWrite(led, HIGH);
     digitalWrite(ledled, HIGH);
    etatLed = 1;
}
else if((analogRead(pinlum)>1000 ) && (var == 0)){
   digitalWrite(led, LOW);
     digitalWrite(ledled, LOW);
    etatLed = 0;
}
if((dht.readHumidity() < 11) && (var == 0)){
  digitalWrite(pompe, LOW);
  digitalWrite(ledpompe, LOW);
  delay(4000);
  digitalWrite(pompe, HIGH);
  digitalWrite(ledpompe, HIGH);
  
}
}
