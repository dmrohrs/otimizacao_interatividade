//Código para a implementação do projeto de trainee do Tesla

//==================================================================================================
//                                    Controle do Auto-Falante
/*
//necessita ter uma faixa no cartão sd com esse nome
// sd:/mp3/0001.mp3
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>
#include <Ultrasonic.h>
// forward declare the notify class, just the name
class Mp3Notify;


//Definindo pinos
#define pino_trigger 4
#define pino_echo 3
Ultrasonic ultrasonic(pino_trigger, pino_echo);

SoftwareSerial secondarySerial(9, 10); // RX, TX
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
DfMp3 dfmp3(secondarySerial);

// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(DfMp3& mp3, uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3& mp3, DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};
void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    // if you have loops with delays, its important to 
    // call dfmp3.loop() periodically so it allows for notifications 
    // to be handled without interrupts
    dfmp3.loop(); 
    delay(1);
  }
}
*/
//===================================================================================================
//                              Parte de Aceleração, Freio e Cooler
#include <Adafruit_NeoPixel.h>
#define pwm_cooler 10
#define pot_acelerador A0  // Pin do potenciometro do acelerador
#define pot_freio      A1  // Pin do potenciometro do freio
#define PIN_LEDFREIO   11      // On Trinket or Gemma, suggest changing this to 1

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
#define PIN        9 // On Trinket or Gemma, suggest changing this to 1

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
float valor_referencia; // Variavel que contem a leitura dos parâmetros(0-1023) da bateria
float valor_bateria; // //Valor real da Tensão da Bateria
float valor_porcentagem; // Valor da porcentagem da bateria

//===================================================================================================
//                           Termistor
// Conexão do termistor
const int pinTermistor = A3;
 
// Parâmetros do termistor
const double beta_termistor = 3600.0;
const double r0_termistor = 10000.0;
const double t0_termistor = 273.0 + 25.0;
const double rx_termistor = r0_termistor * exp(-beta_termistor/t0_termistor);
 
// Parâmetros do circuito
const double vcc_termistor = 5.0;
const double R_termistor = 20000.0;
 
// Numero de amostras na leitura
const int nAmostras_termistor = 5;

void setup() {
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

    //=============================================================
    //Setup do auto falante
  /*
    dfmp3.begin();
  
    uint16_t volume = dfmp3.getVolume();
    dfmp3.setVolume(24);
  
    uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
   */
    //=============================================================
    //setup dos LEDS
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(255); //varia de 0 - 255
    //============================================================
    //setup dados bateria
    pinMode(referencia, INPUT); // Declaração da variavel referencia como entrada
    pinMode(pinTermistor, INPUT);
}
//===============================================================
// Função para o controle de velocidade do cooler
// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
    switch (pin) {
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
void loop() {
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
 if (Serial.available()>0)
 {
   mediaDist = (float)Serial.parseInt();
  Serial.println(mediaDist);
 }
  if(mediaDist < 20){            //toca a faixa de áudio quando a distancia for < 20cm
        //delay(10000);
        //dfmp3.playMp3FolderTrack(1);  // sd:/mp3/0001.mp3
        //waitMilliseconds(10000); //ajustar para o tamanho da faixa de audio
        //Serial.println("page page01"); 
      //===================================================================
      // Aceleração fora do nextion
      float aceleracao = (analogRead(pot_acelerador));
      int aceleracao_int = (int)aceleracao;
      int aceleracao_cooler = map(aceleracao_int, 0, 1023, 0, 320); //converte o valor recebido
      //analogWrite25k(pwm_cooler, aceleracao_cooler);
      
      float freio = (analogRead(pot_freio));
    if(Serial.available())
    Serial.println(freio);
      int freio_int = (int)freio;
      int freio_nextion = map(freio_int, 0, 1023, 0, 100); //variavel enviada para o nextion
      Serial.println(freio_nextion);
      if(freio_nextion > 10)
        {
        pixels_freio.fill(pixels.Color(255, 0,0), 0, NUMPIXELS_FREIO);
      
        pixels_freio.show();   // Send the updated pixel colors to the hardware.
        }
       else
       {
        pixels_freio.fill(pixels.Color(0, 0,0), 0, NUMPIXELS_FREIO);
      
        pixels_freio.show();   // Send the updated pixel colors to the hardware.
       }
    
       //===============================================================================
      while(mediaDist < 20){
        // Parte com o nextion
        if(Serial.available()>0) //se receber qualquer coisa
        {
          String Received = Serial.readString();
          
          if(Received == "paginavelocidade")
          {
            while(Serial.readString() != "voltar") //quando a pessoa sair da pagina
            {
            float aceleracao = (analogRead(pot_acelerador));
            int velocidade_int = (int)aceleracao;
            int velocidade_nextion = map(velocidade_int, 0, 1023, 0, 360);
            if(velocidade_nextion > 0 && velocidade_nextion < 40)
            {
              Serial.print("z0.val=");
              Serial.println(velocidade_nextion + 316);
            }
            else if(velocidade_nextion > 40)
            {
              Serial.print("z0.val=");
              Serial.println(velocidade_nextion - 40);
              if(velocidade_nextion > 218)
              {
                Serial.print("z0.val=218");
              }
            }
            int aceleracao_int =  (int)aceleracao;
            int aceleracao_brilho = map(aceleracao_int, 0, 1023, 0, 255);
             //pixels.setBrightness(aceleracao_brilho);
            pixels.fill(pixels.Color(aceleracao_brilho, 255-aceleracao_brilho,0), 0, NUMPIXELS);
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
            if(freio_nextion > 10)
              {
                pixels_freio.fill(pixels.Color(255, 0,0), 0, NUMPIXELS_FREIO);
        
                pixels_freio.show();   // Send the updated pixel colors to the hardware.
              }
            else
              {
                pixels_freio.fill(pixels.Color(0, 0,0), 0, NUMPIXELS_FREIO);
        
                pixels_freio.show();   // Send the updated pixel colors to the hardware.
              }
            }
          }
          
          else if(Received == "paginaaceleracao")
          {
            while(Serial.readString() != "voltar")
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
              if(freio_nextion > 10)
                {
                  pixels_freio.fill(pixels.Color(255, 0,0), 0, NUMPIXELS_FREIO);
        
                  pixels_freio.show();   // Send the updated pixel colors to the hardware.
                }
              else
                {
                  pixels_freio.fill(pixels.Color(0, 0,0), 0, NUMPIXELS_FREIO);
        
                  pixels_freio.show();   // Send the updated pixel colors to the hardware.
                }
            }
          }
          
          else if(Received == "paginagrafico")
          {
            while(Serial.readString() != "voltar")
            {
              
            }
          }
          
          else if(Received == "paginabateria")
          {
            while(Serial.readString() != "voltar")
            {
              valor_referencia = analogRead(referencia); // Variavel recebe a leitura dos parâmetros(0-1023) da bateria
              valor_bateria = (0.00488 * valor_referencia * 16.8)/5; //Valor real da Tensão da Bateria (Conta de conversão)
              valor_porcentagem = ((valor_referencia/1023)*100); // Valor da porcentagem da bateria
            Serial.print ("valor referencia ");
              Serial.println(valor_referencia);
              Serial.print("x1.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da tensão
              Serial.println(valor_bateria*10); // Valor da tensão
              Serial.print("x2.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da porcentagem da bateria
              Serial.println(valor_porcentagem*10); //Valor da porcentagem da bateria
   
              // Le o sensor algumas vezes
              int soma_termistor = 0;
              for (int i = 0; i < nAmostras_termistor; i++) {
              soma_termistor += analogRead(pinTermistor);
              }
       
              // Determina a resistência do termistor
              double v_termistor = (vcc_termistor*soma_termistor)/(nAmostras_termistor*1024.0);
              double rt_termistor = (vcc_termistor*R_termistor)/v_termistor - R_termistor;
       
              // Calcula a temperatura
              double t_termistor = beta_termistor / log(rt_termistor/rx_termistor);
              // Imprime os valores
              Serial.print("x0.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
              Serial.println ((t_termistor - 273)*10); //Valor da Temperatura
              Serial.print("j1.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
              Serial.println (t_termistor - 273); //Valor da Temperatura
            }
          }
          else if(Received == "paginaled")
          {
            int nextion_colorR = 0;
            int nextion_colorG = 0;
            int nextion_colorB = 0;
      String recebido;
            while(Serial.readString() != "voltar")
            {
              recebido = Serial.readString();
              if(recebido == "dinamico")
              {
                recebido = Serial.readString();
                if(recebido == "R")
                {
                  nextion_colorR = Serial.parseInt(); 
                }
                if(recebido == "G")
                {
                  nextion_colorG = Serial.parseInt(); 
                }
                if(recebido == "B")
                {
                  nextion_colorB = Serial.parseInt(); 
                }
                while(Serial.readString() != "cancelar")
                {
                  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
      
                  // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
                  // Here we're using a moderately bright green color:
                  pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB));
                  pixels.setPixelColor(i - 5, pixels.Color(0, 0, 0));
       
                  pixels.show();   // Send the updated pixel colors to the hardware.
      
                  delay(50); // Pause before next pass through loop
                }
                // Led padrão
                pixels.fill(pixels.Color(0, 255, 0), 0, NUMPIXELS); //cor padrão led
                pixels.show();
              }
            }
            else if(recebido == "estatico")
              {
              while(Serial.readString() != "cancelar")
                {
                Serial.println("to no estatico");
                 if (Serial.available());
                recebido = Serial.readString();
                if(recebido == "R")
                {
                  if (Serial.available())
                  nextion_colorR = Serial.parseInt(); 
                }
                if (Serial.available());
                recebido = Serial.readString();
                if(recebido == "G")
                {
                  if (Serial.available())
                  nextion_colorG = Serial.parseInt(); 
                }
                if (Serial.available());
              recebido = Serial.readString();
                if(recebido == "B")
                {
                  if (Serial.available())
                  nextion_colorB = Serial.parseInt(); 
                }
                if (nextion_colorR != 0 || nextion_colorG != 0 || nextion_colorB)
                { 
                  Serial.println (nextion_colorR);
                   Serial.println(nextion_colorG);
                  Serial.println(nextion_colorB);
                  pixels.fill(pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB), 0, NUMPIXELS);
                  pixels.show();
                }
              }
              }
            }
          }
        }
    }
  }
}
