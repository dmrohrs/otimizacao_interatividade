#define referencia A4 //Verificar qual pino vai conectar na hora
int valor_referencia; // Variavel que contem a leitura dos parâmetros(0-1023) da bateria
int valor_bateria; // //Valor real da Tensão da Bateria
int valor_porcentagem; // Valor da porcentagem da bateria

void setup() {
  Serial.begin(9600); //Taxa de comunicação
  pinMode(referencia, INPUT); // Declaração da variavel referencia como entrada
}

void loop() {
  valor_referencia = analogRead(referencia); // Variavel recebe a leitura dos parâmetros(0-1023) da bateria
  valor_bateria = (0.00488 * valor_referencia * 16.8)/5; //Valor real da Tensão da Bateria (Conta de conversão)
  valor_porcentagem = ((valor_referencia/1023)*100); // Valor da porcentagem da bateria
 
  Serial.print("n0.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da tensão
  Serial.println(valor_bateria); // Valor da tensão
  Serial.print("z0.val="); //Definir essa variavel para ser utilizada no Nextion para receber o valor da porcentagem da bateria
  Serial.print(valor_porcentagem); //Valor da porcentagem da bateria 
}
