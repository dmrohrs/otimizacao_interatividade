/*
 * Leitura de temperatura usando um termistor
 */
 
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
 
// Iniciação
void setup() {
  Serial.begin(9600);
}
 
// Laço perpétuo
void loop() {
  // Le o sensor algumas vezes
  int soma_termistor = 0;
  for (int i = 0; i < nAmostras_termistor; i++) {
    soma_termistor += analogRead(pinTermistor);
    delay (10);
  }
 
  // Determina a resistência do termistor
  double v_termistor = (vcc_termistor*soma_termistor)/(nAmostras_termistor*1024.0);
  double rt_termistor = (vcc_termistor*R_termistor)/v_termistor - R_termistor;
 
  // Calcula a temperatura
  double t_termistor = beta_termistor / log(rt_termistor/rx_termistor);
  // Imprime os valores
  Serial.print("n1.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da Temperatura
  Serial.println (t_termistor - 273.0); //Valor da Temperatura
 
}
