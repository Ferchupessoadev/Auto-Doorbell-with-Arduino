#include <RtcDS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// pines
#define pinRele 13

// Config of lcd.
LiquidCrystal_I2C lcd(0x20, 16, 2);

// config of Rtc module ds1302
ThreeWire myWire(11, 10, 12); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
// CONEXIONES DS1302:
// DS1302 CLK/SCLK --> 10
// DS1302 DAT/IO --> 11
// DS1302 RST/CE --> 12
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

bool timbre_sonando = false;
// CONFIG KEYPAD
const byte ROWS = 4;// Filas
const byte COLS = 4;//Columnas

// var config __DATE__ and __TIME__
int compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

//SE CREA Y CONFIGURA EL OBJETO KEYPAD 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup()
{
    // iniciamos el reloj.
    Rtc.Begin();

    // iniciamos el lcd
    lcd.init();
    lcd.backlight();

    // buzzer pin
    pinMode(pinRele, OUTPUT);
}

bool es_recreo_o_cambio_de_hora(int hora, int minuto, int segundo)
{
    static const int timbres[26][2] = {
        // mañana.
        // hora, minutos
        {7, 10},
        {7, 15},
        {8, 0},
        {8, 40},
        {8, 50},
        {9, 30},
        {10, 10},
        {10, 20},
        {11, 0},
        {11, 40},
        {11, 45},
        {12, 25},
        {13, 5},
        // tarde.
        {13, 25},
        {13, 30},
        {14, 10},
        {14, 50},
        {15, 0},
        {15, 40},
        {16, 20},
        {16, 30},
        {17, 10},
        {17, 50},
        {17, 55},
        {18, 35},
        {19, 15},
    };
    for (short i = 0; i < 26; i++)
    {
        if (timbres[i][0] == hora and timbres[i][1] == minuto and segundo > 0 and segundo <= 10)
        {
            return true;
        }
    }
    return false;
};

int secondActual = 0;
int newSecond = secondActual;

void printDateTime(RtcDateTime now)
{
    secondActual = now.Second();
    if (newSecond != secondActual and !timbre_sonando)
    {
        lcd.clear();
    };
    newSecond = secondActual;
    lcd.setCursor(0, 0);
    if (!timbre_sonando)
    {
        lcd.print(now.Hour());
        lcd.print(":");
        lcd.print(now.Minute());
        lcd.print(":");
        lcd.print(now.Second());
    }
    else
    {
        lcd.print("Timbre sonando");
    }
};

void loop()
{
        static bool config_mode = false;
        char key = keypad.getKey();
        if (key != NO_KEY || config_mode) {
          if(key == '#') {
            int compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond
            lcd.setCursor(0,0);
            if (!config_mode) {
              config_mode = true;
              lcd.clear();
              lcd.print("Configuracion de la fecha y hora.");
              delay(1000);
            };
          } 
          else (config_mode) {
            switch(currentStep) {
              case 0:
                
                break;
              case 1:

                break:
              case 2:

                break;

              case 3:
                break;
            }
          }
          
        } else {
          RtcDateTime now = Rtc.GetDateTime();
          printDateTime(now);
          const char *dayOfWeekStr[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
          const char *dayOfWeek = dayOfWeekStr[now.DayOfWeek()];
          if (dayOfWeek.equals("Sabado") and dayOfWeek.equals("Domingo"))
          {
              if (es_recreo_o_cambio_de_hora(now.Hour(), now.Minute(), now.Second()))
              {
                  digitalWrite(pinRele, HIGH);
                  timbre_sonando = true;
              }
              else if (timbre_sonando)
              {
                  digitalWrite(pinRele, LOW);
                  timbre_sonando = false;
                  lcd.clear();
              }
          };
        }
}
