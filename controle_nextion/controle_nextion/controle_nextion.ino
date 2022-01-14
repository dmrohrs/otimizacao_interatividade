void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.avaliable()>0) //se receber qualquer coisa
  {
    char Received = Serial.read();

    if(Received == "page 1")
    {
      
    }

    else if(Received == "page 2")
    {
      
    }
    
    else if(Received == "page 3")
    {
      
    }
    
    else if(Received == "page 4")
    {
      
    }
  }
}

void ledpadrao()
{
  
}
