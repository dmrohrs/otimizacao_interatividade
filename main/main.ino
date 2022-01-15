//Código para a implementação do projeto de trainee do Tesla

//==================================================================================================
//                                    Controle do Auto-Falante

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
//===================================================================================================
//                              Parte de Aceleração, freio e Cooler
#include <Adafruit_NeoPixel.h>
#define pwm_cooler 10
#define pot_acelerador A0  // Pin do potenciometro do acelerador
#define pot_freio      A1  // Pin do potenciometro do freio
#define PIN_LEDFREIO   2      // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS_FREIO 20 // Popular NeoPixel ring size

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
#define referencia A4 //Verificar qual pino vai conectar na hora
int valor_referencia; // Variavel que contem a leitura dos parâmetros(0-1023) da bateria
int valor_bateria; // //Valor real da Tensão da Bateria
int valor_porcentagem; // Valor da porcentagem da bateria

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
    TCCR1A = 0;           // undo the configuration done by...
    TCCR1B = 0;           // ...the Arduino core library
    TCNT1  = 0;           // reset timer
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch; B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
    ICR1   = 320;         // TOP = 320

    // Set the PWM pins as output.
    pinMode(pwm_cooler, OUTPUT);
    pinMode(pot_acelerador, INPUT);
    pinMode(pot_freio, INPUT);
    Serial.begin(9600);

    //=============================================================
    //Setup do auto falante
    dfmp3.begin();
  
    uint16_t volume = dfmp3.getVolume();
    Serial.print("volume ");
    Serial.println(volume);
    dfmp3.setVolume(24);
  
    uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
    Serial.print("files ");
    Serial.println(count);
    //=============================================================
    //setup dos LEDS
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

    //============================================================
    //setup dados bateria
    pinMode(referencia, INPUT); // Declaração da variavel referencia como entrada
    
}

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
  //                Programação inicial (ultrassonico e auto falante)
  
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
  mediaDist = mediaDist/5;

  if(mediaDist < 20){            //toca a faixa de áudio quando a distancia for < 20cm
    delay(10000);
    dfmp3.playMp3FolderTrack(1);  // sd:/mp3/0001.mp3
    waitMilliseconds(10000); //ajustar para o tamanho da faixa de audio
    Serial.println("page page01"); 
  }
    
  //====================================================================
  //Aceleração, Freio e cooler  
  
  float aceleracao = (analogRead(pot_acelerador));
  int aceleracao_int = (int)aceleracao;
  int aceleracao_nextion = map(aceleracao_int, 0, 1023, 0, 100);
  int aceleracao_cooler = map(aceleracao_int, 0, 1023, 0, 320); //converte o valor recebido
  Serial.println(aceleracao_nextion);  //envia para p nextion
  analogWrite25k(pwm_cooler, aceleracao_cooler);
  
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

    
  //======================================================================
  //LEDS

  pixels.fill(pixels.Color(0, 255, 0), 0, NUMPIXELS); //cor padrão led

  if(Serial.avaliable()>0) //se receber qualquer coisa
  {
    char Received = Serial.read();

    if(Received == "pageled")
    {
      Received = Serial.read();
      if(Received == "modo 1")
      {
        int nextion_colorR = Serial.parseInt();
        int nextion_colorB = Serial.parseInt();
        int nextion_colorG = Serial.parseInt();
      }
    }
  }
  pixels.clear(); // Set all pixel colors to 'off'
  
  char Received = Serial.read();
  
  int nextion_colorR = 0;
  int nextion_colorG = 255;
  int nextion_colorB = 0;

    //modo de cor única
  
   pixels.fill(pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB), 0, NUMPIXELS);
  
   pixels.show();   // Send the updated pixel colors to the hardware.
   

   // modo em que a cor vai "deslizando" pela barra
   /*
   for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorG, nextion_colorB));
    pixels.setPixelColor(i - 5, pixels.Color(0, 0, 0));
 
    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(50); // Pause before next pass through loop
  }*/

  //============================================================================
  //Aquisição de dados Bateria
  valor_referencia = analogRead(referencia); // Variavel recebe a leitura dos parâmetros(0-1023) da bateria
  valor_bateria = (0.00488 * valor_referencia * 16.8)/5; //Valor real da Tensão da Bateria (Conta de conversão)
  valor_porcentagem = ((valor_referencia/1023)*100); // Valor da porcentagem da bateria
 
  Serial.print("n0.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da tensão
  Serial.println(valor_bateria); // Valor da tensão
  Serial.print("z0.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da porcentagem da bateria
  Serial.print(valor_porcentagem); //Valor da porcentagem da bateria

  //============================================================================
  //Termistor
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
  Serial.print("n1.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
  Serial.println (t_termistor - 273.0); //Valor da Temperatura
  //==============================================================================
  
  }
