#define pot_acelerador A0  // Pin do potenciometro do acelerador


void setup() {
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
    pinMode( 9, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(pot_acelerador, INPUT);
 
}

// PWM output @ 25 kHz, only on pins 9 and 10.
// Output value should be between 0 and 320, inclusive.
void analogWrite25k(int pin, int value)
{
    switch (pin) {
        case 9:
            OCR1A = value;
            break;
        case 5:
            OCR1B = value;
            break;
        default:
            // no other pin will work
            break;
    }
}

void loop() {
    
  float aceleracao = (analogRead(pot_acelerador));
  int aceleracao_int = (int)aceleracao;
  int velocidade_cooler = map(aceleracao_int, 0, 1023, 0, 320);
    
    analogWrite25k(10, velocidade_cooler);
}
