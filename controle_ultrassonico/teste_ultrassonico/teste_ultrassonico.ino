//Programa: Conectando Sensor Ultrassonico HC-SR04 ao Arduino
//Autor: FILIPEFLOP
 
//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 3
#define pino_led 6
 
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);
 
void setup()
{
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
}
 
void loop()
{
  //Le as informacoes do sensor, em cm e pol
  float cmMsec[5];
  float mediaDist = 0;
  long microsec = ultrasonic.timing();
  for(int i = 0; i <4 ; i++)
  {
    cmMsec[i] = ultrasonic.convert(microsec, Ultrasonic::CM);
  }
  for(int i = 0; i <4 ; i++)
  {
    mediaDist += cmMsec[i];
  }
  mediaDist = mediaDist/5; //variavel final da distância
  //Exibe informacoes no serial monitor
  if(mediaDist < 20){
    digitalWrite(pino_led, HIGH);
  }
  else{
    digitalWrite(pino_led, LOW);
  }
  Serial.print("Distancia em cm: ");
  Serial.println(mediaDist);
      delay(100);
}
