#define pot_acelerador A0  // Pin do potenciometro do acelerador
#define pot_freio      A1  // Pin do potenciometro do freio
#define led_freio      2   // Pino do freio

void setup() {
  // put your setup code here, to run once:
  pinMode(pot_acelerador, INPUT);
  pinMode(pot_freio, INPUT);
  pinMode(led_freio, OUTPUT);
}

void loop() {
  // Acelerador
  float aceleracao = (analogRead(pot_acelerador));
  int aceleracao_int = (int)aceleracao;
  int velocidade_cooler = map(aceleracao_int, 0, 1023, 0, 320);
  int aceleracao_nextion = map(aceleracao_int, 0, 1023, 0, 100);

  //freio
  float freio = (analogRead(pot_acelerador));
  int freio_int = (int)aceleracao;
  int freio_nextion = map(aceleracao_int, 0, 1023, 0, 100);
  if(freio_nextion > 10){
    digitalWrite(led_freio, HIGH);
  }
}
