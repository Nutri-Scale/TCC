#include <LCD-I2C.h>
#include "HX711.h"                    // Biblioteca HX711 
#include <SoftwareSerial.h>
#include <Pushbutton.h>
#include <Wire.h>

#define DOUT  2                    // HX711 DATA OUT = pino A0 do Arduino 
#define CLK  3                       // HX711 SCK IN = pino A1 do Arduin
#define botliga 10

HX711 balanca;          // define instancia balança HX711
LCD_I2C lcd(0x27, 16, 2);
SoftwareSerial serialdobluetooth(8,9);

float calibration_factor = 125000;     // fator de calibração
int valorBluetooth; // informação enviada pelo usuário do bluetooth
int botTare = 6;
int tareApertado = 0;
int ligado = 0;



Pushbutton ligar(botliga); 


void setup()
{
  pinMode(botTare, INPUT_PULLUP);
  Wire.begin();
  lcd.begin(&Wire);
  lcd.display();
  lcd.backlight();
  lcd.setCursor(0,0);
  Serial.begin(9600);            // monitor serial 9600 Bps
  serialdobluetooth.begin(9600); //serial do bluetooth
  balanca.begin(DOUT, CLK);      // inicializa a balança
  balanca.set_scale();                                             // configura a escala da Balança
  
}

void zeraBalanca ()
{                                      
  balanca.tare();                                                 // zera a Balança
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("NutriScale");
  lcd.setCursor(5,1);
  lcd.print("Zerada");
  delay(500);
  lcd.clear();
}

void codigoPrincipal() {
  
  if(ligado == 0){
    balanca.power_down();
    lcd.backlightOff();
    delay(375);
    ligar.waitForButton(); //Espera o botão ser ligado
    balanca.power_up();
    zeraBalanca();
    delay(200);
    lcd.backlight();
    zeraBalanca();
  }

  ligado = 1;

    while(!ligar.getSingleDebouncedPress() && ligado == 1){ //vai ver se o botão vermelho foi apertado pra poder desligar a balança
        tareApertado = !digitalRead(botTare);

        if (tareApertado == 1) {
          zeraBalanca();
        }
      
      balanca.set_scale(calibration_factor);                     // ajusta fator de calibração

      float peso_Gramas = balanca.get_units(25)*1000;
      Serial.println(peso_Gramas);

      if (peso_Gramas <= 3 ) {
        peso_Gramas = 0;
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print(peso_Gramas, 1);
        serialdobluetooth.print(peso_Gramas);
        lcd.setCursor(10,0);
        lcd.print("g");
      }
      else{
        peso_Gramas == 3;
        const float pesoReal = peso_Gramas;

          if (peso_Gramas <= pesoReal-3 || peso_Gramas >= pesoReal+3)
          {
            const float pesoImpresso  = pesoReal;
            lcd.clear();
            lcd.setCursor(5,0);
            lcd.print(pesoImpresso, 1);
            serialdobluetooth.print(pesoImpresso);
            lcd.setCursor(10,0);
            lcd.print("g");
          }
          else{
            lcd.clear();
            lcd.setCursor(5,0);
            lcd.print(pesoReal, 1);
            serialdobluetooth.print(pesoReal);
            lcd.setCursor(10,0);
            lcd.print("g");
          }
      }
       

      
      


      if (serialdobluetooth.available()) {
        valorBluetooth = serialdobluetooth.read();
      }


      switch (valorBluetooth) 
      {
        case 't': 
        zeraBalanca();
        valorBluetooth = 0;
        break;
        case 'l':
        ligado = 1;
        break;
        case 'd':
        ligado = 0;
        break;
      }
    }
  
  ligado = 0;
  
}

void loop()
{
 Serial.println(ligado);

  codigoPrincipal();  
}
