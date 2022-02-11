//Código para a implementação do projeto de trainee do Tesla

//==================================================================================================
//                                    Controle do Auto-Falante
/*
//necessita ter uma faixa no cartão sd com esse nome
// sd:/mp3/0001.mp3
/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>

 ***************************************************
 This example shows the basic function of library for DFPlayer.

 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

//===================================================================================================
//                              Parte de Aceleração, Freio e Cooler
//#include <Adafruit_NeoPixel.h>
#define pwm_cooler 10
#define pot_acelerador A0 // Pin do potenciometro do acelerador
#define pot_freio A1      // Pin do potenciometro do freio
#define PIN_LEDFREIO 11   // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS_FREIO 4 // Popular NeoPixel ring size
float mediaDist = 25;
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels_freio(NUMPIXELS_FREIO, PIN_LEDFREIO, NEO_GRB + NEO_KHZ800);
//===================================================================================================
//                              Controle dos LEDS

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 9 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 20 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels

//===================================================================================================
//                           Dados da Bateria
#define referencia A4
float valor_referencia;  // Variavel que contem a leitura dos parâmetros(0-1023) da bateria
float valor_bateria;     // //Valor real da Tensão da Bateria
float valor_porcentagem; // Valor da porcentagem da bateria

//===================================================================================================
//                           Termistor
// Conexão do termistor
const int pinTermistor = A3;

// Parâmetros do termistor
const double beta_termistor = 3600.0;
const double r0_termistor = 10000.0;
const double t0_termistor = 273.0 + 25.0;
const double rx_termistor = r0_termistor * exp(-beta_termistor / t0_termistor);

// Parâmetros do circuito
const double vcc_termistor = 5.0;
const double R_termistor = 20000.0;

// Numero de amostras na leitura
const int nAmostras_termistor = 5;

void setup()
{
    //==============================================================
    //Setup do cooler/ aceleração e freio
    // Configure Timer 1 for PWM @ 25 kHz.
    /*
    TCCR1A = 0;           // undo the configuration done by...
    TCCR1B = 0;           // ...the Arduino core library
    TCNT1  = 0;           // reset timer
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch; B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
    ICR1   = 320;         // TOP = 320
    */
    // Set the PWM pins as output.
    pinMode(pwm_cooler, OUTPUT);
    pinMode(pot_acelerador, INPUT);
    pinMode(pot_freio, INPUT);
    Serial.begin(9600);
    Serial.setTimeout(10);

    //=============================================================
    //Setup do auto falante
    SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
    DFRobotDFPlayerMini myDFPlayer;
    void printDetail(uint8_t type, int value);
    mySoftwareSerial.begin(9600);
    Serial.begin(115200);
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while(true);
    }
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(10);  //Set volume value. From 0 to 30
    //myDFPlayer.play(1);  //Play the first mp3
    //Serial.end();

    //=============================================================
    //Setup dos LEDS
    pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(255); //varia de 0 - 255
    //============================================================
    //Setup dados bateria
    pinMode(referencia, INPUT); // Declaração da variavel referencia como entrada
    pinMode(pinTermistor, INPUT);
}
//===============================================================
// Função para o controle de velocidade do cooler
// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
    switch (pin)
    {
    case 9:
        OCR1A = value;
        break;
    case 10:
        OCR1B = value;
        break;
    default:
        // no other pin will work
        break;
    }
}
void loop()
{
    //====================================================================
    // Led padrão
    pixels.fill(pixels.Color(0, 255, 0), 0, NUMPIXELS); //cor padrão led
    pixels.show();

    //====================================================================
    //                Programação inicial (ultrassonico e auto falante)

    float cmMsec[5];
    /*long microsec = ultrasonic.timing();
  for(int i = 0; i <4 ; i++)
  {
    cmMsec[i] = ultrasonic.convert(microsec, Ultrasonic::CM);
  }
  for(int i = 0; i <4 ; i++)
  {
    mediaDist += cmMsec[i];
  }
  mediaDist = mediaDist/5;
  */
    if (Serial.available() > 0)
    {
        mediaDist = (float)Serial.parseInt();
        Serial.println(mediaDist);
    }
    if (mediaDist < 20)
    { //toca a faixa de áudio quando a distancia for < 20cm
        static unsigned long timer = millis();
        /*
        if (millis() - timer > 3000) {
            timer = millis();
            myDFPlayer.next();  //Play next mp3 every 3 second.
        */
        myDFPlayer.play(1);  //Play the first mp3
        }
        if (myDFPlayer.available()) {
            printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
                               }

        //===================================================================
        // Aceleração fora do nextion
        float aceleracao = (analogRead(pot_acelerador));
        int aceleracao_int = (int)aceleracao;
        int aceleracao_cooler = map(aceleracao_int, 0, 1023, 0, 320); //converte o valor recebido
        //analogWrite25k(pwm_cooler, aceleracao_cooler);

        float freio = (analogRead(pot_freio));
        if (Serial.available())
            Serial.println(freio);
        int freio_int = (int)freio;
        int freio_nextion = map(freio_int, 0, 1023, 0, 100); //variavel enviada para o nextion
        Serial.println(freio_nextion);
        if (freio_nextion > 10)
        {
            pixels_freio.fill(pixels.Color(255, 0, 0), 0, NUMPIXELS_FREIO);

            pixels_freio.show(); // Send the updated pixel colors to the hardware.
        }
        else
        {
            pixels_freio.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS_FREIO);

            pixels_freio.show(); // Send the updated pixel colors to the hardware.
        }
    }

        //===============================================================================
        while (mediaDist < 20)
        {
            // Parte com o nextion
            Serial.println("Inicio void loop");
            if (Serial.available() > 0) //se receber qualquer coisa
            {
                String Received = Serial.readString();
                if (Received == "paginavelocidade")
                {
                    Serial.println("Pagina velocidade");
                    while (Serial.readString() != "voltar") //quando a pessoa sair da pagina
                    {
                        float aceleracao = (analogRead(pot_acelerador));
                        int velocidade_int = (int)aceleracao;
                        int velocidade_nextion = map(velocidade_int, 0, 1023, 0, 360);
                        if (velocidade_nextion > 0 && velocidade_nextion < 40)
                        {
                            Serial.print("z0.val=");
                            Serial.println(velocidade_nextion + 316);
                        }
                        else if (velocidade_nextion > 40)
                        {
                            Serial.print("z0.val=");
                            Serial.println(velocidade_nextion - 40);
                            if (velocidade_nextion > 218)
                            {
                                Serial.print("z0.val=218");
                            }
                        }
                        int aceleracao_int = (int)aceleracao;
                        int aceleracao_brilho = map(aceleracao_int, 0, 1023, 0, 255);
                        //pixels.setBrightness(aceleracao_brilho);
                        pixels.fill(pixels.Color(aceleracao_brilho, 255 - aceleracao_brilho, 0), 0, NUMPIXELS);
                        pixels.show();
                        int aceleracao_nextion = map(aceleracao_int, 0, 1023, 292, 65);
                        Serial.print("z1.val=");
                        Serial.println(aceleracao_nextion);
                        int velocidade_digital = map(aceleracao_int, 0, 1023, 0, 260);
                        Serial.print("n0.val=");
                        Serial.println(aceleracao_nextion);
                        aceleracao_cooler = map(aceleracao_int, 0, 1023, 0, 320); //converte o valor recebido
                        //analogWrite25k(pwm_cooler, aceleracao_cooler);
                        float freio = (analogRead(pot_freio));
                        int freio_int = (int)freio;
                        int freio_nextion = map(freio_int, 0, 1023, 0, 100); //variavel enviada para o nextion
                        if (freio_nextion > 10)
                        {
                            pixels_freio.fill(pixels.Color(255, 0, 0), 0, NUMPIXELS_FREIO);

                            pixels_freio.show(); // Send the updated pixel colors to the hardware.
                        }
                        else
                        {
                            pixels_freio.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS_FREIO);

                            pixels_freio.show(); // Send the updated pixel colors to the hardware.
                        }
                    }
                }

                else if (Received == "paginaaceleracao")
                {
                    Serial.println("Pagina aceleracao");
                    while (Serial.readString() != "voltar")
                    {
                        float aceleracao = (analogRead(pot_acelerador));
                        int aceleracao_int = (int)aceleracao;
                        int aceleracao_nextion = map(aceleracao_int, 0, 1023, 0, 360);
                        Serial.print("j0.val=");
                        Serial.println(aceleracao_nextion);

                        float freio = (analogRead(pot_freio));
                        int freio_int = (int)freio;
                        int freio_nextion = map(freio_int, 0, 1023, 0, 100); //variavel enviada para o nextion
                        Serial.print("j1.val=");
                        Serial.println(freio_nextion);
                        aceleracao_cooler = map(aceleracao_int, 0, 1023, 0, 320); //converte o valor recebido
                                                                                  //analogWrite25k(pwm_cooler, aceleracao_cooler);
                        if (freio_nextion > 10)
                        {
                            pixels_freio.fill(pixels.Color(255, 0, 0), 0, NUMPIXELS_FREIO);

                            pixels_freio.show(); // Send the updated pixel colors to the hardware.
                        }
                        else
                        {
                            pixels_freio.fill(pixels.Color(0, 0, 0), 0, NUMPIXELS_FREIO);

                            pixels_freio.show(); // Send the updated pixel colors to the hardware.
                        }
                    }
                }

                else if (Received == "paginagrafico")
                {
                    Serial.println("Pagina grafico");

                    while (Serial.readString() != "voltar")
                    {
                    }
                }

                else if (Received == "paginabateria")
                {
                    Serial.println("Pagina bateria");
                    while (Serial.readString() != "voltar")
                    {
                        valor_referencia = analogRead(referencia);               // Variavel recebe a leitura dos parâmetros(0-1023) da bateria
                        valor_bateria = (0.00488 * valor_referencia * 16.8) / 5; //Valor real da Tensão da Bateria (Conta de conversão)
                        valor_porcentagem = ((valor_referencia / 1023) * 100);   // Valor da porcentagem da bateria
                        Serial.print("valor referencia ");
                        Serial.println(valor_referencia);
                        Serial.print("x1.val=");                //Definir essa variavel para ser utilizada no Nextion para receber o valor da tensão
                        Serial.println(valor_bateria * 10);     // Valor da tensão
                        Serial.print("x2.val=");                //Definir essa variavel para ser utilizada no Nextion para receber o valor da porcentagem da bateria
                        Serial.println(valor_porcentagem * 10); //Valor da porcentagem da bateria

                        // Le o sensor algumas vezes
                        int soma_termistor = 0;
                        for (int i = 0; i < nAmostras_termistor; i++)
                        {
                            soma_termistor += analogRead(pinTermistor);
                        }

                        // Determina a resistência do termistor
                        double v_termistor = (vcc_termistor * soma_termistor) / (nAmostras_termistor * 1024.0);
                        double rt_termistor = (vcc_termistor * R_termistor) / v_termistor - R_termistor;

                        // Calcula a temperatura
                        double t_termistor = beta_termistor / log(rt_termistor / rx_termistor);
                        // Imprime os valores
                        Serial.print("x0.val=");                  //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
                        Serial.println((t_termistor - 273) * 10); //Valor da Temperatura
                        Serial.print("j1.val=");                  //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
                        Serial.println(t_termistor - 273);        //Valor da Temperatura
                    }
                }
                else if (Received == "paginaled")
                {
                    Serial.println("Pagina Led");
                    String nR, nG, nB;
                    int nextion_colorR = 0;
                    int nextion_colorG = 0;
                    int nextion_colorB = 0;
                    String recebido = "0";
                    while (recebido != "voltar")
                    {
                        while (Serial.available() == 0)
                            delay(1);
                        if (Serial.available())
                            recebido = Serial.readString();
                        Serial.println(recebido);
                        if (recebido == "dinamico")
                        {
                            Serial.print("Dinamico");
                              while (Serial.available() == 0)
                                delay(1);
                            if (Serial.available())
                            {
                                nR = Serial.readString();
                                delay(10);
                                while (Serial.available() == 0)
                                    delay(1);
                                nG = Serial.readString();
                                delay(1);
                                while (Serial.available() == 0)
                                    delay(1);
                                nB = Serial.readString();

                                nextion_colorR = nR.toInt();
                                nextion_colorG = nG.toInt();
                                nextion_colorB = nB.toInt();
                                Serial.print("R = ");
                                Serial.println(nextion_colorR);
                                Serial.print("G = ");
                                Serial.println(nextion_colorG);
                                Serial.print("B = ");
                                Serial.println(nextion_colorB);
                              
                while (recebido != "cancelar")
                              {
                                    for (int i = 0; i < NUMPIXELS; i++)
                                    { // For each pixel...

                                        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
                                        // Here we're using a moderately bright green color:
                                        pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB));
                                        pixels.setPixelColor(i - 5, pixels.Color(0, 0, 0));

                                        pixels.show(); // Send the updated pixel colors to the hardware.

                                        delay(50); // Pause before next pass through loop

                                        // Led padrão
                                        pixels.fill(pixels.Color(0, 255, 0), 0, NUMPIXELS); //cor padrão led
                                        pixels.show();
                                      if (Serial.available())
                                        recebido = Serial.readString();
                                        if (recebido == "cancelar")
                                            i = NUMPIXELS;
                                    }
                              }
                            }
                        }
                        else if (recebido == "estatico")
                        {
                            Serial.println("Estatico");

                            while (Serial.available() == 0)
                                delay(1);
                            if (Serial.available())
                            {
                                nR = Serial.readString();
                                delay(10);
                                while (Serial.available() == 0)
                                    delay(1);
                                nG = Serial.readString();
                                delay(1);
                                while (Serial.available() == 0)
                                    delay(1);
                                nB = Serial.readString();

                                nextion_colorR = nR.toInt();
                                nextion_colorG = nG.toInt();
                                nextion_colorB = nB.toInt();
                                Serial.print("R = ");
                                Serial.println(nextion_colorR);
                                Serial.print("G = ");
                                Serial.println(nextion_colorG);
                                Serial.print("B = ");
                                Serial.println(nextion_colorB);
                                while (recebido != "cancelar")
                                {
                                    pixels.fill(pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB), 0, NUMPIXELS);
                                    pixels.show();
                                    while (Serial.available() == 0)
                                        delay(1);
                                    if (Serial.available())
                                        recebido = Serial.readString();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
