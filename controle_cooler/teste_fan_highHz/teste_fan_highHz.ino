#define leitura_pot A0

const int controle_cooler = 6;
int count = 0;
unsigned long start_time;
int rpm;

void setup() {
  // put your setup code here, to run once:
  pinMode(leitura_pot, INPUT);
  analogWrite(controle_cooler, 0);
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  ICR1 = 320;
  pinMode(controle_cooler, OUTPUT);
  OCR1A = 0;
  OCR1B = 0;
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), counter, RISING);
}

void loop() {
  float aceleracao = (analogRead(leitura_pot))/4;
  aceleracao = aceleracao * 51; //fator de escala
  int aceleracao_int = (int)aceleracao;
  analogWrite (controle_cooler, aceleracao_int);
  for (int pwm = 64; pwm <= 320; pwm +=64)
  {
    OCR1A = pwm;
    delay(5000);
    start_time = millis();
    count = 0;
    
    while((millis() - start_time) < 1000){
    }
    rpm = count *30;
    Serial.print(map(pwm, 0, 320, 0, 100));
    Serial.print("% , Speed=");
    Serial.print(rpm);
    Serial.println("rpm");
  }
  Serial.print("aceleracao pot");
  Serial.println(aceleracao);
}
void counter(){
  count++;
}
