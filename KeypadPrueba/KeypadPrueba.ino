#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;// Filas
const byte COLS = 4;//Columnas

char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'=','0','.','/'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

//SE CREA Y CONFIGURA EL OBJETO KEYPAD 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Ferchodev");
  char key = keypad.getKey();
  if (key != NO_KEY){//SI ES DIFERENTE DE NULO
    Serial.println(key);    
    lcd.setCursor(0,1);
    lcd.print(key);
  }
  
}
