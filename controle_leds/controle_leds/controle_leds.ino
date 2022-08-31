
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        9 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 100 // Popular NeoPixel ring size

#define brilhomaximo 255

void ModoCorUnica();
void ModoDeslizante();
void ModoDeslizanteIdaVolta();
void ModoPulsante();
void Incrementa();

const byte interruptPin = 2;
int Funcao = 0;
int brilho = 0;

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels
int nextion_colorR = 191;
int nextion_colorG = 255;
int nextion_colorB = 0;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  // END of Trinket-specific code.
  pixels.clear();
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(51); // Set BRIGHTNESS to about 1/5 (max = 255)
  pinMode(interruptPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(interruptPin),Incrementa, CHANGE);
}

    
void loop() {
// pixels.clear(); // Set all pixel colors to 'off'
 /*
  switch (Funcao) 
 {
  case 0:
  ModoCorUnica();

  case 1:
  ModoDeslizante();

  case 2:
  ModoPulsante();

 }
  */
  //ModoCorUnica();
  //ModoPulsante();
  //ModoDeslizante();
  ModoDeslizanteIdaVolta();
}
void ModoCorUnica()
  {
   pixels.fill(pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG), 0, NUMPIXELS);
   pixels.show();   // Send the updated pixel colors to the hardware.
   //delay(1000);
  }
   // modo em que a cor vai "deslizando" pela barra
   void ModoDeslizante()
   {
      for(int i=0; i<(NUMPIXELS+10); i++) 
      { // For each pixel...
    
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG));
      pixels.setPixelColor(i-10, pixels.Color(0, 0, 0));
   
      pixels.show();   // Send the updated pixel colors to the hardware.
  
      delay(10); // Pause before next pass through loop
    }
   }
  

  void ModoPulsante(){
    while(brilho < 255){
      pixels.fill(pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG), 0, NUMPIXELS);
      pixels.setBrightness(brilho);
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(5);
      brilho+=1;
    }
    while(brilho > 5){
      pixels.fill(pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG), 0, NUMPIXELS);
      pixels.setBrightness(brilho);
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(5);
      brilho-=1;
    }
    
  }
  void ModoDeslizanteIdaVolta()
   {
      for(int i=0; i<(NUMPIXELS+1); i++) 
      { // For each pixel...
    
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG));
      pixels.setPixelColor(i-1, pixels.Color(0, 0, 0));
   
      pixels.show();   // Send the updated pixel colors to the hardware.
  
      delay(50); // Pause before next pass through loop
    }
    for(int i=NUMPIXELS; i > 1; i--) 
      { // For each pixel...
    
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(nextion_colorR, nextion_colorB, nextion_colorG));
      pixels.setPixelColor(i+1, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
  
      delay(50); // Pause before next pass through loop
    }
   }
void Incrementa()
{
 if(Funcao < 2)
 {
  Funcao++;
 }
 else
 {
  Funcao = 0;
 }
}
