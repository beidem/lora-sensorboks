// Vi legger til bibliotekene vi trenger. LoRaWan for å koble til The things network (TTN), DHT for å hente informasjon fra DHT11-sensoren, SPI for seriellkommunikasjon, og CayenneLPP for å laste opp informasjon via Cayenne sin LPP protokoll.
#include <LoRaWan.h>
// #include <SPI.h>
#include <DHT.h>
#include <CayenneLPP.h>

// Konfigurerer og setter opp DHT11-sensoren
#define DHTPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Definerer hvor stor bufferen til CayenneLPP skal være, altså hvor mange byte med data vi kan sende på én gang. 51 bytes er maks.
CayenneLPP lpp(51);

void setup()
{
  // Siden vi bruker seriellkommunikasjon mellom Seeed Xiao-mikrokontrolleren og LoRa-modulen vår, 
  // så må vi også sette opp seriellkommunikasjon via USB hvis vi vil bruke seriellmonitor i f.eks. Arduino IDE.
  // Ta vekk kommentarmerkene for de to neste linjene for å slå på dette. Merk at det er mulig dette må slås av dersom du
  // skal kjøre mikrokontrolleren på batteristrøm.
  // SerialUSB.begin(115200);
  // while(!SerialUSB);

  // Vi initialiserer DHT11-sensoren
  dht.begin();
  
  // Vi starter kommunikasjonen med LoRa-modulen
  lora.init();
    
  // Vi setter nøklene vi trenger for å koble oss til TTN via OTAA. 
  // Vi trenger "network session key", NwkSKey, "application session key", AppSKey, og "application key", AppKey - i den rekkefølgen. Dette finnes i oversikten/dashbordet i TTN.
  lora.setKey("00000000000000000000000000000000", "00000000000000000000000000000000", "00000000000000000000000000000000");
  
  // Setter modus til enheten (her er det en skrivefeil i biblioteket) til OTAA, "Over-The-Air Activation", og datahastigheten. 
  lora.setDeciveMode(LWOTAA);
  // Viktig at vi bruker "EU868", siden LoRaWAN bruker 868 MHz-båndet i Norge og Europa. DR0 angir type.
  lora.setDataRate(DR0, EU868);
  
  // Vi setter opp kanalene (frekvensområdet) vi bruker. Dette trenger sannsynligvis ikke endres, men må matche innstillingene til gatewayen.
  lora.setChannel(0, 868.1);
  lora.setChannel(1, 868.3);
  lora.setChannel(2, 868.5);
  
  // Setter opp frekvensvindu for mottak av informasjon fra gatewayen.
  lora.setReceiceWindowFirst(0, 868.1);
  lora.setReceiceWindowSecond(869.5, DR3);
  
  // Setter styring av "Duty cycle", hvor mye tid vi bruker på å sende informasjon. Dette er vanligvis ikke nødvendig.
  lora.setDutyCycle(false);
  lora.setJoinDutyCycle(false);
  
  // Setter styrken på signalet. Kan justeres mellom 10 og 30 etter behov.
  lora.setPower(14);
  
  // Hvis tilkobling til TTN feilet, stopp.
  while(!lora.setOTAAJoin(JOIN));

  // Pause i 2 sekunder.
  delay(2000);

}

void loop()
{ 
  // Pause i 2 sekunder
  delay(30000);
  // Kaller funksjonen printVariables, se linje 76.
  printVariables();

  // Oppretter en boolsk variabel for beskjeden som skal sendes. Kan bruke denne til å sjekke om beskjeden har blitt sent.
  bool message = false;
  // Sender pakke via LoRaWAN. Pakken består av LPP-bufferen, størrelsen til bufferen, og timeout for sending av pakken.
  // Hvis pakken blir sendt blir message satt til TRUE, ellers FALSE
  message = lora.transferPacket(lpp.getBuffer(), lpp.getSize(), 10);
}

// Funksjonen vi bruker for å skrive sensorinformasjon til LPP-bufferen.
void printVariables()
{
  // Tømmer bufferen.
  lpp.reset();

  // Leser av luftfuktighetsmåling fra DHT11-sensoren.
  float humidity = dht.readHumidity();;
  // Legger til luftfuktighetsmålingen til LPP-bufferen.
  // Her angir vi typen data slik at det blir dekodet direkte som luftfuktighet i Cayenne-dashboardet.
  // Dataene blir sendt i kanal 1 til Cayenne.
  lpp.addRelativeHumidity(1, humidity);

  // Leser av temperaturmåling fra DHT11-sensoren.
  float temp = dht.readTemperature();
  // Legger til temperaturmålingen til LPP-bufferen.
  // Her angir vi typen data slik at det blir dekodet direkte som temperatur i Cayenne-dashboardet.
  // Dataene blir sendt i kanal 2 til Cayenne.
  lpp.addTemperature(2, temp);
}

